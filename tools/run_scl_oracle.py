#!/usr/bin/env python3
"""Strict, informational subanta oracle comparison against shabda_forms.tsv."""

from __future__ import annotations

import argparse
import csv
import os
import random
import subprocess
import sys
import unicodedata
from collections import defaultdict
from typing import Dict, List, Optional, Tuple

from devanagari_slp1 import slp1_to_devanagari

ROOT = os.path.dirname(__file__)
OUTPUT_TSV = os.path.join(ROOT, "../tests/regression/subanta_oracle_results.tsv")
SHABDA_TSV = os.path.join(ROOT, "../data/shabda_forms.tsv")
DEMO_BIN = os.path.join(ROOT, "../build/ash_demo")
DEFAULT_SAMPLE_SIZE = 1200
RNG_SEED = 42


def nfc(text: str) -> str:
    return unicodedata.normalize("NFC", text or "")


def normalize_compare(text: str) -> str:
    """Normalize an oracle/our form for comparison.

    The shabda_forms.tsv oracle prefixes sambodhana entries with the
    vocative particle "हे " ("he"), which is presentation, not part of
    the morphological form. Strip it.
    For pancami-eka the oracle records both "रामाद्" and "रामात्" joined
    by "-"; treat any of the alternatives as equivalent so we match the
    standard form.
    """
    s = nfc(text)
    if s.startswith("हे "):
        s = s[len("हे "):]
    if "-" in s:
        return s
    return s


def alt_forms(text: str) -> list:
    s = nfc(text)
    if s.startswith("हे "):
        s = s[len("हे "):]
    parts = [s]
    if "-" in s:
        parts = [p.strip() for p in s.split("-") if p.strip()]
    # Strip the "हे " sambodhana prefix from each alternative as well
    # (some oracle rows have it on every alt: "हे X-हे Y").
    return [p[len("हे "):] if p.startswith("हे ") else p for p in parts]


def _to_enum_case(vibhakti: str) -> str:
    return {
        "prathama": "PRATHAMA", "dvitiya": "DVITIYA", "tritiya": "TRITIYA",
        "caturthi": "CATURTHI", "pancami": "PANCAMI", "shasthi": "SHASTHI",
        "saptami": "SAPTAMI", "sambodhana": "SAMBODHANA",
    }[vibhakti]


def _to_enum_number(vacana: str) -> str:
    return {"ekavacana": "EKAVACANA", "dvivacana": "DVIVACANA", "bahuvacana": "BAHUVACANA"}[vacana]


def stem_class(stem: str) -> str:
    if stem.endswith("A"):
        return "aa-stem"
    if stem.endswith("i") or stem.endswith("I"):
        return "i-stem"
    if stem.endswith("u") or stem.endswith("U"):
        return "u-stem"
    if stem.endswith("a"):
        return "a-stem"
    return "consonant-stem"


def call_our_library(stem: str, linga: str, vibhakti: str, vacana: str) -> Tuple[str, str]:
    if not os.path.exists(DEMO_BIN):
        return "ERROR:missing-demo", ""
    result = subprocess.run(
        [DEMO_BIN, "subanta", stem, linga, vibhakti, vacana],
        capture_output=True, text=True, timeout=10, check=False,
    )
    if result.returncode != 0:
        err = (result.stderr or result.stdout).strip()
        return (f"ERROR:{err}" if err else f"ERROR:exit-{result.returncode}"), ""
    for line in result.stdout.splitlines():
        line = line.strip()
        if not line or line.startswith("libAshtadhyayi") or line.startswith("─"):
            continue
        return line, nfc(slp1_to_devanagari(line))
    return "ERROR:empty-output", ""


