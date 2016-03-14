import sys
import re

MaxCode = 0x0010FFFF

values = ['Unassigned'] * (MaxCode + 1)
r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s+;\s+(\w+)\s+#.*")

for line in sys.stdin.readlines():
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

for val in values:
    print("    GraphemeBreak::%s," % val)
