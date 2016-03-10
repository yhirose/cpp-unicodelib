import sys

MaxCode = 0x0010FFFF

lines = sys.stdin.readlines()
data = [x.rstrip().split(';') for x in lines]

def iterateUCDItems():
    codePointPrev = -1
    i = 0
    while i < len(data):
        flds = data[i]
        codePoint = int(flds[0], 16)
        name = flds[1]
        category = flds[2]

        for cp in range(codePointPrev + 1, codePoint):
            yield cp, 'Invalid'

        if flds[1].endswith('First>'):
            fldsLast = data[i + 1]
            codePointLast = int(fldsLast[0], 16)
            categoryLast = fldsLast[2]
            for cp in range(codePoint, codePointLast + 1):
                yield cp, categoryLast
            codePointPrev = codePointLast
            i += 2
        else:
            yield codePoint, category
            codePointPrev = codePoint
            i += 1

    for cp in range(codePointPrev + 1, MaxCode + 1):
        yield cp, 'Invalid'

for cp, cat in iterateUCDItems():
    print("    { %s }," % (cat))

