#!/usr/bin/env python3
"""
ingest_shabda.py — Parse shabda (pre-computed nominal paradigms) into TSV.

Source: github.com/ashtadhyayi-com/data  (shabda/data2.txt)

REAL SCHEMA:
  { "name": "shabdapatha_v2", "data": [
      { "urlid": "@rAma1",
        "word": "राम",           ← prātipadika (Devanāgarī)
        "linga": "P",            ← P=puṃs S=strī N=napuṃsaka A=all
        "artha": "...",
        "artha_eng": "...",
        "forms": "रामः;रामौ;रामाः;रामम्;...",  ← 24 forms, ;-separated
                                              ← Order: P-E, P-D, P-B, Dv-E,...Vo-B
                                              ← 8 vibhaktis × 3 vacanas = 24
        "zbaseindex": "1.1"
      }, ... 9007 entries total
  ]}

This is the GOLD ORACLE for Phase 4 (subanta) validation — every prātipadika
has its 24 forms pre-computed by the same Aṣṭādhyāyī rules we implement.

Usage:
  python3 tools/ingest_shabda.py           # generate data/shabda_forms.tsv
  python3 tools/ingest_shabda.py --validate
"""

import argparse, csv, json, os, sys, unicodedata

RAW_BASE   = "https://raw.githubusercontent.com/ashtadhyayi-com/data/master"
SHABDA_URL = f"{RAW_BASE}/shabda/data2.txt"
FALLBACK   = os.path.join(os.path.dirname(__file__), "../vendor/shabda_fallback.json")
OUTPUT_TSV = os.path.join(os.path.dirname(__file__), "../data/shabda_forms.tsv")

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

VIBHAKTI_NAMES = ['prathama','dvitiya','tritiya','caturthi',
                   'pancami','shasthi','saptami','sambodhana']
VACANA_NAMES   = ['ekavacana','dvivacana','bahuvacana']
LINGA_MAP      = {'P':'PUMS','S':'STRI','N':'NAPUMSAKA','A':'ALL'}

def deva_to_slp1(text):
    text = unicodedata.normalize('NFC', text)
    return ''.join(DEVA_TO_SLP1.get(c, c if ord(c)<128 else '') for c in text)

def fetch_data():
    if os.path.exists(FALLBACK):
        with open(FALLBACK, encoding='utf-8') as f:
            return json.load(f)
    try:
        import urllib.request
        print(f"Fetching {SHABDA_URL} ...", file=sys.stderr)
        with urllib.request.urlopen(SHABDA_URL, timeout=60) as resp:
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
    count_words = count_forms = 0
    with open(OUTPUT_TSV, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t', quoting=csv.QUOTE_MINIMAL)
        writer.writerow(['stem_id','stem_deva','stem_slp1','linga',
                          'vibhakti','vacana','form_deva','form_slp1',
                          'artha_eng'])
        stem_id = 0
        for entry in entries:
            stem_id += 1
            word_deva = unicodedata.normalize('NFC', entry.get('word', ''))
            word_slp1 = deva_to_slp1(word_deva)
            linga     = LINGA_MAP.get(entry.get('linga', 'A'), 'ALL')
            artha_eng = entry.get('artha_eng', '')
            forms_str = entry.get('forms', '')
            if not forms_str: continue
            forms = forms_str.split(';')
            if len(forms) < 24: continue
            count_words += 1
            # Order: P-E,P-D,P-B, Dv-E,Dv-D,Dv-B, ... Sm-E,Sm-D,Sm-B  (8×3=24)
            for idx in range(24):
                vib_idx = idx // 3
                vac_idx = idx % 3
                form_deva = forms[idx].strip()
                form_slp1 = deva_to_slp1(form_deva)
                writer.writerow([stem_id, word_deva, word_slp1, linga,
                                  VIBHAKTI_NAMES[vib_idx], VACANA_NAMES[vac_idx],
                                  form_deva, form_slp1, artha_eng])
                count_forms += 1
    print(f"Generated {OUTPUT_TSV}: {count_words} prātipadikas, {count_forms} forms")

def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/shabda_forms.tsv not found."); sys.exit(1)
    count = 0
    rama_forms = {}
    with open(OUTPUT_TSV, encoding='utf-8') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            count += 1
            if row['stem_deva'] == 'राम':
                key = f"{row['vibhakti']}-{row['vacana']}"
                rama_forms[key] = row['form_deva']
    if count < 100000:
        print(f"FAIL: Only {count} forms (expected ≥100k)"); sys.exit(1)
    # Spot check rāma nom sg = रामः
    if rama_forms.get('prathama-ekavacana') != 'रामः':
        print(f"FAIL: rāma nom sg = {rama_forms.get('prathama-ekavacana')} (expected रामः)")
        sys.exit(1)
    print(f"PASS: {count} forms loaded ✓")
    print(f"  rāma nom sg: {rama_forms.get('prathama-ekavacana')}")
    print(f"  rāma acc pl: {rama_forms.get('dvitiya-bahuvacana')}")
    print(f"  rāma inst sg: {rama_forms.get('tritiya-ekavacana')}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--validate', action='store_true')
    args = parser.parse_args()
    if args.validate: validate()
    else: generate(fetch_data())

if __name__ == '__main__':
    main()
