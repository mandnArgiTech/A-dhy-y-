#!/usr/bin/env python3
"""
ingest_dhatupatha.py — Parse Dhātupāṭha into TSV.

Source: github.com/ashtadhyayi-com/data  (dhatu/data.txt)

REAL SCHEMA (confirmed by inspection):
  { "name": "dhatu", "data": [
      { "i": "1010001",          <- encoded id
        "baseindex": "01.0001",  <- gana.serial
        "dhatu": "भू",           <- Devanagari display form
        "aupadeshik": "भू",      <- upadesa form (Devanagari)
        "gana": "1",             <- gaṇa 1-10
        "pada": "P",             <- P/A/U (parasmai/atmane/ubhaya)
        "settva": "S",           <- S=set A=aset V=vet
        "artha": "सत्तायाम्",   <- Sanskrit meaning
        "artha_english": "to exist, to become...",
        ...
      }, ...
  ]}

Usage:
  python3 tools/ingest_dhatupatha.py           # generate data/dhatupatha.tsv
  python3 tools/ingest_dhatupatha.py --validate # validate
"""

import argparse, csv, json, os, sys, unicodedata

RAW_BASE   = "https://raw.githubusercontent.com/ashtadhyayi-com/data/master"
DHATU_URL  = f"{RAW_BASE}/dhatu/data.txt"
FALLBACK   = os.path.join(os.path.dirname(__file__), "../vendor/dhatupatha_fallback.json")
OUTPUT_TSV = os.path.join(os.path.dirname(__file__), "../data/dhatupatha.tsv")
OUTPUT_H   = os.path.join(os.path.dirname(__file__), "../vendor/dhatupatha_count.h")

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
    '्':'', '^':'^', '~':'~',
}

def deva_to_slp1(text: str) -> str:
    text = unicodedata.normalize('NFC', text)
    result = []
    for ch in text:
        if ch in DEVA_TO_SLP1:
            result.append(DEVA_TO_SLP1[ch])
        elif ord(ch) < 128:
            result.append(ch)
    return ''.join(result)


def fetch_data() -> dict:
    if os.path.exists(FALLBACK):
        with open(FALLBACK, encoding='utf-8') as f:
            return json.load(f)
    try:
        import urllib.request
        print(f"Fetching {DHATU_URL} ...", file=sys.stderr)
        with urllib.request.urlopen(DHATU_URL, timeout=30) as resp:
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
    count = 0
    with open(OUTPUT_TSV, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t', quoting=csv.QUOTE_MINIMAL)
        writer.writerow(['global_id','gana','serial_in_gana',
                          'upadesa_deva','upadesa_slp1',
                          'meaning_skt','meaning_en','pada_flag','settva'])
        for entry in entries:
            try:
                baseindex = entry.get('baseindex', '01.0001')
                gana_str, serial_str = baseindex.split('.')
                gana   = int(gana_str)
                serial = int(serial_str)
                upadesa_deva = unicodedata.normalize('NFC',
                    str(entry.get('aupadeshik', entry.get('dhatu', ''))))
                upadesa_slp1 = deva_to_slp1(upadesa_deva)
                meaning_skt  = str(entry.get('artha', ''))
                meaning_en   = str(entry.get('artha_english', ''))
                pada_flag    = str(entry.get('pada', 'U')).strip().upper()
                settva       = str(entry.get('settva', '')).strip().upper()
                count       += 1
                writer.writerow([count, gana, serial, upadesa_deva, upadesa_slp1,
                                  meaning_skt, meaning_en, pada_flag, settva])
            except (ValueError, KeyError) as e:
                print(f"  SKIP {entry.get('i','?')}: {e}", file=sys.stderr)

    os.makedirs(os.path.dirname(OUTPUT_H), exist_ok=True)
    with open(OUTPUT_H, 'w', encoding='utf-8') as f:
        f.write("/* Auto-generated — do not edit */\n")
        f.write(f"#define ASH_DHATU_COUNT {count}\n")
        f.write("#define ASH_GANA_COUNT 10\n")
    print(f"Generated {OUTPUT_TSV}: {count} dhātus")


def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/dhatupatha.tsv not found."); sys.exit(1)
    count = 0
    errors = []
    with open(OUTPUT_TSV, encoding='utf-8') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            count += 1
            g = int(row['gana'])
            if not (1 <= g <= 10):
                errors.append(f"Row {count}: gana={g} out of range")
            if not row['upadesa_slp1'].strip():
                errors.append(f"Row {count}: empty upadesa_slp1")
    # Key spot-checks using Devanagari
    expected = {'भू': False, 'गमॢ': False, 'चुर्': False}
    with open(OUTPUT_TSV, encoding='utf-8') as f:
        for line in f:
            for k in expected:
                if k in line:
                    expected[k] = True
    for k, found in expected.items():
        if not found:
            errors.append(f"Root '{k}' not found in dhātupāṭha")
    if errors:
        for e in errors[:10]: print(f"  ERROR: {e}")
        sys.exit(1)
    if count < 1938:
        print(f"FAIL: Only {count} dhātus"); sys.exit(1)
    print(f"PASS: {count} dhātus loaded across 10 gaṇas ✓")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--validate', action='store_true')
    args = parser.parse_args()
    if args.validate: validate()
    else: generate(fetch_data())

if __name__ == '__main__':
    main()
