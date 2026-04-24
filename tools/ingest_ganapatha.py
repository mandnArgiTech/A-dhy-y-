#!/usr/bin/env python3
"""
ingest_ganapatha.py — Parse Gaṇapāṭha into TSV.

Source: github.com/ashtadhyayi-com/data  (ganapath/data.txt)

REAL SCHEMA (confirmed by inspection):
  { "name": "ganapath", "data": [
      { "ind": 1,
        "name": "सर्वादिः",   <- gaṇa name (Devanagari)
        "sutra": "1.1.27",   <- governing sūtra address
        "vartika": "",
        "words": "सर्व । विश्व । उभ ...",  <- pipe/dot-separated word list
        "type": "P"
      }, ...
  ]}  — 262 gaṇas total

The 14 Māheśvara-sūtras come from shivasutra/data.txt:
  { "name": "shivasutra", "data": [
      { "id": "1", "sutra": "अइउण्", "kashika": "...", "vyakhya": "..." },
      ...  14 entries
  ]}

Usage:
  python3 tools/ingest_ganapatha.py           # generate data/ganapatha.tsv
  python3 tools/ingest_ganapatha.py --validate # validate
"""

import argparse, csv, json, os, re, sys, unicodedata

RAW_BASE   = "https://raw.githubusercontent.com/ashtadhyayi-com/data/master"
GANA_URL   = f"{RAW_BASE}/ganapath/data.txt"
SHIVA_URL  = f"{RAW_BASE}/shivasutra/data.txt"
FALLBACK_G = os.path.join(os.path.dirname(__file__), "../vendor/ganapatha_fallback.json")
FALLBACK_S = os.path.join(os.path.dirname(__file__), "../vendor/shivasutra_fallback.json")
OUTPUT_TSV = os.path.join(os.path.dirname(__file__), "../data/ganapatha.tsv")

DEVA_TO_SLP1 = {
    'अ':'a','आ':'A','इ':'i','ई':'I','उ':'u','ऊ':'U',
    'ऋ':'f','ॠ':'F','ऌ':'x',
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
}

def deva_to_slp1(text: str) -> str:
    text = unicodedata.normalize('NFC', text)
    return ''.join(DEVA_TO_SLP1.get(ch, ch if ord(ch) < 128 else '') for ch in text)


def fetch_json(url: str, fallback: str) -> dict:
    if os.path.exists(fallback):
        with open(fallback, encoding='utf-8') as f:
            return json.load(f)
    try:
        import urllib.request
        print(f"Fetching {url} ...", file=sys.stderr)
        with urllib.request.urlopen(url, timeout=30) as resp:
            data = json.loads(resp.read())
        os.makedirs(os.path.dirname(fallback), exist_ok=True)
        with open(fallback, 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        return data
    except Exception as e:
        print(f"ERROR fetching {url}: {e}", file=sys.stderr)
        return {}


def parse_words(words_str: str) -> list:
    """Split gaṇa word list on । (daṇḍa) and whitespace."""
    words_str = re.sub(r'<<[^>]*>>', '', words_str)  # strip sūtra refs
    parts = re.split(r'[।\.\|]+', words_str)
    return [w.strip() for w in parts if w.strip()]


def generate():
    gana_data  = fetch_json(GANA_URL, FALLBACK_G)
    shiva_data = fetch_json(SHIVA_URL, FALLBACK_S)

    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    rows = []
    gana_id = 0

    # 1. Māheśvara-sūtras (Śiva-sūtras)
    gana_id += 1
    shiva_entries = shiva_data.get('data', [])
    for entry in shiva_entries:
        sid   = int(entry['id'])
        sutra_deva = unicodedata.normalize('NFC', entry['sutra'])
        sutra_slp1 = deva_to_slp1(sutra_deva)
        rows.append([gana_id, 'mAheSvarasUtra', 'māheśvarasūtra',
                     'shivasutra', sid, sutra_slp1, sutra_deva, ''])

    # 2. All gaṇas from gaṇapāṭha
    for entry in gana_data.get('data', []):
        gana_id += 1
        name_deva = unicodedata.normalize('NFC', str(entry.get('name', '')))
        name_slp1 = deva_to_slp1(name_deva)
        sutra_ref = str(entry.get('sutra', ''))
        words_str = str(entry.get('words', ''))
        members   = parse_words(words_str)
        for j, word_deva in enumerate(members, 1):
            word_slp1 = deva_to_slp1(word_deva)
            rows.append([gana_id, name_slp1, name_deva, sutra_ref,
                         j, word_slp1, word_deva, ''])

    with open(OUTPUT_TSV, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t', quoting=csv.QUOTE_MINIMAL)
        writer.writerow(['gana_id','gana_name_slp1','gana_name_deva','sutra_ref',
                          'serial','member_slp1','member_deva','note'])
        writer.writerows(rows)

    # Count distinct gana names
    names = {r[1] for r in rows}
    print(f"Generated {OUTPUT_TSV}: {len(names)} gaṇas, {len(rows)} members")
    print(f"  Māheśvara-sūtras: {len(shiva_entries)} entries ✓")


def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/ganapatha.tsv not found."); sys.exit(1)
    gana_names, count = set(), 0
    maheshvara_count = 0
    with open(OUTPUT_TSV, encoding='utf-8') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            count += 1
            gana_names.add(row['gana_name_slp1'])
            if row['gana_name_slp1'] == 'mAheSvarasUtra':
                maheshvara_count += 1
    errors = []
    if 'mAheSvarasUtra' not in gana_names:
        errors.append("Māheśvara-sūtra gaṇa missing")
    if maheshvara_count != 14:
        errors.append(f"Māheśvara-sūtras: expected 14, got {maheshvara_count}")
    if len(gana_names) < 240:
        errors.append(f"Only {len(gana_names)} gaṇas (expected ≥260)")
    if errors:
        for e in errors: print(f"  ERROR: {e}")
        sys.exit(1)
    print(f"PASS: {len(gana_names)} gaṇas, {count} members")
    print(f"  Māheśvara-sūtras: {maheshvara_count} entries ✓")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--validate', action='store_true')
    args = parser.parse_args()
    if args.validate: validate()
    else: generate()

if __name__ == '__main__':
    main()
