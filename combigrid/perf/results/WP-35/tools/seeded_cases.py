# Prints a Boost --run_test filter of every test case whose body calls randGen.setSeed() (clock).
import re, subprocess, sys, collections
files = subprocess.run(['grep', '-rl', r'setSeed()', '.', '--include=*.cpp'], capture_output=True, text=True).stdout.split()
suites = collections.OrderedDict()
for f in files:
    suite_stack = []
    case = None; body = []
    for line in open(f):
        m = re.match(r'BOOST_(?:AUTO|FIXTURE)_TEST_SUITE\((\w+)', line)
        if m: suite_stack.append(m.group(1)); continue
        if line.startswith('BOOST_AUTO_TEST_SUITE_END'):
            if case and any(k in ''.join(body) for k in ('setSeed()', 'withClockSeed(', 'withSeeds(')): suites.setdefault('/'.join(suite_stack), []).append(case)
            case = None; body = []; suite_stack.pop(); continue
        m = re.match(r'BOOST_AUTO_TEST_CASE\((\w+)', line)
        if m:
            if case and any(k in ''.join(body) for k in ('setSeed()', 'withClockSeed(', 'withSeeds(')): suites.setdefault('/'.join(suite_stack), []).append(case)
            case = m.group(1); body = []; continue
        body.append(line)
print(':'.join((s + '/' if s else '') + ','.join(c) for s, c in suites.items()))
print(sum(len(c) for c in suites.values()), 'cases in', len(suites), 'suites', file=sys.stderr)
