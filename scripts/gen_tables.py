import sys
import re

#------------------------------------------------------------------------------
# Constants
#------------------------------------------------------------------------------

MaxCopePoint = 0x0010FFFF

# Block sizes the trie builder weighs against each other; the block size
# decides how much the second stage deduplicates.
BlockShifts = [5, 6, 7, 8, 9]

# Every type a table can hold a value of: what one value costs, which decides
# whether a pool of distinct values is worth an extra indirection, and whether
# its values are enumerators (spelled `T::Name`) rather than literals.
ValueTypes = {
    'int': (4, False),
    'uint8_t': (1, False),
    'uint16_t': (2, False),
    'uint32_t': (4, False),
    'uint64_t': (8, False),
    'const char *': (8, False),
    'const char32_t *': (8, False),
    'GeneralCategory': (4, True),
    'Block': (4, True),
    'Script': (4, True),
    'GraphemeBreak': (4, True),
    'WordBreak': (4, True),
    'SentenceBreak': (4, True),
    'Emoji': (4, True),
    'EastAsianWidth': (4, True),
}

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

def to_unicode_literal(str):
    if len(str):
        return 'U"%s"' % ''.join([('\\U%08X' % x) for x in str])
    return '0'

def sorted_by_code_point(rows, unique=True):
    rows = sorted(rows, key=lambda r: r[0])
    if unique:
        for a, b in zip(rows, rows[1:]):
            assert a[0] != b[0], 'duplicate code point 0x%08X' % a[0]
    return rows

def assign_index(indices, cp, i):
    indices += [0] * (cp + 1 - len(indices))
    indices[cp] = i

#------------------------------------------------------------------------------
# generateTable
#------------------------------------------------------------------------------

def typeSize(type):
    return ValueTypes[type][0]

def needsTypePrefix(type):
    """Enum values are spelled `T::Name`; every other type's are literals."""
    return ValueTypes[type][1]

def indexWidth(n):
    return 1 if n <= 0x100 else 2

def indexType(n):
    return 'uint{}_t'.format(indexWidth(n) * 8)

def buildTrie(indices, blockShift):
    """(stage1, stage2) for one block size, over indices into the value pool.

    stage1 maps a block of 2**blockShift code points to a block of stage2, and
    stage2 holds one entry per code point in it. Blocks with equal contents
    share one stage2 block, which is where the compression comes from: most of
    the code space repeats.
    """
    blockSize = 1 << blockShift
    stage1 = []
    stage2 = []
    seen = {}
    for i in range(0, len(indices), blockSize):
        key = tuple(indices[i:i + blockSize])
        block = seen.get(key)
        if block is None:
            block = seen[key] = len(stage2) >> blockShift
            stage2.extend(key)
        stage1.append(block)
    return stage1, stage2

def generateTable(name, type, defval, out, values, blockSize=None):
    """Emit `values`, one entry per code point from U+0000 on, as a
    deduplicated two-stage trie.

    A table may stop before the end of the code point space -- the case
    mapping ones do, right after their last mapped code point. It is padded
    out to whole blocks with the default (which dedups to a single block) and
    bounds-checks against its own length rather than 0x10FFFF.

    `blockSize` pins the block size instead of searching for the smallest;
    see the note at the case mapping call sites for the one place where the
    smallest table is not the fastest one.
    """
    def formatValue(val):
        if not needsTypePrefix(type):
            return str(val)
        return 'D' if val == defval else 'T::{}'.format(val)

    assert len(values) <= MaxCopePoint + 1, \
        '%s: %d values for %d code points' % (name, len(values),
                                              MaxCopePoint + 1)
    truncated = len(values) <= MaxCopePoint

    # The pool does not depend on the block size, so it is built once and the
    # search runs over indices into it.
    pool = []
    poolIndex = {}
    for v in values:
        if v not in poolIndex:
            poolIndex[v] = len(pool)
            pool.append(v)
    if truncated and defval not in poolIndex:
        poolIndex[defval] = len(pool)
        pool.append(defval)
    indices = [poolIndex[v] for v in values]

    # Values no wider than an index into them make the pool a load that buys
    # nothing, so stage2 holds them directly instead.
    valueSize = typeSize(type)
    pooled = valueSize > indexWidth(len(pool))
    entrySize = indexWidth(len(pool)) if pooled else valueSize

    best = None
    for shift in ([blockSize.bit_length() - 1] if blockSize else BlockShifts):
        pad = -len(indices) % (1 << shift)
        padded = indices + [poolIndex[defval]] * pad if pad else indices
        stage1, stage2 = buildTrie(padded, shift)
        size = (len(stage1) * indexWidth(len(stage2) >> shift)
                + len(stage2) * entrySize
                + (len(pool) * valueSize if pooled else 0))
        if best is None or size < best[0]:
            best = (size, shift, stage1, stage2, len(padded))
    _, shift, stage1, stage2, covered = best

    out.write('namespace {} {{\n'.format(name))
    out.write('using T = {};\nconstexpr T D = {};\n'.format(
        type,
        '{}::{}'.format(type, defval) if needsTypePrefix(type) else defval))

    def writeArray(elemType, arrayName, items, fmt=str):
        out.write('inline constexpr {} {}[] = {{\n'.format(
            elemType, arrayName))
        for i, v in enumerate(items):
            if i % 16 == 0:
                out.write('\n ' if i else ' ')
            out.write(fmt(v) + ',')
        out.write('\n};\n')

    writeArray(indexType(len(stage2) >> shift), '_stage1', stage1)
    if pooled:
        writeArray(indexType(len(pool)), '_stage2', stage2)
        writeArray('T', '_pool', pool, formatValue)
        read = '_pool[_stage2[{}]]'
    else:
        writeArray('T', '_stage2', [pool[i] for i in stage2], formatValue)
        read = '_stage2[{}]'
    slot = '(_stage1[cp >> {0}] << {0}) + (cp & {1})'.format(
        shift, (1 << shift) - 1)
    bound = ('cp >= 0x{:X}'.format(covered) if truncated
             else 'cp > 0x{:X}'.format(MaxCopePoint))

    out.write("""inline T get_value(char32_t cp) {{
  if ({0}) {{
    return D;
  }}
  return {1};
}}
}}
""".format(bound, read.format(slot)))

