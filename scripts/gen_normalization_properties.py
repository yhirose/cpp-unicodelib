import sys
import re

MaxCode = 0x0010FFFF

db = sys.argv[1]
unicodeDataPath = db + '/UnicodeData.txt'

data = [x.rstrip().split(';') for x in open(unicodeDataPath)]
r = re.compile(r"(?:<(\w+)> )?(.+)")

def items():
    codePointPrev = -1
    i = 0
    while i < len(data):
        flds = data[i]
        codePoint = int(flds[0], 16)
        name = flds[1]
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

for cp, cls, compat, codes in items():
    if compat:
        compat = '"%s"' % compat
    else:
        compat = 'nullptr'
    if codes:
        codes = 'U"%s"' % ''.join(["\\U%08X" % x for x in codes])
    else:
        codes = 'nullptr'
    print("%d, %s, %s," % (cls, compat, codes))

