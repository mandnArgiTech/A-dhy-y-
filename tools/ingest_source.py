#!/usr/bin/env python3
"""
ingest_source.py — Parse Ashtadhyayi Sutrapatha into TSV.

Usage:
  python3 tools/ingest_source.py           # generate data/sutras.tsv
  python3 tools/ingest_source.py --validate # validate existing TSV

See STORY_0_2_sutrapatha_ingestion.md for full specification.
"""

import argparse
import csv
import json
import os
import sys
import unicodedata
import urllib.request

SUTRA_SOURCE_URL = (
    "https://raw.githubusercontent.com/ashtadhyayi-com/data/master/sutras/sutra-details.json"
)
FALLBACK_JSON = os.path.join(os.path.dirname(__file__), "../vendor/sutra_fallback.json")
OUTPUT_TSV    = os.path.join(os.path.dirname(__file__), "../data/sutras.tsv")
OUTPUT_HEADER = os.path.join(os.path.dirname(__file__), "../vendor/sutras_count.h")

# ── SLP1 mapping (IAST char → SLP1 char/str) ─────────────────────────────────
# Longest-match order (multi-char IAST first)
IAST_TO_SLP1 = [
    ("ā",  "A"), ("Ā",  "A"),
    ("ī",  "I"), ("Ī",  "I"),
    ("ū",  "U"), ("Ū",  "U"),
    ("ṛ",  "f"), ("Ṛ",  "f"),
    ("ṝ",  "F"), ("Ṝ",  "F"),
    ("ḷ",  "x"), ("Ḷ",  "x"),
    ("ḹ",  "X"), ("Ḹ",  "X"),
    ("ṃ",  "M"), ("Ṃ",  "M"),
    ("ḥ",  "H"), ("Ḥ",  "H"),
    ("ṅ",  "N"), ("Ṅ",  "N"),
    ("ñ",  "Y"), ("Ñ",  "Y"),
    ("ṭ",  "w"), ("Ṭ",  "w"),
    ("ṭh", "W"), ("Ṭh", "W"),
    ("ḍ",  "q"), ("Ḍ",  "q"),
    ("ḍh", "Q"), ("Ḍh", "Q"),
    ("ṇ",  "R"), ("Ṇ",  "R"),
    ("ś",  "S"), ("Ś",  "S"),
    ("ṣ",  "z"), ("Ṣ",  "z"),
    ("kh", "K"),
    ("gh", "G"),
    ("ch", "C"),
    ("jh", "J"),
    ("ṭh", "W"),
    ("ḍh", "Q"),
    ("th", "T"),
    ("dh", "D"),
    ("ph", "P"),
    ("bh", "B"),
    ("ai", "E"),
    ("au", "O"),
    ("a",  "a"), ("A",  "a"),
    ("i",  "i"), ("I",  "i"),
    ("u",  "u"), ("U",  "u"),
    ("e",  "e"), ("E",  "e"),
    ("o",  "o"), ("O",  "o"),
    ("k",  "k"), ("K",  "k"),
    ("g",  "g"), ("G",  "g"),
    ("c",  "c"), ("C",  "c"),
    ("j",  "j"), ("J",  "j"),
    ("t",  "t"), ("T",  "t"),
    ("d",  "d"), ("D",  "d"),
    ("n",  "n"), ("N",  "n"),
    ("p",  "p"), ("P",  "p"),
    ("b",  "b"), ("B",  "b"),
    ("m",  "m"), ("M",  "m"),
    ("y",  "y"), ("Y",  "y"),
    ("r",  "r"), ("R",  "r"),
    ("l",  "l"), ("L",  "l"),
    ("v",  "v"), ("V",  "v"),
    ("s",  "s"), ("S",  "s"),
    ("h",  "h"), ("H",  "h"),
]

def iast_to_slp1(text: str) -> str:
    """Convert IAST Unicode text to SLP1 ASCII."""
    text = unicodedata.normalize("NFC", text)
    result = []
    i = 0
    while i < len(text):
        matched = False
        for iast, slp1 in IAST_TO_SLP1:
            if text[i:i+len(iast)] == iast:
                result.append(slp1)
                i += len(iast)
                matched = True
                break
        if not matched:
            result.append(text[i])
            i += 1
    return "".join(result)