#------------------------------------------------------------------------------
# generateRecordTable
#------------------------------------------------------------------------------

def generateRecordTable(name, type, out, fields, blockSize=None):
    """Emit a struct-valued table as one trie per field.

    `fields` is [(fieldName, fieldType, defaultValue, values)] in declaration
    order. Each field gets its own table inside the namespace, and get_value()
    puts the struct back together.

    A field is stored once and reached only through its own table, so a caller
    that wants one field links one table. That is not a size micro-decision:
    NormalizationProperties holds pointers into the decomposition data, so a
    single table of whole structs makes every decomposition string in the
    database reachable from combining_class(), which wants an integer. Storing
    the struct whole would also keep two copies of any field a caller reads
    directly, which is a thing that can drift; here there is one copy.
    """
    out.write('namespace {} {{\n'.format(name))
    for fieldName, fieldType, defval, values in fields:
        generateTable(fieldName, fieldType, defval, out, values, blockSize)
    out.write('inline {0} get_value(char32_t cp) {{\n'.format(type))
    out.write('  return {{{}}};\n}}\n}}\n'.format(
        ', '.join('{}::get_value(cp)'.format(f[0]) for f in fields)))

#------------------------------------------------------------------------------
# genGeneralCategoryPropertyTable
#------------------------------------------------------------------------------

def genGeneralCategoryPropertyTable(ucd):
    fin = open(ucd + '/UnicodeData.txt')

    defval = 'Cn'
    data = [x.rstrip().split(';') for x in fin]

    def items():
        codePointPrev = -1
        i = 0
        while i < len(data):
            flds = data[i]
            codePoint = int(flds[0], 16)
            value = flds[2]

            for cp in range(codePointPrev + 1, codePoint):
                yield cp, defval

            if flds[1].endswith('First>'):
                fldsLast = data[i + 1]
                codePointLast = int(fldsLast[0], 16)
                categoryLast = fldsLast[2]
                for cp in range(codePoint, codePointLast + 1):
                    yield cp, categoryLast
                codePointPrev = codePointLast
                i += 2
            else:
                yield codePoint, value
                codePointPrev = codePoint
                i += 1

        for cp in range(codePointPrev + 1, MaxCopePoint + 1):
            yield cp, defval

    values = [val for cp, val in items()]
    generateTable('_general_category_properties', 'GeneralCategory', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# genPropertyTable
#------------------------------------------------------------------------------

# Bit assignments must match the hand-written Property_* constants in
# unicodelib.h. Never derive them from the order of appearance in
# PropList.txt: that order changes between Unicode versions.
PROPERTY_BITS = {
    'White_Space': 0,
    'Bidi_Control': 1,
    'Join_Control': 2,
    'Dash': 3,
    'Hyphen': 4,
    'Quotation_Mark': 5,
    'Terminal_Punctuation': 6,
    'Other_Math': 7,
    'Hex_Digit': 8,
    'ASCII_Hex_Digit': 9,
    'Other_Alphabetic': 10,
    'Ideographic': 11,
    'Diacritic': 12,
    'Extender': 13,
    'Other_Lowercase': 14,
    'Other_Uppercase': 15,
    'Noncharacter_Code_Point': 16,
    'Other_Grapheme_Extend': 17,
    'IDS_Binary_Operator': 18,
    'IDS_Trinary_Operator': 19,
    'Radical': 20,
    'Unified_Ideograph': 21,
    'Other_Default_Ignorable_Code_Point': 22,
    'Deprecated': 23,
    'Soft_Dotted': 24,
    'Logical_Order_Exception': 25,
    'Other_ID_Start': 26,
    'Other_ID_Continue': 27,
    'Sentence_Terminal': 28,
    'Variation_Selector': 29,
    'Pattern_White_Space': 30,
    'Pattern_Syntax': 31,
    'Prepended_Concatenation_Mark': 32,
}

def genPropertyTable(ucd):
    fin = open(ucd + '/PropList.txt')

    values = [0] * (MaxCopePoint + 1)
    skipped = set()
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            name = m.group(3)
            if name not in PROPERTY_BITS:
                skipped.add(name)
                continue
            val = PROPERTY_BITS[name]

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] += (1 << val)
            else:
                values[codePoint] += (1 << val)

    for name in sorted(skipped):
        print('NOTE: PropList property not exposed by unicodelib: ' + name,
              file=sys.stderr)

    generateTable('_properties', "uint64_t", 0, sys.stdout, values)

