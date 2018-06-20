import sys
import re

MaxCode = 0x0010FFFF

r = re.compile(r"^[0-9A-F]+(?:\.\.[0-9A-F]+)?\s*;\s*(.+?)\s*(?:#.+)?$")

names = []
for line in sys.stdin.readlines():
    m = r.match(line)
    if m:
        name = ''.join([x.title() if x.islower() else x for x in re.split(r"[ -]", m.group(1))])
        if not name in names:
            names.append(name)

for name in names:
    print('  %s,' % name)
