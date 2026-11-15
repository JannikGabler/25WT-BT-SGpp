#!/usr/bin/env python3
"""WP-56: per-round times of an `ab e2e` JSON (python3 rounds.py FILE [METRIC], ms)."""
import json
import sys

data = json.load(open(sys.argv[1]))["data"]
metric = sys.argv[2] if len(sys.argv) > 2 else "operator"
for key in sorted(data, key=lambda k: (k.split("|")[1], k)):
    vals = [1000 * statistics_value for rec in data[key] for statistics_value in rec["metrics"][metric]]
    print(f"{key:45s} " + " ".join(f"{v:7.1f}" for v in vals))
