#!/usr/bin/env python3
"""
ingest_ganapatha.py — Parse Gaṇapāṭha into TSV.

Usage:
  python3 tools/ingest_ganapatha.py           # generate data/ganapatha.tsv
  python3 tools/ingest_ganapatha.py --validate # validate existing TSV

See STORY_0_4_ganapatha_ingestion.md for full specification.
"""

import argparse, csv, json, os, sys, unicodedata, urllib.request

GANA_SOURCE_URL = (
    "https://raw.githubusercontent.com/ashtadhyayi-com/data/master/ganapatha/ganapatha.json"
)
FALLBACK_JSON = os.path.join(os.path.dirname(__file__), "../vendor/ganapatha_fallback.json")
OUTPUT_TSV    = os.path.join(os.path.dirname(__file__), "../data/ganapatha.tsv")

# 14 Māheśvara-sūtras encoded inline
MAHESHVARA_SUTRAS = [
    "aiuR",
    "fxk",
    "eoN",
    "EOc",
    "hyvr w",
    "lN",
    "YmGRnm",
    "JBYA",
    "GQz",
    "jbgqdS",
    "KPCWTcwtv",
    "kpy",
    "SzsR",
    "hl",
]

def iast_to_slp1_simple(text: str) -> str:
    """Minimal converter for ganapatha (extend in full implementation)."""
    table = [
        ("ā","A"),("ī","I"),("ū","U"),("ṛ","f"),("ṅ","N"),("ñ","Y"),
        ("ṭ","w"),("ḍ","q"),("ṇ","R"),("ś","S"),("ṣ","z"),("ṃ","M"),("ḥ","H"),
        ("kh","K"),("gh","G"),("ch","C"),("jh","J"),("ṭh","W"),("ḍh","Q"),
        ("th","T"),("dh","D"),("ph","P"),("bh","B"),("ai","E"),("au","O"),
    ]
    text = unicodedata.normalize("NFC", text)
    result, i = [], 0
    while i < len(text):
        matched = False
        for src, dst in table:
            if text[i:i+len(src)] == src:
                result.append(dst); i += len(src); matched = True; break
        if not matched:
            result.append(text[i]); i += 1
    return "".join(result)


def fetch_data() -> list:
    if os.path.exists(FALLBACK_JSON):
        with open(FALLBACK_JSON, encoding="utf-8") as f:
            return json.load(f)
    try:
        print("Fetching gaṇapāṭha ...", file=sys.stderr)
        with urllib.request.urlopen(GANA_SOURCE_URL, timeout=30) as resp:
            data = json.loads(resp.read())
        os.makedirs(os.path.dirname(FALLBACK_JSON), exist_ok=True)
        with open(FALLBACK_JSON, "w", encoding="utf-8") as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        return data
    except Exception as e:
        print(f"ERROR fetching gaṇapāṭha: {e}", file=sys.stderr)
        return []  # Fall through to minimal set


def generate(data: list):
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    gana_id = 0
    rows = []

    # Add Māheśvara-sūtras as special gaṇa
    gana_id += 1
    for i, sutra in enumerate(MAHESHVARA_SUTRAS, 1):
        rows.append([gana_id, "mAheSvarasUtra", "māheśvarasūtra", i, sutra, sutra, ""])

    # Add gaṇas from data
    if data:
        for gana_entry in data:
            gana_name_iast = unicodedata.normalize("NFC",
                str(gana_entry.get("gana_name", gana_entry.get("name", ""))))
            if not gana_name_iast:
                continue
            gana_name_slp1 = iast_to_slp1_simple(gana_name_iast)
            gana_id += 1
            members = gana_entry.get("members", gana_entry.get("words", []))
            for j, member in enumerate(members, 1):
                if isinstance(member, dict):
                    member_iast = unicodedata.normalize("NFC", str(member.get("word", "")))
                    note = str(member.get("note", ""))
                else:
                    member_iast = unicodedata.normalize("NFC", str(member))
                    note = ""
                member_slp1 = iast_to_slp1_simple(member_iast)
                rows.append([gana_id, gana_name_slp1, gana_name_iast,
                              j, member_slp1, member_iast, note])

    with open(OUTPUT_TSV, "w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", quoting=csv.QUOTE_MINIMAL)
        writer.writerow(["gana_id","gana_name_slp1","gana_name_iast",
                          "serial","member_slp1","member_iast","note"])
        writer.writerows(rows)

    total_ganas = gana_id
    print(f"Generated {OUTPUT_TSV}: {total_ganas} gaṇas, {len(rows)} members")


def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/ganapatha.tsv not found."); sys.exit(1)
    gana_names = set()
    count = 0
    with open(OUTPUT_TSV, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            gana_names.add(row["gana_name_slp1"])
            count += 1
    errors = []
    if "mAheSvarasUtra" not in gana_names:
        errors.append("Māheśvara-sūtra gaṇa missing")
    maheshvara_members = 0
    with open(OUTPUT_TSV, encoding="utf-8") as f:
        for line in f:
            if "mAheSvarasUtra" in line:
                maheshvara_members += 1
    maheshvara_members -= 1  # subtract header match
    if maheshvara_members != 14:
        errors.append(f"Māheśvara-sūtras: expected 14 members, got {maheshvara_members}")
    if errors:
        for e in errors: print(f"  ERROR: {e}")
        sys.exit(1)
    print(f"PASS: {len(gana_names)} gaṇas, {count} members loaded")
    print(f"      Māheśvara-sūtra gaṇa: {maheshvara_members} entries ✓")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--validate", action="store_true")
    args = parser.parse_args()
    if args.validate: validate()
    else: generate(fetch_data())


if __name__ == "__main__":
    main()
