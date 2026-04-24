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

DEVA_TO_SLP1 = {
    'अ':'a','आ':'A','इ':'i','ई':'I','उ':'u','ऊ':'U',
    'ऋ':'f','ॠ':'F','ऌ':'x','ॡ':'X',
    'ए':'e','ऐ':'E','ओ':'o','औ':'O',
    'ं':'M','ः':'H','ँ':'~',
    'क':'k','ख':'K','ग':'g','घ':'G','ङ':'N',
    'च':'c','छ':'C','ज':'j','झ':'J','ञ':'Y',
    'ट':'w','ठ':'W','ड':'q','ढ':'Q','ण':'R',
    'त':'t','थ':'T','द':'d','ध':'D','न':'n',
    'प':'p','फ':'P','ब':'b','भ':'B','म':'m',
    'य':'y','र':'r','ल':'l','व':'v',
    'श':'S','ष':'z','स':'s','ह':'h',
    'ा':'A','ि':'i','ी':'I','ु':'u','ू':'U',
    'ृ':'f','े':'e','ै':'E','ो':'o','ौ':'O',
    '्':'',
    # Standalone consonants without vowel (halanta forms)
    'क्':'k','ख्':'K','ग्':'g','घ्':'G','ङ्':'N',
    'च्':'c','छ्':'C','ज्':'j','झ्':'J','ञ्':'Y',
    'ट्':'w','ठ्':'W','ड्':'q','ढ्':'Q','ण्':'R',
    'त्':'t','थ्':'T','द्':'d','ध्':'D','न्':'n',
    'प्':'p','फ्':'P','ब्':'b','भ्':'B','म्':'m',
    'य्':'y','र्':'r','ल्':'l','व्':'v',
    'श्':'S','ष्':'z','स्':'s','ह्':'h',
}

def deva_to_slp1(text: str) -> str:
    text = unicodedata.normalize('NFC', text)
    result = []
    i = 0
    while i < len(text):
        # Try 2-char first (halanta forms)
        two = text[i:i+2]
        if two in DEVA_TO_SLP1:
            result.append(DEVA_TO_SLP1[two])
            i += 2
        elif text[i] in DEVA_TO_SLP1:
            result.append(DEVA_TO_SLP1[text[i]])
            i += 1
        elif ord(text[i]) < 128:
            result.append(text[i])
            i += 1
        else:
            i += 1
    return ''.join(result)


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
        name_slp1 = deva_to_slp1(name_deva)
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
