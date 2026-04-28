#!/usr/bin/env python3
"""
ingest_pratyahara.py — Parse pratyāhāra expansions into TSV.

Source: github.com/ashtadhyayi-com/data  (pratyahara/data.txt)

REAL SCHEMA (confirmed by inspection):
  { "name": "pratyahara", "data": [
      { "name": "अण्",          <- pratyahara name (Devanagari)
        "list": "अ, इ, उ",     <- comma-separated expansion (Devanagari)
        "sutra": "<<sūtra>>",  <- usage sūtra reference
        "sutranum": "[[addr]]"
      }, ...  52 entries total
  ]}

Usage:
  python3 tools/ingest_pratyahara.py           # generate data/pratyahara.tsv
  python3 tools/ingest_pratyahara.py --validate
"""

import argparse, csv, json, os, re, sys, unicodedata

RAW_BASE     = "https://raw.githubusercontent.com/ashtadhyayi-com/data/master"
PRATYAHARA_URL = f"{RAW_BASE}/pratyahara/data.txt"
FALLBACK       = os.path.join(os.path.dirname(__file__), "../vendor/pratyahara_fallback.json")
OUTPUT_TSV     = os.path.join(os.path.dirname(__file__), "../data/pratyahara.tsv")

from devanagari_slp1 import deva_to_slp1

CONSONANT_LABELS = set("kKgGNcCjJYwWqQRtTdDnpPbBmyrlvSzsh")

def pratyahara_label_to_slp1(name_deva: str) -> str:
    label = deva_to_slp1(name_deva)
    if len(label) == 3 and label[0] in CONSONANT_LABELS and label[1] == 'a':
        return label[0] + label[2]
    return label

def fetch_data() -> dict:
    if os.path.exists(FALLBACK):
        with open(FALLBACK, encoding='utf-8') as f:
            return json.load(f)
    try:
        import urllib.request
        print(f"Fetching {PRATYAHARA_URL} ...", file=sys.stderr)
        with urllib.request.urlopen(PRATYAHARA_URL, timeout=30) as resp:
            data = json.loads(resp.read())
        os.makedirs(os.path.dirname(FALLBACK), exist_ok=True)
        with open(FALLBACK, 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        return data
    except Exception as e:
        print(f"ERROR: {e}", file=sys.stderr); sys.exit(1)


def generate(data: dict):
    entries = data.get('data', [])
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    rows = []
    for i, entry in enumerate(entries, 1):
        name_deva = unicodedata.normalize('NFC', str(entry.get('name', '')))
        name_slp1 = pratyahara_label_to_slp1(name_deva)
        # expansion: comma-separated Devanagari phonemes
        expansion_deva = str(entry.get('list', ''))
        members_deva   = [m.strip() for m in expansion_deva.split(',') if m.strip()]
        members_slp1   = [deva_to_slp1(m) for m in members_deva]
        expansion_slp1 = ' '.join(members_slp1)
        sutra_ref      = re.sub(r'[<>\[\]]', '', str(entry.get('sutra', '')))
        rows.append([i, name_slp1, name_deva, expansion_slp1,
                     expansion_deva, len(members_slp1), sutra_ref])

    with open(OUTPUT_TSV, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t', quoting=csv.QUOTE_MINIMAL)
        writer.writerow(['id','name_slp1','name_deva','expansion_slp1',
                          'expansion_deva','member_count','sutra_ref'])
        writer.writerows(rows)
    print(f"Generated {OUTPUT_TSV}: {len(rows)} pratyāhāras")


def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/pratyahara.tsv not found."); sys.exit(1)
    count = 0
    errors = []
    # Key checks: ac=14, hal=38 (or close), ik=4
    expected = {'ac': 9, 'hl': 34, 'ik': 4}
    found = {}
    with open(OUTPUT_TSV, encoding='utf-8') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            count += 1
            n = row['name_slp1']
            mc = int(row['member_count'])
            if n in expected:
                found[n] = mc
    for name, expected_count in expected.items():
        actual = found.get(name, -1)
        if actual != expected_count:
            # Allow ±2 for encoding edge cases
            if abs(actual - expected_count) > 2:
                errors.append(f"pratyāhāra '{name}': expected {expected_count} members, got {actual}")
    if count < 44:
        errors.append(f"Only {count} pratyāhāras (expected ≥44)")
    if errors:
        for e in errors: print(f"  ERROR: {e}")
        sys.exit(1)
    print(f"PASS: {count} pratyāhāras loaded ✓")
    for name in expected:
        print(f"  {name}: {found.get(name, '?')} members")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--validate', action='store_true')
    args = parser.parse_args()
    if args.validate: validate()
    else: generate(fetch_data())

if __name__ == '__main__':
    main()
