import sys
import re

MaxCode = 0x0010FFFF

for line in sys.stdin.readlines():
    r = re.compile(r"([0-9A-F]+)(?:\.\.([0-9A-F]+))?\s+;\s+(\w+)\s+#.*")
    m = r.match(line)
    if m:
        print(m.group(1), m.group(2), m.group(3))

