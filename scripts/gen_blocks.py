import sys
import re

MaxCode = 0x0010FFFF

r = re.compile(r"([0-9A-F]+)\.\.([0-9A-F]+)\s*;\s+(.+)")

for line in sys.stdin.readlines():
    m = r.match(line)
    if m:
        codePointFirst = int(m.group(1), 16)
        codePointLast = int(m.group(2), 16)
        block = ''.join([x.title() if x.islower() else x for x in re.split(r"[ -]", m.group(3))])
        print('    %s,' % block)
