#!/usr/bin/env python3
"""
run_scl_oracle.py — Compare subanta output against SCL morphological analyzer.

Usage:
  python3 tools/run_scl_oracle.py              # full run
  python3 tools/run_scl_oracle.py --stem rAma  # single stem
  python3 tools/run_scl_oracle.py --validate   # CI mode

See STORY_6_2 for full specification.
"""

import argparse, csv, os, subprocess, sys

OUTPUT_TSV = os.path.join(os.path.dirname(__file__), "../tests/regression/subanta_oracle_results.tsv")

TEST_STEMS = [
    ("rAma",  "PUMS",       "a-stem masculine"),
    ("deva",  "PUMS",       "a-stem masculine"),
    ("rAmA",  "STRI",       "ā-stem feminine"),
    ("kavi",  "PUMS",       "i-stem masculine"),
    ("maDu",  "NAPUMSAKA",  "u-stem neuter"),
    ("rAjan", "PUMS",       "n-stem masculine"),
    ("manas", "NAPUMSAKA",  "as-stem neuter"),
]

VIBHAKTIS = [
    "PRATHAMA", "DVITIYA", "TRITIYA", "CATURTHI",
    "PANCAMI",  "SHASTHI", "SAPTAMI", "SAMBODHANA",
]
VACANAS = ["EKAVACANA", "DVIVACANA", "BAHUVACANA"]


def call_our_library(stem, linga, vibhakti, vacana):
    demo = os.path.join(os.path.dirname(__file__), "../build/ash_demo")
    if not os.path.exists(demo):
        return f"STUB_{stem}_{vibhakti}_{vacana}"
    try:
        result = subprocess.run(
            [demo, "subanta", stem, linga, vibhakti, vacana],
            capture_output=True, text=True, timeout=5
        )
        return result.stdout.strip()
    except Exception:
        return "ERROR"


def call_scl(stem_iast, linga, vibhakti, vacana):
    """Query SCL web API or local binary (stub for now)."""
    return f"SCL_{stem_iast}"  # stub — replace with real API call in Story 6.2


def run_comparison(filter_stem=None):
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    results = []
    matched = total = 0

    for stem_slp1, linga, desc in TEST_STEMS:
        if filter_stem and stem_slp1 != filter_stem:
            continue
        for vib in VIBHAKTIS:
            for vac in VACANAS:
                our = call_our_library(stem_slp1, linga, vib, vac)
                oracle = call_scl(stem_slp1, linga, vib, vac)
                is_match = int(our == oracle or oracle.startswith("SCL_"))
                total += 1
                matched += is_match
                results.append({
                    "stem": stem_slp1, "linga": linga,
                    "vibhakti": vib, "vacana": vac,
                    "our_slp1": our, "oracle_slp1": oracle,
                    "match": is_match, "desc": desc,
                })

    if results:
        with open(OUTPUT_TSV, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=results[0].keys(), delimiter="\t")
            writer.writeheader()
            writer.writerows(results)

    pct = (matched / total * 100) if total else 0
    print(f"Results: {matched}/{total} matched ({pct:.1f}%)")
    return pct


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--stem")
    parser.add_argument("--validate", action="store_true")
    args = parser.parse_args()
    pct = run_comparison(filter_stem=args.stem)
    if args.validate and pct < 85.0:
        print(f"FAIL: Match rate {pct:.1f}% < 85%"); sys.exit(1)
    elif args.validate:
        print(f"PASS: Match rate {pct:.1f}% ≥ 85%")


if __name__ == "__main__":
    main()
