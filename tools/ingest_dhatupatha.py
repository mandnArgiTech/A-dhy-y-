#!/usr/bin/env python3
"""
ingest_dhatupatha.py — Parse Dhātupāṭha into TSV.

Usage:
  python3 tools/ingest_dhatupatha.py           # generate data/dhatupatha.tsv
  python3 tools/ingest_dhatupatha.py --validate # validate existing TSV

See STORY_0_3_dhatupatha_ingestion.md for full specification.
"""

import argparse, csv, json, os, sys, unicodedata, urllib.request

DHATU_SOURCE_URL = (
    "https://raw.githubusercontent.com/ashtadhyayi-com/data/master/dhatupatha/dhatupatha.json"
)
FALLBACK_JSON = os.path.join(os.path.dirname(__file__), "../vendor/dhatupatha_fallback.json")
OUTPUT_TSV    = os.path.join(os.path.dirname(__file__), "../data/dhatupatha.tsv")
OUTPUT_HEADER = os.path.join(os.path.dirname(__file__), "../vendor/dhatupatha_count.h")

# Minimal IAST→SLP1 (import full table from ingest_source.py in real implementation)
IAST_TO_SLP1 = [
    ("ā","A"),("ī","I"),("ū","U"),("ṛ","f"),("ṝ","F"),("ḷ","x"),
    ("ṃ","M"),("ḥ","H"),("ṅ","N"),("ñ","Y"),("ṭh","W"),("ṭ","w"),
    ("ḍh","Q"),("ḍ","q"),("ṇ","R"),("ś","S"),("ṣ","z"),
    ("kh","K"),("gh","G"),("ch","C"),("jh","J"),("th","T"),("dh","D"),
    ("ph","P"),("bh","B"),("ai","E"),("au","O"),
    ("a","a"),("i","i"),("u","u"),("e","e"),("o","o"),
    ("k","k"),("g","g"),("c","c"),("j","j"),("t","t"),("d","d"),
    ("n","n"),("p","p"),("b","b"),("m","m"),("y","y"),("r","r"),
    ("l","l"),("v","v"),("s","s"),("h","h"),
]

def iast_to_slp1(text: str) -> str:
    text = unicodedata.normalize("NFC", text)
    result, i = [], 0
    while i < len(text):
        matched = False
        for iast, slp1 in IAST_TO_SLP1:
            if text[i:i+len(iast)] == iast:
                result.append(slp1); i += len(iast); matched = True; break
        if not matched:
            result.append(text[i]); i += 1
    return "".join(result)


def fetch_data() -> list:
    if os.path.exists(FALLBACK_JSON):
        with open(FALLBACK_JSON, encoding="utf-8") as f:
            return json.load(f)
    try:
        print(f"Fetching dhātupāṭha ...", file=sys.stderr)
        with urllib.request.urlopen(DHATU_SOURCE_URL, timeout=30) as resp:
            data = json.loads(resp.read())
        os.makedirs(os.path.dirname(FALLBACK_JSON), exist_ok=True)
        with open(FALLBACK_JSON, "w", encoding="utf-8") as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        return data
    except Exception as e:
        print(f"ERROR fetching dhātupāṭha: {e}", file=sys.stderr)
        sys.exit(1)


def generate(data: list):
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    count = 0
    with open(OUTPUT_TSV, "w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", quoting=csv.QUOTE_MINIMAL)
        writer.writerow(["global_id","gana","serial_in_gana","upadesa_iast",
                          "upadesa_slp1","root_clean_slp1","it_flags",
                          "meaning_skt","meaning_en","pada_flag"])
        gana_serial = {}
        for entry in data:
            try:
                gana = int(entry.get("gana", entry.get("class", 0)))
                gana_serial[gana] = gana_serial.get(gana, 0) + 1
                upadesa_iast = unicodedata.normalize("NFC",
                    str(entry.get("dhatu", entry.get("root", ""))))
                upadesa_slp1 = iast_to_slp1(upadesa_iast)
                meaning_en = str(entry.get("meaning", entry.get("english", "")))
                meaning_skt = str(entry.get("artha", ""))
                pada_flag = str(entry.get("pada", "U")).strip().upper()
                if pada_flag not in ("P", "A", "U"):
                    pada_flag = "U"
                count += 1
                writer.writerow([count, gana, gana_serial[gana],
                                  upadesa_iast, upadesa_slp1,
                                  upadesa_slp1,  # TODO: strip in Story 2.3
                                  "",             # it_flags: TODO
                                  meaning_skt, meaning_en, pada_flag])
            except (ValueError, KeyError):
                continue

    os.makedirs(os.path.dirname(OUTPUT_HEADER), exist_ok=True)
    with open(OUTPUT_HEADER, "w") as f:
        f.write(f"/* Auto-generated — do not edit */\n")
        f.write(f"#define ASH_DHATU_COUNT {count}\n")
        f.write(f"#define ASH_GANA_COUNT 10\n")
    print(f"Generated {OUTPUT_TSV}: {count} dhātus")


def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/dhatupatha.tsv not found."); sys.exit(1)
    count = 0
    gana_counts = {}
    errors = []
    with open(OUTPUT_TSV, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            count += 1
            g = int(row["gana"])
            gana_counts[g] = gana_counts.get(g, 0) + 1
            if not (1 <= g <= 10):
                errors.append(f"Row {count}: gana={g} out of range")
            if not row["upadesa_slp1"].strip():
                errors.append(f"Row {count}: empty upadesa_slp1")
    # Spot checks
    found = {"BU": False, "gam": False, "cur": False}
    with open(OUTPUT_TSV, encoding="utf-8") as f:
        for line in f:
            for k in found:
                if k in line:
                    found[k] = True
    for k, v in found.items():
        if not v:
            errors.append(f"Root '{k}' not found in dhātupāṭha")
    if errors:
        for e in errors[:10]: print(f"  ERROR: {e}")
        sys.exit(1)
    if count < 1938:
        print(f"FAIL: Only {count} dhātus (expected ≥1938)"); sys.exit(1)
    print(f"PASS: {count} dhātus loaded across {len(gana_counts)} gaṇas")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--validate", action="store_true")
    args = parser.parse_args()
    if args.validate: validate()
    else: generate(fetch_data())


if __name__ == "__main__":
    main()