#------------------------------------------------------------------------------
# genDerivedCorePropertyTable
#------------------------------------------------------------------------------

# Bit assignments must match the hand-written DerivedProperty_* constants in
# unicodelib.h (see PROPERTY_BITS above for why they are fixed here).
DERIVED_PROPERTY_BITS = {
    'Math': 0,
    'Alphabetic': 1,
    'Lowercase': 2,
    'Uppercase': 3,
    'Cased': 4,
    'Case_Ignorable': 5,
    'Changes_When_Lowercased': 6,
    'Changes_When_Uppercased': 7,
    'Changes_When_Titlecased': 8,
    'Changes_When_Casefolded': 9,
    'Changes_When_Casemapped': 10,
    'ID_Start': 11,
    'ID_Continue': 12,
    'XID_Start': 13,
    'XID_Continue': 14,
    'Default_Ignorable_Code_Point': 15,
    'Grapheme_Extend': 16,
    'Grapheme_Base': 17,
    'Grapheme_Link': 18,
    'InCB_Linker': 19,
    'InCB_Consonant': 20,
    'InCB_Extend': 21,
}

def genDerivedCorePropertyTable(ucd):
    fin = open(ucd + '/DerivedCoreProperties.txt')

    values = [0] * (MaxCopePoint + 1)
    skipped = set()
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)(?:;\s*(\w+))?\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            name = m.group(3)
            prop_val = m.group(4)

            if prop_val:
                name = name + "_" + prop_val

            if name not in DERIVED_PROPERTY_BITS:
                skipped.add(name)
                continue

            val = DERIVED_PROPERTY_BITS[name]

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] += (1 << val)
            else:
                values[codePoint] += (1 << val)

    for name in sorted(skipped):
        print('NOTE: DerivedCoreProperties property not exposed by '
              'unicodelib: ' + name, file=sys.stderr)

    generateTable('_derived_core_properties', "uint32_t", 0, sys.stdout, values)

#------------------------------------------------------------------------------
# genSimpleCaseMappingTable
#------------------------------------------------------------------------------

def genSimpleCaseMappingTable(ucd):
    fin = open(ucd + '/UnicodeData.txt')

    data = [x.rstrip().split(';') for x in fin]
    r = re.compile(r"(?:<(\w+)> )?(.+)")

    def items():
        codePointPrev = -1
        i = 0
        while i < len(data):
            flds = data[i]
            codePoint = int(flds[0], 16)
            upper = flds[12]
            lower = flds[13]
            title = flds[14]

            if flds[1].endswith('First>'):
                fldsLast = data[i + 1]
                codePointLast = int(fldsLast[0], 16)
                codePointPrev = codePointLast
                i += 2
            else:
                if len(upper) or len(lower) or len(title):
                    if len(upper) == 0:
                        upper = flds[0]
                    if len(lower) == 0:
                        lower = flds[0]
                    if len(title) == 0:
                        title = flds[0]
                    yield codePoint, int(upper, 16), int(lower, 16), int(title, 16)
                codePointPrev = codePoint
                i += 1

    # Row 0 is the "no mapping" sentinel, so indices stored in the block table
    # below are 1-based and 0 means "this code point has no simple case mapping".
    indices = []

    print("inline constexpr char32_t _simple_case_mapping_values[][3] = {")
    print('{ 0x00000000, 0x00000000, 0x00000000 },')
    for i, (cp, upper, lower, title) in enumerate(sorted_by_code_point(items()), start=1):
        print('{ 0x%08X, 0x%08X, 0x%08X },' % (upper, lower, title))
        assign_index(indices, cp, i)
    print("};")

    # Pinned rather than searched. The search minimises bytes and picks a
    # 32- or 64-entry block for the four case mapping tables, which measures
    # 10% slower on to_lowercase/to_uppercase: those read all four per
    # character, so the four first stages compete for cache, and a first
    # stage is dense and touched on every lookup while a second stage is
    # sparse. Byte count sees none of that -- weighting the first stage by 2
    # or by 4 still does not select 256 -- so it is pinned here. It costs
    # 9,246 bytes across the four tables. (The property tables are the
    # opposite: pinning 256 on them costs 39,040 bytes and changes no
    # measurement.)
    generateTable('_simple_case_mappings', 'uint16_t', 0, sys.stdout, indices,
                  blockSize=256)

