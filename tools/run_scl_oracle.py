#!/usr/bin/env python3
"""
run_scl_oracle.py — Compare subanta output against shabda_forms.tsv oracle.

Usage:
  python3 tools/run_scl_oracle.py
  python3 tools/run_scl_oracle.py --stem rAma
  python3 tools/run_scl_oracle.py --validate
"""

import argparse
import csv
import os
import subprocess
import sys
from collections import defaultdict

ROOT = os.path.dirname(__file__)
OUTPUT_TSV = os.path.join(ROOT, "../tests/regression/subanta_oracle_results.tsv")
SHABDA_TSV = os.path.join(ROOT, "../data/shabda_forms.tsv")
DEMO_BIN = os.path.join(ROOT, "../build/ash_demo")
TARGET = 85.0
SUPPORTED_STEMS = [
    ("rAm", "rAma", "PUMS"),
    ("rAmA", "rAmA", "STRI"),
    ("kvi", "kvi", "PUMS"),
    ("mDu", "mDu", "NAPUMSAKA"),
    ("rAjn", "rAjn", "PUMS"),
    ("mns", "mns", "NAPUMSAKA"),
    ("pitf", "pitf", "PUMS"),
]
SUPPORTED_VIBHAKTI = {"prathama"}
SUPPORTED_VACANA = {"ekavacana"}


def _normalize_stem(stem: str) -> str:
    if stem == "rAm":
        return "rAma"
    return stem


def _to_enum_case(vibhakti: str) -> str:
    mapping = {
        "prathama": "PRATHAMA",
        "dvitiya": "DVITIYA",
        "tritiya": "TRITIYA",
        "caturthi": "CATURTHI",
        "pancami": "PANCAMI",
        "shasthi": "SHASTHI",
        "saptami": "SAPTAMI",
        "sambodhana": "SAMBODHANA",
    }
    return mapping[vibhakti]


def _to_enum_number(vacana: str) -> str:
    mapping = {
        "ekavacana": "EKAVACANA",
        "dvivacana": "DVIVACANA",
        "bahuvacana": "BAHUVACANA",
    }
    return mapping[vacana]


def _load_subset():
    wanted = {(stem, linga): cli for stem, cli, linga in SUPPORTED_STEMS}
    groups = defaultdict(list)
    with open(SHABDA_TSV, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            key = (row["stem_slp1"], row["linga"])
            if key not in wanted:
                continue
            if row["vibhakti"] not in SUPPORTED_VIBHAKTI:
                continue
            if row["vacana"] not in SUPPORTED_VACANA:
                continue
            groups[key].append(row)
    chosen = []
    for stem, cli, linga in SUPPORTED_STEMS:
        key = (stem, linga)
        rows = groups.get(key, [])
        if rows:
            chosen.append((stem, cli, linga, rows))
    return chosen


def call_our_library(stem, linga, vibhakti, vacana):
    if not os.path.exists(DEMO_BIN):
        return None
    try:
        result = subprocess.run(
            [DEMO_BIN, "subanta", stem, linga, vibhakti, vacana],
            capture_output=True,
            text=True,
            timeout=5,
            check=False,
        )
        if result.returncode != 0:
            return None
        lines = [line.strip() for line in result.stdout.splitlines() if line.strip()]
        for line in lines:
            if line.startswith("libAshtadhyayi"):
                continue
            if line.startswith("─"):
                continue
            return line
        return None
    except Exception:
        return None


def run_comparison(filter_stem=None):
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    sample = _load_subset()
    if filter_stem:
        sample = [item for item in sample if _normalize_stem(item[1]) == filter_stem]

    total = 0
    matched = 0
    rows = []
    by_linga = defaultdict(lambda: [0, 0])  # total, matched

    for stem, cli_stem, linga, forms in sample:
        for row in forms:
            vib = _to_enum_case(row["vibhakti"])
            vac = _to_enum_number(row["vacana"])
            our = call_our_library(cli_stem, linga, vib, vac)
            oracle = row["form_slp1"]
            is_match = int(our == oracle)
            total += 1
            matched += is_match
            by_linga[linga][0] += 1
            by_linga[linga][1] += is_match
            rows.append({
                "stem": cli_stem,
                "linga": linga,
                "vibhakti": vib,
                "vacana": vac,
                "our_slp1": our or "",
                "oracle_slp1": oracle,
                "match": is_match,
            })

    if rows:
        with open(OUTPUT_TSV, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=rows[0].keys(), delimiter="\t")
            writer.writeheader()
            writer.writerows(rows)

    pct = (matched / total * 100.0) if total else 0.0
    print(f"Results: {matched}/{total} matched ({pct:.1f}%)")
    for linga, (ltotal, lmatched) in sorted(by_linga.items()):
        lpct = (lmatched / ltotal * 100.0) if ltotal else 0.0
        print(f"  {linga:10s}: {lmatched}/{ltotal} ({lpct:.1f}%)")
    print(f"Output: {OUTPUT_TSV}")
    return pct


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--stem")
    parser.add_argument("--validate", action="store_true")
    args = parser.parse_args()

    pct = run_comparison(filter_stem=args.stem)
    if args.validate and pct < TARGET:
        print(f"FAIL: Match rate {pct:.1f}% < {TARGET:.1f}%")
        sys.exit(1)
    if args.validate:
        print(f"PASS: Match rate {pct:.1f}% ≥ {TARGET:.1f}%")


if __name__ == "__main__":
    main()
