#!/usr/bin/env python3
"""
ingest_dhatuforms.py — Parse pre-computed tiṅanta forms into TSV.

Source: github.com/ashtadhyayi-com/data  (dhatu/dhatuforms_vidyut_shuddha_kartari.txt)

This is the GOLD ORACLE for Phase 3 (tiṅanta) validation — every dhātu has
9 forms per lakāra per pada pre-computed by the same rules we implement.

REAL SCHEMA:
  { "01.0001": {                    ← baseindex: gana.serial
      "plat":  "भवति;भवतः;...",    ← Parasmai laT  (9 forms: 3 puruṣa × 3 vacana)
      "plit":  "बभूव;बभूवतुः;...", ← Parasmai liT (perfect)
      "plut":  "...",               ← Parasmai luT (periphrastic future)
      "plrut": "...",               ← Parasmai lṛT (simple future)
      "plot":  "...",               ← Parasmai loT (imperative)
      "plang": "...",               ← Parasmai laṄ (imperfect)
      "pvidhiling": "...",          ← Parasmai vidhi-liṄ (optative)
      "pashirling": "...",          ← Parasmai āśīr-liṄ (benedictive)
      "plung": "...",               ← Parasmai luṄ (aorist)
      "plrung":"...",               ← Parasmai lṛṄ (conditional)
      ... alat, alit, etc. for ātmane
    },
    "01.0002": { ... },  ...  2229 dhātus total
  }

Forms ordered: prathama-eka, prathama-dvi, prathama-bahu,
               madhyama-eka, madhyama-dvi, madhyama-bahu,
               uttama-eka, uttama-dvi, uttama-bahu   (9 total)

Usage:
  python3 tools/ingest_dhatuforms.py           # generate data/dhatuforms.tsv
  python3 tools/ingest_dhatuforms.py --validate
"""

import argparse, csv, json, os, sys, unicodedata

RAW_BASE = "https://raw.githubusercontent.com/ashtadhyayi-com/data/master"
URL      = f"{RAW_BASE}/dhatu/dhatuforms_vidyut_shuddha_kartari.txt"
FALLBACK = os.path.join(os.path.dirname(__file__), "../vendor/dhatuforms_shuddha_kartari_fallback.json")
OUTPUT_TSV = os.path.join(os.path.dirname(__file__), "../data/dhatuforms.tsv")

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

# Key → (pada, lakara)
KEY_MAP = {
    'plat':  ('P', 'LAT'),   'alat':  ('A', 'LAT'),
    'plit':  ('P', 'LIT'),   'alit':  ('A', 'LIT'),
    'plut':  ('P', 'LUT'),   'alut':  ('A', 'LUT'),
    'plrut': ('P', 'LRT'),   'alrut': ('A', 'LRT'),
    'plot':  ('P', 'LOT'),   'alot':  ('A', 'LOT'),
    'plang': ('P', 'LAN'),   'alang': ('A', 'LAN'),
    'pvidhiling':  ('P', 'VIDHILIM'),  'avidhiling':  ('A', 'VIDHILIM'),
    'pashirling':  ('P', 'ASHIRLIM'),  'aashirling':  ('A', 'ASHIRLIM'),
    'plung': ('P', 'LUN'),   'alung': ('A', 'LUN'),
    'plrung':('P', 'LRN'),   'alrung':('A', 'LRN'),
}

PURUSHA_NAMES = ['PRATHAMA','MADHYAMA','UTTAMA']
VACANA_NAMES  = ['EKA','DVI','BAHU']

def deva_to_slp1(text):
    text = unicodedata.normalize('NFC', text)
    return ''.join(DEVA_TO_SLP1.get(c, c if ord(c)<128 else '') for c in text)

def fetch_data():
    if os.path.exists(FALLBACK):
        with open(FALLBACK, encoding='utf-8') as f:
            return json.load(f)
    try:
        import urllib.request
        print(f"Fetching {URL} ...", file=sys.stderr)
        with urllib.request.urlopen(URL, timeout=120) as resp:
            data = json.loads(resp.read())
        os.makedirs(os.path.dirname(FALLBACK), exist_ok=True)
        with open(FALLBACK, 'w', encoding='utf-8') as f:
            json.dump(data, f, ensure_ascii=False)
        return data
    except Exception as e:
        print(f"ERROR: {e}", file=sys.stderr); sys.exit(1)

def generate(data):
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    count_dhatus = count_forms = 0
    with open(OUTPUT_TSV, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t', quoting=csv.QUOTE_MINIMAL)
        writer.writerow(['baseindex','gana','serial','pada','lakara',
                          'purusha','vacana','form_deva','form_slp1'])
        for baseindex, lakara_dict in data.items():
            try:
                gana_s, serial_s = baseindex.split('.')
                gana, serial = int(gana_s), int(serial_s)
            except (ValueError, AttributeError):
                continue
            count_dhatus += 1
            for key, forms_str in lakara_dict.items():
                if key not in KEY_MAP or not forms_str: continue
                pada, lakara = KEY_MAP[key]
                forms = forms_str.split(';')
                if len(forms) < 9: continue
                for idx in range(9):
                    pu = PURUSHA_NAMES[idx // 3]
                    va = VACANA_NAMES[idx % 3]
                    form_deva = forms[idx].strip()
                    if not form_deva: continue
                    form_slp1 = deva_to_slp1(form_deva)
                    writer.writerow([baseindex, gana, serial, pada, lakara,
                                      pu, va, form_deva, form_slp1])
                    count_forms += 1
    print(f"Generated {OUTPUT_TSV}: {count_dhatus} dhātus, {count_forms} forms")

def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/dhatuforms.tsv not found."); sys.exit(1)
    count = 0
    bhu_forms = {}
    with open(OUTPUT_TSV, encoding='utf-8') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            count += 1
            if row['baseindex'] == '01.0001' and row['lakara'] == 'LAT':
                key = f"{row['pada']}-{row['purusha']}-{row['vacana']}"
                bhu_forms[key] = row['form_deva']
    if count < 50000:
        print(f"FAIL: Only {count} forms"); sys.exit(1)
    # bhū lat parasmai prathama eka = भवति
    if bhu_forms.get('P-PRATHAMA-EKA') != 'भवति':
        print(f"FAIL: bhū LAT P-PRATHAMA-EKA = {bhu_forms.get('P-PRATHAMA-EKA')} (expected भवति)")
        sys.exit(1)
    print(f"PASS: {count} tiṅanta forms loaded ✓")
    print(f"  bhū LAT parasmai prathama eka: {bhu_forms.get('P-PRATHAMA-EKA')}")
    print(f"  bhū LAT parasmai uttama bahu:  {bhu_forms.get('P-UTTAMA-BAHU')}")

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--validate', action='store_true')
    args = parser.parse_args()
    if args.validate: validate()
    else: generate(fetch_data())

if __name__ == '__main__':
    main()