#------------------------------------------------------------------------------
# genSpecialCaseMappingTable
#------------------------------------------------------------------------------

def genSpecialCaseMappingTable(ucd):
    r = re.compile(r"(?!#)(.+?); #")

    def to_array(str):
        if len(str):
            return str.split(' ')
        return []

    def is_language(str):
        return str in ('lt', 'tr', 'az')

    def items():
        fin = open(ucd + '/SpecialCasing.txt')
        for line in fin:
            m = r.match(line)
            if m:
                flds = m.group(1).split('; ')

                cp = int(flds[0], 16)
                lower = [int(x, 16) for x in to_array(flds[1])]
                title = [int(x, 16) for x in to_array(flds[2])]
                upper = [int(x, 16) for x in to_array(flds[3])]

                hasContext = False
                language = '0'
                context = 'Unassigned'
                if len(flds) == 5:
                    hasContext = True
                    for x in to_array(flds[4]):
                        if is_language(x):
                            language = '"%s"' % x
                        else:
                            context = x

                yield cp, lower, title, upper, language, context, hasContext

    rows = list(items())

    # Context-dependent entries: several code points have more than one entry
    # (full_case_mapping() walks them in SpecialCasing.txt order and applies
    # the first whose language/context condition matches), so these are
    # grouped as {offset, count} into a flat SpecialCasing array, the same
    # shape as the script extension tables below, keyed by code point through
    # a block table.
    context_rows = sorted_by_code_point([r for r in rows if r[6]], unique=False)

    groups = []
    for cp, lower, title, upper, language, context, hasContext in context_rows:
        if not groups or groups[-1][0] != cp:
            groups.append((cp, []))
        groups[-1][1].append((lower, title, upper, language, context))

    print("inline constexpr SpecialCasing _special_case_mapping_context_values[] = {")
    for cp, entries in groups:
        for lower, title, upper, language, context in entries:
            print('{ %s, %s, %s, %s, SpecialCasingContext::%s },'
                    % (to_unicode_literal(lower), to_unicode_literal(title),
                        to_unicode_literal(upper), language, context))
    print("};")

    print("inline constexpr SpecialCasingGroup _special_case_mapping_context_groups[] = {")
    print('{ 0, 0 },')
    offset = 0
    indices = []
    for i, (cp, entries) in enumerate(groups, start=1):
        print('{ %d, %d },' % (offset, len(entries)))
        offset += len(entries)
        assign_index(indices, cp, i)
    print("};")

    # Pinned to 256; see the note above _simple_case_mappings.
    generateTable('_special_case_mappings', 'uint16_t', 0, sys.stdout, indices,
                  blockSize=256)

    # Default (context-free) entries: exactly one per code point, so this is
    # a plain code-point-indexed block table like _simple_case_mappings.
    default_rows = sorted_by_code_point([r for r in rows if not r[6]])

    indices = []
    print("inline constexpr SpecialCasing _special_case_mapping_default_values[] = {")
    print('{ 0, 0, 0, 0, SpecialCasingContext::Unassigned },')
    for i, (cp, lower, title, upper, language, context, hasContext) in enumerate(
            default_rows, start=1):
        print('{ %s, %s, %s, %s, SpecialCasingContext::%s },'
                % (to_unicode_literal(lower), to_unicode_literal(title),
                    to_unicode_literal(upper), language, context))
        assign_index(indices, cp, i)
    print("};")

    # Pinned to 256; see the note above _simple_case_mappings.
    generateTable('_special_case_mappings_default', 'uint16_t', 0,
                  sys.stdout, indices, blockSize=256)

#------------------------------------------------------------------------------
# genCaseFoldingTable
#------------------------------------------------------------------------------

