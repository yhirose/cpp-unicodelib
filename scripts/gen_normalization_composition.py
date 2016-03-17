import sys
import re

db = sys.argv[1]
unicodeDataPath = db + '/UnicodeData.txt'
compositionExclusionsPath = db + '/CompositionExclusions.txt'

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
for line in open(compositionExclusionsPath):
    m = rRange.match(line)
    if m:
        first = int(m.group(1), 16)
        if m.group(2):
            last = int(m.group(2), 16)
            for i in range(first, last + 1):
                exclusions.add(i)
        else:
            exclusions.add(first)

for cp, codes in items():
    if not cp in exclusions:
        print('{ U"\U%08X\U%08X", 0x%08X },' % (codes[0], codes[1], cp))
