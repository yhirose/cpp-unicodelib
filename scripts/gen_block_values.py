import sys
import re

MaxCode = 0x0010FFFF

values = ['Unassigned'] * (MaxCode + 1)
r = re.compile(r"([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s+(.+)")

for line in sys.stdin.readlines():
    m = r.match(line)
    if m:
        codePointFirst = int(m.group(1), 16)
        codePointLast = int(m.group(2), 16)
        block = ''.join([x.title() if x.islower() else x for x in re.split(r"[ -]", m.group(3))])

        for cp in range(codePointFirst, codePointLast + 1):
            values[cp] = block

for val in values:
    print("    Block::%s," % val)