def load_sample(filter_stem: Optional[str], sample_size: int) -> List[dict]:
    by_stem: Dict[Tuple[str, str], List[dict]] = defaultdict(list)
    rng = random.Random(RNG_SEED)
    with open(SHABDA_TSV, encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            stem = row["stem_slp1"].strip()
            linga = row["linga"].strip()
            if linga == "ALL" or (filter_stem and stem != filter_stem):
                continue
            by_stem[(stem, linga)].append(row)
    buckets: Dict[Tuple[str, str], List[Tuple[str, str]]] = defaultdict(list)
    for key in sorted(by_stem):
        if len(by_stem[key]) >= 24:
            buckets[(key[1], stem_class(key[0]))].append(key)
    for keys in buckets.values():
        rng.shuffle(keys)

    target_stems = max(1, sample_size // 24)
    selected_keys: List[Tuple[str, str]] = []
    bucket_keys = sorted(buckets)
    while len(selected_keys) < target_stems and bucket_keys:
        progressed = False
        for bucket in bucket_keys:
            if buckets[bucket]:
                selected_keys.append(buckets[bucket].pop(0))
                progressed = True
                if len(selected_keys) >= target_stems:
                    break
        if not progressed:
            break

    selected: List[dict] = []
    for key in selected_keys:
        selected.extend(sorted(by_stem[key], key=lambda r: (r["vibhakti"], r["vacana"])))
    return selected


def pct(pair: List[int]) -> float:
    total, matched = pair
    return (matched / total * 100.0) if total else 0.0


def print_breakdown(title: str, stats: Dict[str, List[int]]) -> None:
    print(title)
    for key in sorted(stats):
        total, matched = stats[key]
        print(f"  {key:16s}: {matched}/{total} ({pct(stats[key]):.2f}%)")


def run_comparison(filter_stem: Optional[str], sample_size: int, require_rate: Optional[float]) -> float:
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    sample = load_sample(filter_stem, sample_size)
    total = matched = errors = 0
    rows = []
    mismatches = []
    by_linga: Dict[str, List[int]] = defaultdict(lambda: [0, 0])
    by_vibhakti: Dict[str, List[int]] = defaultdict(lambda: [0, 0])
    by_vacana: Dict[str, List[int]] = defaultdict(lambda: [0, 0])
    by_class: Dict[str, List[int]] = defaultdict(lambda: [0, 0])

    for row in sample:
        vib = _to_enum_case(row["vibhakti"])
        vac = _to_enum_number(row["vacana"])
        ours_slp1, ours_deva = call_our_library(row["stem_slp1"], row["linga"], vib, vac)
        is_error = ours_slp1.startswith("ERROR:")
        ours_norm = normalize_compare(ours_deva)
        oracle_alts = alt_forms(row["form_deva"])
        # Skip empty-oracle rows; the dataset uses empty cells for
        # paradigm slots that were not validated upstream.
        if not row["form_deva"].strip():
            continue
        is_match = int((not is_error) and ours_norm in oracle_alts)
        total += 1; matched += is_match; errors += int(is_error)
        klass = stem_class(row["stem_slp1"])
        for stats, key in [(by_linga, row["linga"]), (by_vibhakti, row["vibhakti"]), (by_vacana, row["vacana"]), (by_class, klass)]:
            stats[key][0] += 1; stats[key][1] += is_match
        result = {
            "stem": row["stem_slp1"], "linga": row["linga"], "stem_class": klass,
            "vibhakti": vib, "vacana": vac,
            "our_slp1": ours_slp1, "our_deva": ours_deva,
            "oracle_slp1": row["form_slp1"], "oracle_deva": row["form_deva"],
            "match": is_match,
        }
        rows.append(result)
        if not is_match:
            mismatches.append(result)

    if rows:
        with open(OUTPUT_TSV, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=rows[0].keys(), delimiter="\t")
            writer.writeheader(); writer.writerows(rows)

    rate = (matched / total * 100.0) if total else 0.0
    stems = len({(r["stem_slp1"], r["linga"]) for r in sample})
    print(f"# of {stems} stems; compared: {total}; matched: {matched}; errors: {errors}; rate: {rate:.2f}%")
    print_breakdown("Per-linga rate:", by_linga)
    print_breakdown("Per-vibhakti rate:", by_vibhakti)
    print_breakdown("Per-vacana rate:", by_vacana)
    print_breakdown("Per-stem-class rate:", by_class)
    print("Top mismatches:")
    for row in mismatches[:20]:
        print(f"  {row['stem']} {row['linga']} {row['vibhakti']}-{row['vacana']}: ours={row['our_deva'] or row['our_slp1']} oracle={row['oracle_deva']}")
    print(f"Output: {OUTPUT_TSV}")
    if require_rate is not None and rate < require_rate:
        print(f"FAIL: match rate {rate:.2f}% < {require_rate:.2f}%")
        sys.exit(1)
    return rate


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--stem")
    parser.add_argument("--sample-size", type=int, default=DEFAULT_SAMPLE_SIZE)
    parser.add_argument("--require-rate", type=float, default=None)
    parser.add_argument("--validate", action="store_true", help="Informational validation mode")
    args = parser.parse_args()
    run_comparison(args.stem, args.sample_size, args.require_rate)


if __name__ == "__main__":
    main()
