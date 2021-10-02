import sys
import re

#------------------------------------------------------------------------------
# Constants
#------------------------------------------------------------------------------

MaxCopePoint = 0x0010FFFF

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

def to_unicode_literal(str):
    if len(str):
        return 'U"%s"' % ''.join([('\\U%08X' % x) for x in str])
    return '0'

#------------------------------------------------------------------------------
# generateTable
#------------------------------------------------------------------------------

def findDataSize(values, blockSize):
    result = []
    for i in range(0, len(values), blockSize):
        items = values[i:i+blockSize]
        result.append(all([x == items[0] for x in items]))

    itemSize = 8
    return (result.count(True) * itemSize + result.count(False) * blockSize * itemSize) / 1024

def findBestBlockSize(values):
    originalSize = findDataSize(values, 1)
    smallestSize = originalSize
    blockSize = 1

    for bs in [16, 32, 64, 128, 256, 512, 1024]:
        sz = findDataSize(values, bs)
        if sz < smallestSize:
            smallestSize = sz
            blockSize = bs

    return blockSize

def isPremitiveType(type):
    return type == 'int' or type == 'uint32_t' or type == 'uint64_t' or type == 'NormalizationProperties'

def generateTable(name, type, defval, out, values):
    def formatValue(val):
        if isPremitiveType(type):
            return "{},".format(val)
        else:
            return "D," if val == defval else "T::{},".format(val)

    if isPremitiveType(type):
        out.write("""namespace {0} {{
""".format(name))
    else:
        out.write("""namespace {0} {{
using T = {1};
const auto D = {1}::{2};
""".format(name, type, defval))

    blockSize = findBestBlockSize(values)
    out.write("static const size_t _block_size = {};\n".format(blockSize))

    blockValues = []
    for i in range(0, len(values), blockSize):
        items = values[i:i+blockSize]
        if all([x == items[0] for x in items]):
            blockValues.append(items[0])
        else:
            blockValues.append(None)
            iblock = i // blockSize
            out.write("static const {} _{}[] = {{ ".format(type, iblock))
            for val in items:
                out.write(formatValue(val))
            out.write(" };\n")

    out.write("static const {} *_blocks[] = {{\n".format(type))
    for iblock, blockValue in enumerate(blockValues):
        if iblock % 8 == 0:
            if iblock == 0:
                out.write(" ")
            else:
                out.write("\n ")
        if blockValue != None:
            out.write("0,")
        else:
            out.write("_{},".format(iblock))
    out.write("\n};\n")

    out.write("static const {} _block_values[] = {{\n".format(type))
    for iblock, blockValue in enumerate(blockValues):
        if iblock % 8 == 0:
            if iblock == 0:
                out.write(" ")
            else:
                out.write("\n ")
        if blockValue != None:
            out.write(formatValue(blockValue))
        else:
            out.write(formatValue(defval))
    out.write("\n};\n")

    out.write("""inline {} get_value(char32_t cp) {{
  auto i = cp / _block_size;
  auto bl = _blocks[i];
  if (bl) {{
    auto off = cp % _block_size;
    return bl[off];
  }}
  return _block_values[i];
}}
}}
""".format(type))

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

def genPropertyTable(ucd):
    fin = open(ucd + '/PropList.txt')

    values = [0] * (MaxCopePoint + 1)
    names = {}
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            name = m.group(3)
            if not name in names:
                names[name] = len(names)
            val = names[name]

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] += (1 << val)
            else:
                values[codePoint] += (1 << val)

    generateTable('_properties', "uint64_t", 0, sys.stdout, values)

#------------------------------------------------------------------------------
# genDerivedCorePropertyTable
#------------------------------------------------------------------------------

def genDerivedCorePropertyTable(ucd):
    fin = open(ucd + '/DerivedCoreProperties.txt')

    values = [0] * (MaxCopePoint + 1)
    names = {}
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s*;\s*(\w+)\s*#.*")

    for line in fin:
        m = r.match(line)
        if m:
            codePoint = int(m.group(1), 16)
            name = m.group(3)
            if not name in names:
                names[name] = len(names)
            val = names[name]

            if m.group(2):
                codePointLast = int(m.group(2), 16)
                for cp in range(codePoint, codePointLast + 1):
                    values[cp] += (1 << val)
            else:
                values[codePoint] += (1 << val)

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

    print("const std::unordered_map<char32_t, const char32_t*> _simple_case_mappings = {")
    for cp, upper, lower, title in items():
        print('{ 0x%08X, U"\\U%08X\\U%08X\\U%08X" },' % (cp, upper, lower, title))
    print("};")

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

    # Regular
    print("const std::unordered_multimap<char32_t, SpecialCasing> _special_case_mappings = {")
    for cp, lower, title, upper, language, context, hasContext in items():
        if hasContext == True:
            print('{ 0x%08X, { %s, %s, %s, %s, SpecialCasingContext::%s } },'
                    % (cp, to_unicode_literal(lower), to_unicode_literal(title),
                        to_unicode_literal(upper), language, context))
    print("};")

    # Default
    print("const std::unordered_multimap<char32_t, SpecialCasing> _special_case_mappings_default = {")
    for cp, lower, title, upper, language, context, hasContext in items():
        if hasContext == False:
            print('{ 0x%08X, { %s, %s, %s, %s, SpecialCasingContext::%s } },'
                    % (cp, to_unicode_literal(lower), to_unicode_literal(title),
                        to_unicode_literal(upper), language, context))
    print("};")

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

    print("const std::unordered_map<char32_t, CaseFolding> _case_foldings = {")
    for cp in dic:
        cf = dic[cp]
        f = to_unicode_literal(cf[2])
        print('{ 0x%08X, { 0x%08X,  0x%08X, %s, 0x%08X } },' % (cp, cf[0], cf[1], f, cf[3]))
    print("};")

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
# genScriptExtensionIdTable
#------------------------------------------------------------------------------