def genCaseFoldingTable(ucd):
    fin = open(ucd + '/CaseFolding.txt')

    r = re.compile(r"(.+?); ([CFST]); (.+?); #.*")

    dic = {}

    for line in fin:
        m = r.match(line)
        if m:
            cp = int(m.group(1), 16)
            status = m.group(2)
            codes = [int(x, 16) for x in m.group(3).split(' ')]
            if not cp in dic:
                dic[cp] = [0, 0, [], 0]
            if status == 'C':
                dic[cp][0] = codes[0]
            elif status == 'S':
                dic[cp][1] = codes[0]
            elif status == 'F':
                dic[cp][2] = codes
            elif status == 'T':
                dic[cp][3] = codes[0]

    indices = []
    print("inline constexpr CaseFolding _case_folding_values[] = {")
    print('{ 0x00000000, 0x00000000, 0, 0x00000000 },')
    for i, cp in enumerate(sorted(dic), start=1):
        cf = dic[cp]
        f = to_unicode_literal(cf[2])
        print('{ 0x%08X, 0x%08X, %s, 0x%08X },' % (cf[0], cf[1], f, cf[3]))
        assign_index(indices, cp, i)
    print("};")

    # Pinned to 256; see the note above _simple_case_mappings.
    generateTable('_case_foldings', 'uint16_t', 0, sys.stdout, indices,
                  blockSize=256)

#------------------------------------------------------------------------------
# genBlockPropertyTable
#------------------------------------------------------------------------------

