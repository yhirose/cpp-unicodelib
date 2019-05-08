import sys
import re

#------------------------------------------------------------------------------
# Constants
#------------------------------------------------------------------------------

MaxCode = 0x0010FFFF

#------------------------------------------------------------------------------
# Utilities
#------------------------------------------------------------------------------

def to_unicode_literal(str):
    if len(str):
        return 'U"%s"' % ''.join([('\\U%08X' % x) for x in str])
    return 'nullptr'

#------------------------------------------------------------------------------
# genGeneralCategoryPropertyTable
#------------------------------------------------------------------------------

def genGeneralCategoryPropertyTable(ucd, out):
    fin = open(ucd + '/UnicodeData.txt')
    fout = open(out + '/_general_category_properties.cpp', 'w')

    data = [x.rstrip().split(';') for x in fin]

    def items():
        codePointPrev = -1
        i = 0
        while i < len(data):
            flds = data[i]
            codePoint = int(flds[0], 16)
            value = flds[2]

            for cp in range(codePointPrev + 1, codePoint):
                yield cp, 'Cn'

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

        for cp in range(codePointPrev + 1, MaxCode + 1):
            yield cp, 'Cn'

    fout.write("const GeneralCategory _general_category_properties[] = {\n")
    for cp, val in items():
        fout.write("GeneralCategory::%s,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# getPropertyTable
#------------------------------------------------------------------------------

def getPropertyTable(ucd, out):
    fin = open(ucd + '/PropList.txt')
    fout = open(out + '/_properties.cpp', 'w')

    values = [0] * (MaxCode + 1)
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

    fout.write("const uint64_t _properties[] = {\n")
    for val in values:
        fout.write("0x%016X,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# getDerivedCorePropertyTable
#------------------------------------------------------------------------------

def getDerivedCorePropertyTable(ucd, out):
    fin = open(ucd + '/DerivedCoreProperties.txt')
    fout = open(out + '/_derived_core_properties.cpp', 'w')

    values = [0] * (MaxCode + 1)
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

    fout.write("const uint32_t _derived_core_properties[] = {\n")
    for val in values:
        fout.write("0x%08X,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# getSimpleCaseMappingTable
#------------------------------------------------------------------------------

def getSimpleCaseMappingTable(ucd, out):
    fin = open(ucd + '/UnicodeData.txt')
    fout = open(out + '/_simple_case_mappings.cpp', 'w')

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

    fout.write("const std::unordered_map<char32_t, const char32_t*> _simple_case_mappings = {\n")
    for cp, upper, lower, title in items():
        fout.write('{ 0x%08X, U"\\U%08X\\U%08X\\U%08X" },\n' % (cp, upper, lower, title))
    fout.write("};\n")

#------------------------------------------------------------------------------
# getSpecialCaseMappingTable
#------------------------------------------------------------------------------

def getSpecialCaseMappingTable(ucd, out):
    fin = open(ucd + '/SpecialCasing.txt')
    fout = open(out + '/_special_case_mappings.cpp', 'w')
    foutDefault = open(out + '/_special_case_mappings_default.cpp', 'w')

    r = re.compile(r"(?!#)(.+?); #")

    def to_array(str):
        if len(str):
            return str.split(' ')
        return []

    def is_language(str):
        return str in ('lt', 'tr', 'az')

    def items():
        for line in fin:
            m = r.match(line)
            if m:
                flds = m.group(1).split('; ')

                cp = int(flds[0], 16)
                lower = [int(x, 16) for x in to_array(flds[1])]
                title = [int(x, 16) for x in to_array(flds[2])]
                upper = [int(x, 16) for x in to_array(flds[3])]

                hasContext = False
                language = 'nullptr'
                context = 'Unassigned'
                if len(flds) == 5:
                    hasContext = True
                    for x in to_array(flds[4]):
                        if is_language(x):
                            language = '"%s"' % x
                        else:
                            context = x

                yield cp, lower, title, upper, language, context, hasContext

    fout.write("const std::unordered_multimap<char32_t, SpecialCasing> _special_case_mappings = {\n")
    foutDefault.write("const std::unordered_multimap<char32_t, SpecialCasing> _special_case_mappings_default = {\n")
    for cp, lower, title, upper, language, context, hasContext in items():
        if hasContext:
            f = fout
        else:
            f = foutDefault
        f.write('{ 0x%08X, { %s, %s, %s, %s, SpecialCasingContext::%s } },\n'
                % (cp, to_unicode_literal(lower), to_unicode_literal(title),
                    to_unicode_literal(upper), language, context))
    fout.write("};\n")
    foutDefault.write("};\n")

#------------------------------------------------------------------------------
# getCaseFoldingTable
#------------------------------------------------------------------------------

def getCaseFoldingTable(ucd, out):
    fin = open(ucd + '/CaseFolding.txt')
    fout = open(out + '/_case_foldings.cpp', 'w')

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

    fout.write("const std::unordered_map<char32_t, CaseFolding> _case_foldings = {\n")
    for cp in dic:
        cf = dic[cp]
        f = to_unicode_literal(cf[2])
        fout.write('{ 0x%08X, { 0x%08X,  0x%08X, %s, 0x%08X } },\n' % (cp, cf[0], cf[1], f, cf[3]))
    fout.write("};\n")

#------------------------------------------------------------------------------
# genBlockPropertyTable
#------------------------------------------------------------------------------

def genBlockPropertyTable(ucd, out):
    fin = open(ucd + '/Blocks.txt')
    fout = open(out + '/_block_properties.cpp', 'w')

    values = ['Unassigned'] * (MaxCode + 1)
    r = re.compile(r"([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s+(.+)")

    for line in fin:
        m = r.match(line)
        if m:
            codePointFirst = int(m.group(1), 16)
            codePointLast = int(m.group(2), 16)
            block = ''.join([x.title() if x.islower() else x for x in re.split(r"[ -]", m.group(3))])

            for cp in range(codePointFirst, codePointLast + 1):
                values[cp] = block

    fout.write("const Block _block_properties[] = {\n")
    for val in values:
        fout.write("    Block::%s,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# genScriptPropertyTable
#------------------------------------------------------------------------------

def genScriptPropertyTable(ucd, out):
    fin = open(ucd + '/Scripts.txt')
    fout = open(out + '/_script_properties.cpp', 'w')

    values = ['Unassigned'] * (MaxCode + 1)
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

    fout.write("const Script _script_properties[] = {\n")
    for val in values:
        fout.write("Script::%s,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# genScriptExtensionIdTable
#------------------------------------------------------------------------------

def genScriptExtensionIdTable(ucd, out):
    fin = open(ucd + '/ScriptExtensions.txt')
    fout = open(out + '/_script_extension_ids.cpp', 'w')

    values = [-1] * (MaxCode + 1)
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

    fout.write("const int _script_extension_ids[] = {\n")
    for id in values:
        fout.write("%d,\n" % id)
    fout.write("};\n")

#------------------------------------------------------------------------------
# genScriptExtensionPropertyForIdTable
#------------------------------------------------------------------------------

def genScriptExtensionPropertyForIdTable(ucd, out):
    fin = open(ucd + '/ScriptExtensions.txt')
    fout = open(out + '/_script_extension_properties_for_id.cpp', 'w')

    # This list is from 'PropertyValueAliases.txt' in Unicode database.
    dic = {
        'Adlm': 'Adlam',
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
        'Copt': 'Coptic',
        'Cprt': 'Cypriot',
        'Cyrl': 'Cyrillic',
        'Deva': 'Devanagari',
        'Dogr': 'Dogra',
        'Dsrt': 'Deseret',
        'Dupl': 'Duployan',
        'Egyp': 'Egyptian_Hieroglyphs',
        'Elba': 'Elbasan',
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
        'Hrkt': 'Katakana_Or_Hiragana',
        'Hung': 'Old_Hungarian',
        'Ital': 'Old_Italic',
        'Java': 'Javanese',
        'Kali': 'Kayah_Li',
        'Kana': 'Katakana',
        'Khar': 'Kharoshthi',
        'Khmr': 'Khmer',
        'Khoj': 'Khojki',
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
        'Xpeo': 'Old_Persian',
        'Xsux': 'Cuneiform',
        'Yiii': 'Yi',
        'Zanb': 'Zanabazar_Square',
        'Zinh': 'Inherited',
        'Zyyy': 'Common',
        'Zzzz': 'Unknown',
    }

    values = [-1] * (MaxCode + 1)
    rHeader = re.compile(r"# Script_Extensions=(.*)")

    fout.write("const std::vector<std::vector<Script>> _script_extension_properties_for_id = {\n")
    id = 0
    for line in fin:
        m = rHeader.match(line)
        if m:
            fout.write('{\n')
            for sc in [dic[x] for x in m.group(1).split(' ')]:
                fout.write('    Script::%s, \n' % sc)
            fout.write('},\n')
            id += 1
    fout.write("};\n")

#------------------------------------------------------------------------------
# genNomalizationPropertyTable
#------------------------------------------------------------------------------

def genNomalizationPropertyTable(ucd, out):
    fin = open(ucd + '/UnicodeData.txt')
    fout = open(out + '/_normalization_properties.cpp', 'w')

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

        for cp in range(codePointPrev + 1, MaxCode + 1):
            yield cp, combiningClass, None, []

    fout.write("const NormalizationProperties _normalization_properties[] = {\n")
    for cp, cls, compat, codes in items():
        if compat:
            compat = '"%s"' % compat
        else:
            compat = 'nullptr'
        if codes:
            codes = 'U"%s"' % ''.join(["\\U%08X" % x for x in codes])
        else:
            codes = 'nullptr'
        fout.write("{ %d, %s, %s },\n" % (cls, compat, codes))
    fout.write("};\n")

#------------------------------------------------------------------------------
# genNomalizationCompositionTable
#------------------------------------------------------------------------------

def genNomalizationCompositionTable(ucd, out):
    fin = open(ucd + '/UnicodeData.txt')
    finExclusions = open(ucd + '/CompositionExclusions.txt')
    fout = open(out + '/_normalization_composition.cpp', 'w')

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

    fout.write("const std::unordered_map<std::u32string, char32_t> _normalization_composition = {\n")
    for cp, codes in items():
        if not cp in exclusions:
            fout.write('{ U"\\U%08X\\U%08X", 0x%08X },\n' % (codes[0], codes[1], cp))
    fout.write("};\n")

#------------------------------------------------------------------------------
# getGraphemeBreakPropertyTable
#------------------------------------------------------------------------------

def getGraphemeBreakPropertyTable(ucd, out):
    fin = open(ucd + '/auxiliary/GraphemeBreakProperty.txt')
    fout = open(out + '/_grapheme_break_properties.cpp', 'w')

    values = ['Unassigned'] * (MaxCode + 1)
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

    fout.write("const GraphemeBreak _grapheme_break_properties[] = {\n")
    for val in values:
        fout.write("GraphemeBreak::%s,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# getWordBreakPropertyTable
#------------------------------------------------------------------------------

def getWordBreakPropertyTable(ucd, out):
    fin = open(ucd + '/auxiliary/WordBreakProperty.txt')
    fout = open(out + '/_word_break_properties.cpp', 'w')

    values = ['Unassigned'] * (MaxCode + 1)
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

    fout.write("const WordBreak _word_break_properties[] = {\n")
    for val in values:
        fout.write("WordBreak::%s,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# getSentenceBreakPropertyTable
#------------------------------------------------------------------------------

def getSentenceBreakPropertyTable(ucd, out):
    fin = open(ucd + '/auxiliary/SentenceBreakProperty.txt')
    fout = open(out + '/_sentence_break_properties.cpp', 'w')

    values = ['Unassigned'] * (MaxCode + 1)
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

    fout.write("const SentenceBreak _sentence_break_properties[] = {\n")
    for val in values:
        fout.write("SentenceBreak::%s,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# getEmojiPropertyTable
#------------------------------------------------------------------------------

def getEmojiPropertyTable(ucd, out):
    fin = open(ucd + '/emoji/emoji-data.txt')
    fout = open(out + '/_emoji_properties.cpp', 'w')

    values = ['Unassigned'] * (MaxCode + 1)
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

    fout.write("const Emoji _emoji_properties[] = {\n")
    for val in values:
        fout.write("Emoji::%s,\n" % val)
    fout.write("};\n")

#------------------------------------------------------------------------------
# Main
#------------------------------------------------------------------------------

if (len(sys.argv) < 3):
    print('usage: python gen_tables.py UCD_DIR OUT_DIR')
else:
    ucd = sys.argv[1]
    out = sys.argv[2]

    genGeneralCategoryPropertyTable(ucd, out)
    getPropertyTable(ucd, out)
    getDerivedCorePropertyTable(ucd, out)
    getSimpleCaseMappingTable(ucd, out)
    getSpecialCaseMappingTable(ucd, out)
    getCaseFoldingTable(ucd, out)
    genBlockPropertyTable(ucd, out)
    genScriptPropertyTable(ucd, out)
    genScriptExtensionIdTable(ucd, out)
    genScriptExtensionPropertyForIdTable(ucd, out)
    genNomalizationPropertyTable(ucd, out)
    genNomalizationCompositionTable(ucd, out)
    getGraphemeBreakPropertyTable(ucd, out)
    getWordBreakPropertyTable(ucd, out)
    getSentenceBreakPropertyTable(ucd, out)
    getEmojiPropertyTable(ucd, out)