def genScriptExtensionIdTable(ucd):
    fin = open(ucd + '/ScriptExtensions.txt')

    values = [-1] * (MaxCopePoint + 1)
    rHeader = re.compile(r"# Script_Extensions=(.*)")
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?.*")

    id = -1
    for line in fin:
        m = rHeader.match(line)
        if m:
            id += 1
        else:
            m = r.match(line)
            if m:
                codePoint = int(m.group(1), 16)

                if m.group(2):
                    codePointLast = int(m.group(2), 16)
                    for cp in range(codePoint, codePointLast + 1):
                        values[cp] = id
                else:
                    values[codePoint] = id

    generateTable('_script_extension_ids', "int", 0, sys.stdout, values)

#------------------------------------------------------------------------------
# genScriptExtensionPropertyForIdTable
#------------------------------------------------------------------------------

def genScriptExtensionPropertyForIdTable(ucd):
    fin = open(ucd + '/ScriptExtensions.txt')

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
        'Geor': 'Georgian',
        'Glag': 'Glagolitic',
        'Gong': 'Gunjala_Gondi',
        'Gonm': 'Masaram_Gondi',
        'Goth': 'Gothic',
        'Gran': 'Grantha',
        'Grek': 'Greek',
        'Gujr': 'Gujarati',
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
        'Khar': 'Kharoshthi',
        'Khmr': 'Khmer',
        'Khoj': 'Khojki',
        'Kits': 'Khitan_Small_Script',
        'Knda': 'Kannada',
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
        'Nand': 'Nandinagari',
        'Narb': 'Old_North_Arabian',
        'Nbat': 'Nabataean',
        'Newa': 'Newa',
        'Nkoo': 'Nko',
        'Nshu': 'Nushu',
        'Ogam': 'Ogham',
        'Olck': 'Ol_Chiki',
        'Orkh': 'Old_Turkic',
        'Ougr': 'Old_Uyghur',
        'Orya': 'Oriya',
        'Osge': 'Osage',
        'Osma': 'Osmanya',
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
        'Ugar': 'Ugaritic',
        'Vaii': 'Vai',
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

    values = [-1] * (MaxCopePoint + 1)
    rHeader = re.compile(r"# Script_Extensions=(.*)")

    print("const std::vector<std::vector<Script>> _script_extension_properties_for_id = {")
    id = 0
    for line in fin:
        m = rHeader.match(line)
        if m:
            print('{')
            for sc in [dic[x] for x in m.group(1).split(' ')]:
                print('    Script::%s, ' % sc)
            print('},')
            id += 1
    print("};")

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

            for cp in range(codePointPrev + 1, codePoint):
                yield cp, combiningClass, None, []

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
            yield cp, combiningClass, None, []

    values = []
    for cp, cls, compat, codes in items():
        if compat:
            compat = '"%s"' % compat
        else:
            compat = '0'
        if codes:
            codes = 'U"%s"' % ''.join(["\\U%08X" % x for x in codes])
        else:
            codes = '0'
        values.append("{{{},{},{}}}".format(cls, compat, codes))

    generateTable('_normalization_properties', 'NormalizationProperties', "{0,0,0}", sys.stdout, values)

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

    print("const std::unordered_map<std::u32string, char32_t> _normalization_composition = {")
    for cp, codes in items():
        if not cp in exclusions:
            print('{ U"\\U%08X\\U%08X", 0x%08X },' % (codes[0], codes[1], cp))
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
    genScriptExtensionIdTable(ucd)
    genScriptExtensionPropertyForIdTable(ucd)
    genNomalizationPropertyTable(ucd)
    genNomalizationCompositionTable(ucd)
    genGraphemeBreakPropertyTable(ucd)
    genWordBreakPropertyTable(ucd)
    genSentenceBreakPropertyTable(ucd)
    genEmojiPropertyTable(ucd)