# Known adhikāra/samjñā sūtras (address → type)
KNOWN_TYPES = {
    (1, 1, 1):  "SAMJNA",
    (1, 1, 2):  "SAMJNA",
    (1, 1, 68): "SAMJNA",
    (1, 3, 1):  "SAMJNA",
    (1, 4, 14): "SAMJNA",
    (3, 1, 1):  "ADHIKARA",
    (3, 4, 1):  "ADHIKARA",
    (4, 1, 1):  "ADHIKARA",
    (6, 1, 1):  "ADHIKARA",
    (6, 4, 1):  "ADHIKARA",
    (8, 2, 1):  "PARIBHASHA",
}


def fetch_data() -> list:
    """Fetch sūtra data from remote or fallback."""
    # Try fallback first (faster, works offline)
    if os.path.exists(FALLBACK_JSON):
        with open(FALLBACK_JSON, encoding="utf-8") as f:
            return json.load(f)
    try:
        print(f"Fetching from {SUTRA_SOURCE_URL} ...", file=sys.stderr)
        with urllib.request.urlopen(SUTRA_SOURCE_URL, timeout=30) as resp:
            data = json.loads(resp.read())
        os.makedirs(os.path.dirname(FALLBACK_JSON), exist_ok=True)
        with open(FALLBACK_JSON, "w", encoding="utf-8") as f:
            json.dump(data, f, ensure_ascii=False, indent=2)
        return data
    except Exception as e:
        print(f"ERROR: Cannot fetch data and no fallback found: {e}", file=sys.stderr)
        sys.exit(1)


def generate(data: list):
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    count = 0
    with open(OUTPUT_TSV, "w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t", quoting=csv.QUOTE_MINIMAL)
        writer.writerow(["global_id", "adhyaya", "pada", "num",
                          "text_iast", "text_slp1", "sutra_type"])
        for entry in data:
            try:
                adhyaya = int(entry.get("adhyaya", entry.get("chapter", 0)))
                pada    = int(entry.get("pada", entry.get("quarter", 0)))
                num     = int(entry.get("sutra_num", entry.get("number", 0)))
                text_iast = unicodedata.normalize("NFC",
                              str(entry.get("sutra", entry.get("text_iast", ""))))
                global_id = count + 1
                text_slp1 = iast_to_slp1(text_iast)
                sutra_type = KNOWN_TYPES.get((adhyaya, pada, num), "VIDHI")
                writer.writerow([global_id, adhyaya, pada, num,
                                  text_iast, text_slp1, sutra_type])
                count += 1
            except (ValueError, KeyError):
                continue

    # Write count header
    os.makedirs(os.path.dirname(OUTPUT_HEADER), exist_ok=True)
    with open(OUTPUT_HEADER, "w", encoding="utf-8") as f:
        f.write(f"/* Auto-generated by tools/ingest_source.py — do not edit */\n")
        f.write(f"#define ASH_SUTRA_COUNT {count}\n")
        f.write(f"#define ASH_SUTRA_TRADITIONAL_COUNT 3959\n")
        f.write(f"#define ASH_SUTRA_BORI_COUNT 3996\n")

    print(f"Generated {OUTPUT_TSV}: {count} sūtras")


def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/sutras.tsv not found. Run without --validate first.")
        sys.exit(1)
    count = 0
    prev_id = 0
    errors = []
    with open(OUTPUT_TSV, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            count += 1
            gid = int(row["global_id"])
            if gid != prev_id + 1:
                errors.append(f"Row {count}: global_id {gid} not monotonic (prev={prev_id})")
            prev_id = gid
            if not row["text_slp1"].strip():
                errors.append(f"Row {count}: empty text_slp1")
            if not (1 <= int(row["adhyaya"]) <= 8):
                errors.append(f"Row {count}: adhyaya={row['adhyaya']} out of range")
            if not (1 <= int(row["pada"]) <= 4):
                errors.append(f"Row {count}: pada={row['pada']} out of range")
    if errors:
        for e in errors[:10]:
            print(f"  ERROR: {e}")
        sys.exit(1)
    if count < 3959:
        print(f"FAIL: Only {count} sūtras loaded (expected ≥3959)")
        sys.exit(1)
    print(f"PASS: {count} sūtras loaded and validated")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--validate", action="store_true")
    args = parser.parse_args()
    if args.validate:
        validate()
    else:
        data = fetch_data()
        generate(data)


if __name__ == "__main__":
    main()
