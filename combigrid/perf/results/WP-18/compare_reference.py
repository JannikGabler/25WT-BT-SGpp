"""Compares the raw perf_reference output (JSON lines) of two snapshots: every field except the
node checksum must be bitwise identical; reports the cases whose node checksum differs."""
import json
import sys

a = [json.loads(l) for l in open(sys.argv[1]) if l.strip().startswith('{')]
b = [json.loads(l) for l in open(sys.argv[2]) if l.strip().startswith('{')]
assert len(a) == len(b)
other, nodes, shifted_only = 0, [], True
for x, y in zip(a, b):
    assert x['case'] == y['case']
    for key in x:
        if key != 'floats' and x[key] != y[key]:
            other += 1
            print('DIFF', x['case'], key)
    if dict(x.get('floats', {})) != dict(y.get('floats', {})):
        nodes.append(x['case'])
        shifted_only &= 'shifted' in x['case']
print(f'{len(a)} cases; differing fields other than floats: {other}; nodeChecksum differs in '
      f'{len(nodes)} cases, all of them on shifted domains: {shifted_only}')