def genBlockPropertyTable(ucd):
    fin = open(ucd + '/Blocks.txt')

    defval = 'Unassigned'
    values = [defval] * (MaxCopePoint + 1)
    r = re.compile(r"([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s+(.+)")

    for line in fin:
        m = r.match(line)
        if m:
            codePointFirst = int(m.group(1), 16)
            codePointLast = int(m.group(2), 16)
            block = "{}".format(''.join([x.title() if x.islower() else x for x in re.split(r"[ -]", m.group(3))]))

            for cp in range(codePointFirst, codePointLast + 1):
                values[cp] = block

    generateTable('_block_properties', 'Block', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# genScriptPropertyTable
#------------------------------------------------------------------------------

def genScriptPropertyTable(ucd):
    fin = open(ucd + '/Scripts.txt')

    defval = 'Unassigned'
    values = [defval] * (MaxCopePoint + 1)
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s+;\s+(\w+)\s+#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            value = m.group(3)

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] = value
            else:
                values[codePoint] = value

    generateTable('_script_properties', 'Script', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# genScriptExtensionTable
#------------------------------------------------------------------------------

def genScriptExtensionTable(ucd):
    # This list is from 'PropertyValueAliases.txt' in Unicode database.
    dic = {
        'Adlm': 'Adlam',
        'Aghb': 'Caucasian_Albanian',
        'Ahom': 'Ahom',
        'Arab': 'Arabic',
        'Armi': 'Imperial_Aramaic',
        'Armn': 'Armenian',
        'Avst': 'Avestan',
        'Bali': 'Balinese',
        'Bamu': 'Bamum',
        'Bass': 'Bassa_Vah',
        'Batk': 'Batak',
        'Beng': 'Bengali',
        'Bhks': 'Bhaiksuki',
        'Bopo': 'Bopomofo',
        'Brah': 'Brahmi',
        'Brai': 'Braille',
        'Bugi': 'Buginese',
        'Buhd': 'Buhid',
        'Cakm': 'Chakma',
        'Cans': 'Canadian_Aboriginal',
        'Cari': 'Carian',
        'Cham': 'Cham',
        'Cher': 'Cherokee',
        'Chrs': 'Chorasmian',
        'Copt': 'Coptic',
        'Cpmn': 'Cypro_Minoan',
        'Cprt': 'Cypriot',
        'Cyrl': 'Cyrillic',
        'Deva': 'Devanagari',
        'Diak': 'Dives_Akuru',
        'Dogr': 'Dogra',
        'Dsrt': 'Deseret',
        'Dupl': 'Duployan',
        'Egyp': 'Egyptian_Hieroglyphs',
        'Elba': 'Elbasan',
        'Elym': 'Elymaic',
        'Ethi': 'Ethiopic',
        'Gara': 'Garay',
        'Geor': 'Georgian',
        'Glag': 'Glagolitic',
        'Gong': 'Gunjala_Gondi',
        'Gonm': 'Masaram_Gondi',
        'Goth': 'Gothic',
        'Gran': 'Grantha',
        'Grek': 'Greek',
        'Gujr': 'Gujarati',
        'Gukh': 'Gurung_Khema',
        'Guru': 'Gurmukhi',
        'Hang': 'Hangul',
        'Hani': 'Han',
        'Hano': 'Hanunoo',
        'Hatr': 'Hatran',
        'Hebr': 'Hebrew',
        'Hira': 'Hiragana',
        'Hluw': 'Anatolian_Hieroglyphs',
        'Hmng': 'Pahawh_Hmong',
        'Hmnp': 'Nyiakeng_Puachue_Hmong',
        'Hrkt': 'Katakana_Or_Hiragana',
        'Hung': 'Old_Hungarian',
        'Ital': 'Old_Italic',
        'Java': 'Javanese',
        'Kali': 'Kayah_Li',
        'Kana': 'Katakana',
        'Kawi': 'Kawi',
        'Khar': 'Kharoshthi',
        'Khmr': 'Khmer',
        'Khoj': 'Khojki',
        'Kits': 'Khitan_Small_Script',
        'Knda': 'Kannada',
        'Krai': 'Kirat_Rai',
        'Kthi': 'Kaithi',
        'Lana': 'Tai_Tham',
        'Laoo': 'Lao',
        'Latn': 'Latin',
        'Lepc': 'Lepcha',
        'Limb': 'Limbu',
        'Lina': 'Linear_A',
        'Linb': 'Linear_B',
        'Lisu': 'Lisu',
        'Lyci': 'Lycian',
        'Lydi': 'Lydian',
        'Mahj': 'Mahajani',
        'Maka': 'Makasar',
        'Mand': 'Mandaic',
        'Mani': 'Manichaean',
        'Marc': 'Marchen',
        'Medf': 'Medefaidrin',
        'Mend': 'Mende_Kikakui',
        'Merc': 'Meroitic_Cursive',
        'Mero': 'Meroitic_Hieroglyphs',
        'Mlym': 'Malayalam',
        'Modi': 'Modi',
        'Mong': 'Mongolian',
        'Mroo': 'Mro',
        'Mtei': 'Meetei_Mayek',
        'Mult': 'Multani',
        'Mymr': 'Myanmar',
        'Nagm': 'Nag_Mundari',
        'Nand': 'Nandinagari',
        'Narb': 'Old_North_Arabian',
        'Nbat': 'Nabataean',
        'Newa': 'Newa',
        'Nkoo': 'Nko',
        'Nshu': 'Nushu',
        'Ogam': 'Ogham',
        'Olck': 'Ol_Chiki',
        'Onao': 'Ol_Onal',
        'Orkh': 'Old_Turkic',
        'Orya': 'Oriya',
        'Osge': 'Osage',
        'Osma': 'Osmanya',
        'Ougr': 'Old_Uyghur',
        'Palm': 'Palmyrene',
        'Pauc': 'Pau_Cin_Hau',
        'Perm': 'Old_Permic',
        'Phag': 'Phags_Pa',
        'Phli': 'Inscriptional_Pahlavi',
        'Phlp': 'Psalter_Pahlavi',
        'Phnx': 'Phoenician',
        'Plrd': 'Miao',
        'Prti': 'Inscriptional_Parthian',
        'Rjng': 'Rejang',
        'Rohg': 'Hanifi_Rohingya',
        'Runr': 'Runic',
        'Samr': 'Samaritan',
        'Sarb': 'Old_South_Arabian',
        'Saur': 'Saurashtra',
        'Sgnw': 'SignWriting',
        'Shaw': 'Shavian',
        'Shrd': 'Sharada',
        'Sidd': 'Siddham',
        'Sind': 'Khudawadi',
        'Sinh': 'Sinhala',
        'Sogd': 'Sogdian',
        'Sogo': 'Old_Sogdian',
        'Sora': 'Sora_Sompeng',
        'Soyo': 'Soyombo',
        'Sund': 'Sundanese',
        'Sunu': 'Sunuwar',
        'Sylo': 'Syloti_Nagri',
        'Syrc': 'Syriac',
        'Tagb': 'Tagbanwa',
        'Takr': 'Takri',
        'Tale': 'Tai_Le',
        'Talu': 'New_Tai_Lue',
        'Taml': 'Tamil',
        'Tang': 'Tangut',
        'Tavt': 'Tai_Viet',
        'Telu': 'Telugu',
        'Tfng': 'Tifinagh',
        'Tglg': 'Tagalog',
        'Thaa': 'Thaana',
        'Thai': 'Thai',
        'Tibt': 'Tibetan',
        'Tirh': 'Tirhuta',
        'Tnsa': 'Tangsa',
        'Todr': 'Todhri',
        'Toto': 'Toto',
        'Tutg': 'Tulu_Tigalari',
        'Ugar': 'Ugaritic',
        'Vaii': 'Vai',
        'Vith': 'Vithkuqi',
        'Wara': 'Warang_Citi',
        'Wcho': 'Wancho',
        'Xpeo': 'Old_Persian',
        'Xsux': 'Cuneiform',
        'Yezi': 'Yezidi',
        'Yiii': 'Yi',
        'Zanb': 'Zanabazar_Square',
        'Zinh': 'Inherited',
        'Zyyy': 'Common',
        'Zzzz': 'Unknown',
    }

    fin = open(ucd + '/ScriptExtensions.txt')

    values = [-1] * (MaxCopePoint + 1)
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(.*?)\s*#.*")

    ids = {}
    lists = []

    for line in fin:
        m = r.match(line)
        if m:
            firstCode = int(m.group(1), 16)

            if m.group(2):
                lastCode = int(m.group(2), 16) + 1
            else:
                lastCode = firstCode + 1

            scripts = m.group(3)

            if scripts in ids:
                id = ids[scripts]
            else:
                id = len(ids)
                ids[scripts] = id
                lists.append([dic[x] for x in scripts.split(' ')])

            for cp in range(firstCode, lastCode):
                values[cp] = id

    print("inline constexpr Script _script_extension_scripts[] = {")
    for scs in lists:
        print('    ' + ' '.join('Script::%s,' % sc for sc in scs))
    print("};")
    print("inline constexpr ScriptExtension _script_extension_properties_for_id[] = {")
    offset = 0
    for scs in lists:
        print('{ %d, %d },' % (offset, len(scs)))
        offset += len(scs)
    print("};")

    generateTable('_script_extension_ids', "int", 0, sys.stdout, values)

#------------------------------------------------------------------------------
# genNomalizationPropertyTable
#------------------------------------------------------------------------------

def genNomalizationPropertyTable(ucd):
    fin = open(ucd + '/UnicodeData.txt')

    data = [x.rstrip().split(';') for x in fin]
    r = re.compile(r"(?:<(\w+)> )?(.+)")

    def items():
        codePointPrev = -1
        i = 0
        while i < len(data):
            flds = data[i]
            codePoint = int(flds[0], 16)
            combiningClass = int(flds[3])
            codes = flds[5]

            # Code points the file does not list are unassigned: canonical
            # combining class 0, no decomposition. Filling a gap with the
            # combining class of the line that follows it gave 45 runs of
            # unassigned code points a nonzero class, U+0590 (220, from the
            # Hebrew accent after it) among them, which then reordered under
            # the canonical ordering algorithm.
            for cp in range(codePointPrev + 1, codePoint):
                yield cp, 0, None, []

            if flds[1].endswith('First>'):
                fldsLast = data[i + 1]
                codePointLast = int(fldsLast[0], 16)
                for cp in range(codePoint, codePointLast + 1):
                    yield cp, combiningClass, None, []
                codePointPrev = codePointLast
                i += 2
            else:
                m = r.match(codes)
                if m:
                    compat = m.group(1)
                    codes = [int(x, 16) for x in m.group(2).split(' ')]
                    yield codePoint, combiningClass, compat, codes
                else:
                    yield codePoint, combiningClass, None, []
                codePointPrev = codePoint
                i += 1

        for cp in range(codePointPrev + 1, MaxCopePoint + 1):
            yield cp, 0, None, []

    classes = []
    compats = []
    decompositions = []
    for cp, cls, compat, codes in items():
        classes.append(cls)
        compats.append('"%s"' % compat if compat else '0')
        decompositions.append(to_unicode_literal(codes))

    generateRecordTable('_normalization_properties', 'NormalizationProperties',
                        sys.stdout,
                        [('combining_class', 'uint8_t', 0, classes),
                         ('compat_format', 'const char *', 0, compats),
                         ('codes', 'const char32_t *', 0, decompositions)])

#------------------------------------------------------------------------------
# genNomalizationCompositionTable
#------------------------------------------------------------------------------

def genNomalizationCompositionTable(ucd):
    fin = open(ucd + '/UnicodeData.txt')
    finExclusions = open(ucd + '/CompositionExclusions.txt')

    data = [x.rstrip().split(';') for x in fin]
    r = re.compile(r"(?:<(\w+)> )?(.+)")

    def items():
        codePointPrev = -1
        i = 0
        while i < len(data):
            flds = data[i]
            codePoint = int(flds[0], 16)
            combiningClass = int(flds[3])
            codes = flds[5]

            if flds[1].endswith('First>'):
                fldsLast = data[i + 1]
                codePointLast = int(fldsLast[0], 16)
                codePointPrev = codePointLast
                i += 2
            else:
                m = r.match(codes)
                if m:
                    compat = m.group(1)
                    codes = [int(x, 16) for x in m.group(2).split(' ')]
                    if len(codes) == 2 and not compat and combiningClass == 0:
                        yield codePoint, codes
                codePointPrev = codePoint
                i += 1

    exclusions = set()
    rRange = re.compile(r"(?:# )?([0-9A-F]{4,})(?:\.\.([0-9A-F]+))?.*")
    for line in finExclusions:
        m = rRange.match(line)
        if m:
            first = int(m.group(1), 16)
            if m.group(2):
                last = int(m.group(2), 16)
                for i in range(first, last + 1):
                    exclusions.add(i)
            else:
                exclusions.add(first)

    rows = sorted_by_code_point(
        ((codes[0], codes[1]), cp) for cp, codes in items() if not cp in exclusions)
    print("inline constexpr Composition _normalization_composition[] = {")
    for (first, second), cp in rows:
        print('{ 0x%08X, 0x%08X, 0x%08X },' % (first, second, cp))
    print("};")

#------------------------------------------------------------------------------
# genGraphemeBreakPropertyTable
#------------------------------------------------------------------------------

def genGraphemeBreakPropertyTable(ucd):
    fin = open(ucd + '/auxiliary/GraphemeBreakProperty.txt')

    defval = 'Unassigned'
    values = [defval] * (MaxCopePoint + 1)
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            value = m.group(3)

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] = value
            else:
                values[codePoint] = value

    generateTable('_grapheme_break_properties', 'GraphemeBreak', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# genWordBreakPropertyTable
#------------------------------------------------------------------------------

def genWordBreakPropertyTable(ucd):
    fin = open(ucd + '/auxiliary/WordBreakProperty.txt')

    defval = 'Unassigned'
    values = [defval] * (MaxCopePoint + 1)
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            value = m.group(3)

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] = value
            else:
                values[codePoint] = value

    generateTable('_word_break_properties', 'WordBreak', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# genSentenceBreakPropertyTable
#------------------------------------------------------------------------------

def genSentenceBreakPropertyTable(ucd):
    fin = open(ucd + '/auxiliary/SentenceBreakProperty.txt')

    defval = 'Unassigned'
    values = [defval] * (MaxCopePoint + 1)
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            value = m.group(3)

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] = value
            else:
                values[codePoint] = value

    generateTable('_sentence_break_properties', 'SentenceBreak', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# genEmojiPropertyTable
#------------------------------------------------------------------------------

def genEmojiPropertyTable(ucd):
    fin = open(ucd + '/emoji/emoji-data.txt')

    defval = 'Unassigned'
    values = [defval] * (MaxCopePoint + 1)
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            value = m.group(3)

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] = value
            else:
                values[codePoint] = value

    generateTable('_emoji_properties', 'Emoji', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# genEastAsianWidthPropertyTable
#------------------------------------------------------------------------------

def genEastAsianWidthPropertyTable(ucd):
    fin = open(ucd + '/EastAsianWidth.txt')

    # Map the short property values in the data file to the descriptive enum
    # members declared in `enum class EastAsianWidth`.
    longname = {
        'N': 'Neutral',
        'A': 'Ambiguous',
        'H': 'Halfwidth',
        'W': 'Wide',
        'F': 'Fullwidth',
        'Na': 'Narrow',
    }

    defval = 'Neutral'
    values = [defval] * (MaxCopePoint + 1)

    # Per the header of EastAsianWidth.txt, unassigned code points in the
    # following ranges default to 'Wide' (W). These ranges are NOT listed in
    # the data lines, so pre-fill them before applying the explicit data
    # (explicit assignments below override these defaults where they overlap).
    for first, last in [
        (0x3400, 0x4DBF),    # CJK Unified Ideographs Extension A
        (0x4E00, 0x9FFF),    # CJK Unified Ideographs
        (0xF900, 0xFAFF),    # CJK Compatibility Ideographs
        (0x20000, 0x2FFFD),  # Plane 2 (Supplementary Ideographic Plane)
        (0x30000, 0x3FFFD),  # Plane 3 (Tertiary Ideographic Plane)
    ]:
        for cp in range(first, last + 1):
            values[cp] = 'Wide'

    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            value = longname[m.group(3)]

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] = value
            else:
                values[codePoint] = value

    generateTable('_east_asian_width_properties', 'EastAsianWidth', defval, sys.stdout, values)

#------------------------------------------------------------------------------
# Main
#------------------------------------------------------------------------------

if (len(sys.argv) < 2):
    print('usage: python gen_tables.py UCD_DIR')
else:
    ucd = sys.argv[1]

    genGeneralCategoryPropertyTable(ucd)
    genPropertyTable(ucd)
    genDerivedCorePropertyTable(ucd)
    genSimpleCaseMappingTable(ucd)
    genSpecialCaseMappingTable(ucd)
    genCaseFoldingTable(ucd)
    genBlockPropertyTable(ucd)
    genScriptPropertyTable(ucd)
    genScriptExtensionTable(ucd)
    genNomalizationPropertyTable(ucd)
    genNomalizationCompositionTable(ucd)
    genGraphemeBreakPropertyTable(ucd)
    genWordBreakPropertyTable(ucd)
    genSentenceBreakPropertyTable(ucd)
    genEmojiPropertyTable(ucd)
    genEastAsianWidthPropertyTable(ucd)
