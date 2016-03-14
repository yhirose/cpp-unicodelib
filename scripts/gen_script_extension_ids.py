import sys
import re

MaxCode = 0x0010FFFF

values = [-1] * (MaxCode + 1)
rHeader = re.compile(r"# Script_Extensions=(.*)")
r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?.*")

id = -1
for line in sys.stdin.readlines():
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

for id in values:
    print("    %d," % id)
