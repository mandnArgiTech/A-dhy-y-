#!/usr/bin/env python3
"""
ingest_shabdaprakriya.py — Parse pre-computed prakriyā (derivation) traces.

Source: github.com/ashtadhyayi-com/data  (shabda/shabdaprakriya.txt)

This is the GOLD ORACLE for trace validation — 4,863 full subanta derivations
with every step and the exact sūtras that applied. Example for "अः" (a, nom sg):

  Step 1: अ + स्             [1.3.2, 1.3.9]   (anubandha stripping)
  Step 2: अ + रुँ           [8.2.66]          (s → ru̇ͅ)
  Step 3: अ + र्             [1.3.2, 1.3.9]   (strip anubandhas)
  Step 4: अः                 [8.3.15]         (visarga)

Our C engine MUST produce the same trace — step, sūtra references, final form.

REAL SCHEMA:
  { "name": "shabdaprakriya", "data": [
      { "form":  "अः",              ← final surface form
        "baseindex": "01.001",
        "word":  "अ",                ← prātipadika
        "vibhakti": "1", "vachan": "1",
        "steps": [
          { "step": "अ + स्",
            "sutras": ["1.3.2", "1.3.9"] },
          ...
        ]
      }, ...  4863 entries
  ]}

Usage:
  python3 tools/ingest_shabdaprakriya.py           # generate data/shabdaprakriya.tsv
  python3 tools/ingest_shabdaprakriya.py --validate
"""

import argparse, csv, json, os, sys, unicodedata

RAW_BASE = "https://raw.githubusercontent.com/ashtadhyayi-com/data/master"
URL      = f"{RAW_BASE}/shabda/shabdaprakriya.txt"
FALLBACK = os.path.join(os.path.dirname(__file__), "../vendor/shabdaprakriya_fallback.json")
OUTPUT_TSV = os.path.join(os.path.dirname(__file__), "../data/shabdaprakriya.tsv")

def fetch_data():
    if os.path.exists(FALLBACK):
        with open(FALLBACK, encoding='utf-8') as f:
            return json.load(f)
    try:
        import urllib.request
        print(f"Fetching {URL} ...", file=sys.stderr)
        with urllib.request.urlopen(URL, timeout=60) as resp:
            data = json.loads(resp.read())
        os.makedirs(os.path.dirname(FALLBACK), exist_ok=True)
        with open(FALLBACK, 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False)
        return data
    except Exception as e:
        print(f"ERROR: {e}", file=sys.stderr); sys.exit(1)

def generate(data):
    entries = data.get('data', [])
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    count = 0
    with open(OUTPUT_TSV, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t', quoting=csv.QUOTE_MINIMAL)
        writer.writerow(['stem_deva','baseindex','form_deva','vibhakti','vacana',
                          'step_num','step_form_deva','sutras'])
        for entry in entries:
            word = unicodedata.normalize('NFC', entry.get('word', ''))
            baseindex = entry.get('baseindex', '')
            form = unicodedata.normalize('NFC', entry.get('form', ''))
            vib = entry.get('vibhakti', '')
            vac = entry.get('vachan', '')
            for step_num, step in enumerate(entry.get('steps', []), 1):
                step_form = unicodedata.normalize('NFC', step.get('step', ''))
                sutras = ','.join(step.get('sutras', []))
                writer.writerow([word, baseindex, form, vib, vac,
                                  step_num, step_form, sutras])
                count += 1
    print(f"Generated {OUTPUT_TSV}: {len(entries)} prakriyās, {count} steps")

def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/shabdaprakriya.tsv not found."); sys.exit(1)
    count = 0
    prakriyas = set()
    with open(OUTPUT_TSV, encoding='utf-8') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            count += 1
            prakriyas.add((row['stem_deva'], row['form_deva']))
    if count < 10000:
        print(f"FAIL: Only {count} steps"); sys.exit(1)
    print(f"PASS: {count} prakriyā steps across {len(prakriyas)} forms ✓")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--validate', action='store_true')
    args = parser.parse_args()
    if args.validate: validate()
    else: generate(fetch_data())

if __name__ == '__main__':
    main()
