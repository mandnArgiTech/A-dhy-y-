#!/usr/bin/env python3
"""
ingest_unadipatha.py — Generate data/unadipatha.tsv from real Uṇādi data.

Reads vendor/unaadi_fallback.json (748 sūtras with sūtra text, suffix and
Siddhānta-Kaumudī commentary) and emits a Devanāgarī + SLP1 TSV.
"""

import argparse
import csv
import json
import os
import sys
import unicodedata

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)

from devanagari_slp1 import deva_to_slp1  # noqa: E402

OUTPUT_TSV = os.path.join(HERE, "../data/unadipatha.tsv")
FALLBACK = os.path.join(HERE, "../vendor/unaadi_fallback.json")


def fetch_data():
    with open(FALLBACK, encoding="utf-8") as f:
        return json.load(f)


def normalize(s):
    return unicodedata.normalize("NFC", s or "")


def generate():
    data = fetch_data()
    entries = data.get("data", [])
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    with open(OUTPUT_TSV, "w", encoding="utf-8", newline="") as f:
        w = csv.writer(f, delimiter="\t", quoting=csv.QUOTE_MINIMAL)
        w.writerow(
            [
                "unadi_id",
                "sutra_deva",
                "sutra_slp1",
                "pratyay_deva",
                "pratyay_slp1",
                "sk_commentary",
                "sutra_ref",
            ]
        )
        for entry in entries:
            uid = int(entry.get("i", "0"))
            sutra_deva = normalize(entry.get("sutra", ""))
            sutra_slp1 = deva_to_slp1(sutra_deva)
            pratyay_deva = normalize(entry.get("pratyay", ""))
            pratyay_slp1 = deva_to_slp1(pratyay_deva)
            sk = normalize(entry.get("sk", "")).replace("\n", " ").replace("\t", " ")
            w.writerow(
                [
                    uid,
                    sutra_deva,
                    sutra_slp1,
                    pratyay_deva,
                    pratyay_slp1,
                    sk,
                    "",
                ]
            )
    print(f"Generated {OUTPUT_TSV}: {len(entries)} entries")


def validate():
    if not os.path.exists(OUTPUT_TSV):
        print("ERROR: data/unadipatha.tsv not found")
        sys.exit(1)

    count = 0
    first_pratyay_slp1 = None
    with open(OUTPUT_TSV, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            count += 1
            if not row.get("sutra_slp1", "").strip():
                print(f"ERROR: empty sutra_slp1 in row {count}")
                sys.exit(1)
            if first_pratyay_slp1 is None:
                first_pratyay_slp1 = row.get("pratyay_slp1", "")

    if count != 748:
        print(f"ERROR: expected 748 Uṇādi entries, got {count}")
        sys.exit(1)
    if first_pratyay_slp1 != "uR":
        print(f"ERROR: first row pratyay_slp1 expected 'uR', got {first_pratyay_slp1!r}")
        sys.exit(1)
    print(f"PASS: {count} uNAdi rows; first pratyay = {first_pratyay_slp1}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--validate", action="store_true")
    args = parser.parse_args()
    if args.validate:
        validate()
    else:
        generate()


if __name__ == "__main__":
    main()
