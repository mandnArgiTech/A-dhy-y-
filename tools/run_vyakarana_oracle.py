#!/usr/bin/env python3
"""Compare tinanta results against dhatuforms.tsv oracle."""

from __future__ import annotations

import argparse
import csv
import os
import subprocess
import sys
from typing import Dict, List, Optional, Tuple


ROOT = os.path.dirname(__file__)
OUTPUT_TSV = os.path.join(ROOT, "../tests/regression/tinanta_oracle_results.tsv")
DATA_DHATUFORMS = os.path.join(ROOT, "../data/dhatuforms.tsv")
DATA_DHATUPATHA = os.path.join(ROOT, "../data/dhatupatha.tsv")
DEMO_BIN = os.path.join(ROOT, "../build/ash_demo")
MAX_VALIDATE_ROWS = 1000

PADA_MAP = {"P": "PARASMAI", "A": "ATMANE"}
PURUSHA_MAP = {"PRATHAMA": "PRATHAMA", "MADHYAMA": "MADHYAMA", "UTTAMA": "UTTAMA"}
VACANA_MAP = {"EKA": "EKAVACANA", "DVI": "DVIVACANA", "BAHU": "BAHUVACANA"}


def load_dhatu_map() -> Dict[Tuple[str, str], str]:
    """Map (gana, serial) -> upadesa_slp1 root."""
    mapping: Dict[Tuple[str, str], str] = {}
    with open(DATA_DHATUPATHA, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            key = (row["gana"].strip(), row["serial_in_gana"].strip())
            mapping[key] = row["upadesa_slp1"].strip()
    return mapping


def call_our_library(root_slp1: str, gana: str, purusha: str, vacana: str, pada: str) -> str:
    """Call CLI tinanta mode and parse first non-empty line as SLP1."""
    if not os.path.exists(DEMO_BIN):
        return "ERROR:missing-demo"
    cmd = [
        DEMO_BIN,
        "tinanta",
        root_slp1,
        gana,
        "LAT",
        purusha,
        vacana,
        pada,
    ]
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=10, check=False)
    if proc.returncode != 0:
        err = (proc.stderr or proc.stdout).strip()
        return f"ERROR:{err}" if err else f"ERROR:exit-{proc.returncode}"
    for line in proc.stdout.splitlines():
        line = line.strip()
        if not line or line.startswith("libAshtadhyayi") or line.startswith("─"):
            continue
        return line
    return "ERROR:empty-output"


def iter_oracle_rows(filter_root: Optional[str]) -> List[dict]:
    """Collect LAT rows with resolved root_slp1 from oracle files."""
    dh_map = load_dhatu_map()
    rows: List[dict] = []
    with open(DATA_DHATUFORMS, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            if row["lakara"] != "LAT":
                continue
            key = (row["gana"].strip(), row["serial"].strip())
            root = dh_map.get(key)
            if not root:
                continue
            if filter_root and root != filter_root:
                continue
            rows.append(
                {
                    "root": root,
                    "gana": row["gana"].strip(),
                    "pada": row["pada"].strip(),
                    "purusha": row["purusha"].strip(),
                    "vacana": row["vacana"].strip(),
                    "oracle_slp1": row["form_slp1"].strip(),
                }
            )
    return rows


def run_comparison(filter_root: Optional[str], limit: Optional[int]) -> float:
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    rows = iter_oracle_rows(filter_root)
    if limit is not None:
        rows = rows[:limit]

    results = []
    matched = 0
    total = 0
    errors = 0

    for row in rows:
        pada = PADA_MAP.get(row["pada"])
        purusha = PURUSHA_MAP.get(row["purusha"])
        vacana = VACANA_MAP.get(row["vacana"])
        if not pada or not purusha or not vacana:
            continue
        ours = call_our_library(row["root"], row["gana"], purusha, vacana, pada)
        oracle = row["oracle_slp1"]
        is_error = ours.startswith("ERROR:")
        is_match = int((not is_error) and ours == oracle)
        total += 1
        matched += is_match
        errors += 1 if is_error else 0
        results.append(
            {
                "root": row["root"],
                "gana": row["gana"],
                "lakara": "LAT",
                "purusha": row["purusha"],
                "vacana": row["vacana"],
                "pada": row["pada"],
                "our_slp1": ours,
                "oracle_slp1": oracle,
                "match": is_match,
                "note": "engine error" if is_error else "",
            }
        )

    if results:
        with open(OUTPUT_TSV, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=list(results[0].keys()), delimiter="\t")
            writer.writeheader()
            writer.writerows(results)

    pct = (matched / total * 100.0) if total else 0.0
    print(f"Rows compared: {total}, matched: {matched}, errors: {errors}, rate: {pct:.2f}%")
    print(f"Output: {OUTPUT_TSV}")
    return pct


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", help="Filter by root SLP1")
    parser.add_argument("--limit", type=int, default=None, help="Limit row count")
    parser.add_argument("--validate", action="store_true",
                        help="CI mode; enforces minimum threshold")
    args = parser.parse_args()

    limit = args.limit
    if args.validate and limit is None and not args.root:
        limit = MAX_VALIDATE_ROWS

    pct = run_comparison(filter_root=args.root, limit=limit)

    if args.validate:
        threshold = 90.0
        if pct < threshold:
            print(f"FAIL: match rate {pct:.2f}% < {threshold:.1f}%")
            sys.exit(1)
        print(f"PASS: match rate {pct:.2f}% >= {threshold:.1f}%")


if __name__ == "__main__":
    main()
