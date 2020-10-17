import sys
import re

def getNameAliases(ucd):
    dict = {}
    fin = open(ucd + '/NameAliases.txt')
    for line in fin:
        line = line.rstrip()
        if len(line) > 0 and line[0] != '#':
            flds = line.split(';')
            cp = int(flds[0], 16)
            if not cp in dict:
                dict[cp] = flds[1]
    return dict

def genUnicodeSymbols(ucd):
    fin = open(ucd + '/UnicodeData.txt')

    data = [x.rstrip().split(';') for x in fin]

    def items():
        i = 0
        while i < len(data):
            flds = data[i]
            codePoint = int(flds[0], 16)
            name = flds[1]

            yield codePoint, name, len(flds[0])
            if flds[1].endswith('First>'):
                fldsLast = data[i + 1]
                codePointLast = int(fldsLast[0], 16)
                nameLast = fldsLast[1]
                yield codePointLast, nameLast, len(fldsLast[0])
                i += 2
            else:
                i += 1

    aliases = getNameAliases(ucd)

    print('''
#pragma once

#include <cstdlib>

namespace unicode {
''')

    for cp, name, nameLen in items():
        if cp != 0:
            name = re.sub(r'[<>,]', '', name)
            if name == 'control':
                name = aliases[cp]
            symbol = '_'.join([x.upper() for x in re.split(r"[ -]", name)])
            if nameLen > 4:
                print("const char32_t %s = 0x%08X;" % (symbol, cp))
            else:
                print("const char32_t %s = 0x%04X;" % (symbol, cp))

    print('''
} // namespace unicode
''')

if (len(sys.argv) < 2):
    print('usage: python gen_unicode_symbols.py UCD_DIR')
else:
    ucd = sys.argv[1]
    genUnicodeSymbols(ucd)
