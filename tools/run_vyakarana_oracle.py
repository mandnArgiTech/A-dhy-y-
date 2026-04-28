#!/usr/bin/env python3
"""Strict, informational tinanta oracle comparison against dhatuforms.tsv."""

from __future__ import annotations

import argparse
import csv
import os
import subprocess
import sys
import unicodedata
from collections import defaultdict
from typing import Dict, Iterable, List, Optional, Tuple

from devanagari_slp1 import slp1_to_devanagari

ROOT = os.path.dirname(__file__)
OUTPUT_TSV = os.path.join(ROOT, "../tests/regression/tinanta_oracle_results.tsv")
DATA_DHATUFORMS = os.path.join(ROOT, "../data/dhatuforms.tsv")
DATA_DHATUPATHA = os.path.join(ROOT, "../data/dhatupatha.tsv")
DEMO_BIN = os.path.join(ROOT, "../build/ash_demo")
DEFAULT_SAMPLE_SIZE = 450

PADA_MAP = {"P": "PARASMAI", "A": "ATMANE"}
PURUSHA_MAP = {"PRATHAMA": "PRATHAMA", "MADHYAMA": "MADHYAMA", "UTTAMA": "UTTAMA"}
VACANA_MAP = {"EKA": "EKAVACANA", "DVI": "DVIVACANA", "BAHU": "BAHUVACANA"}


def nfc(text: str) -> str:
    return unicodedata.normalize("NFC", text or "")


def load_dhatu_map() -> Dict[Tuple[str, str], str]:
    mapping: Dict[Tuple[str, str], str] = {}
    with open(DATA_DHATUPATHA, encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            mapping[(row["gana"].strip(), row["serial_in_gana"].strip())] = row["upadesa_slp1"].strip()
    return mapping


def call_our_library(root_slp1: str, gana: str, purusha: str, vacana: str, pada: str) -> Tuple[str, str]:
    if not os.path.exists(DEMO_BIN):
        return "ERROR:missing-demo", ""
    cmd = [DEMO_BIN, "tinanta", root_slp1, gana, "LAT", purusha, vacana, pada]
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=10, check=False)
    if proc.returncode != 0:
        err = (proc.stderr or proc.stdout).strip()
        return (f"ERROR:{err}" if err else f"ERROR:exit-{proc.returncode}"), ""
    for line in proc.stdout.splitlines():
        line = line.strip()
        if not line or line.startswith("libAshtadhyayi") or line.startswith("─"):
            continue
        return line, nfc(slp1_to_devanagari(line))
    return "ERROR:empty-output", ""


def load_lat_rows(filter_root: Optional[str]) -> List[dict]:
    dh_map = load_dhatu_map()
    rows: List[dict] = []
    with open(DATA_DHATUFORMS, encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            if row["lakara"] != "LAT" or row["pada"].strip() != "P":
                continue
            key = (row["gana"].strip(), row["serial"].strip())
            root = dh_map.get(key)
            if not root or (filter_root and root != filter_root):
                continue
            rows.append({
                "root": root,
                "gana": row["gana"].strip(),
                "purusha": row["purusha"].strip(),
                "vacana": row["vacana"].strip(),
                "pada": row["pada"].strip(),
                "oracle_deva": nfc(row["form_deva"].strip()),
                "oracle_slp1": row["form_slp1"].strip(),
            })
    return rows


def sample_rows(rows: List[dict], sample_size: int) -> List[dict]:
    grouped: Dict[Tuple[str, str], List[dict]] = defaultdict(list)
    for row in rows:
        grouped[(row["gana"], row["root"])].append(row)
    selected_roots: List[Tuple[str, str]] = []
    quotas = [("1", 12), ("2", 8), ("4", 8), ("6", 8), ("10", 8)]
    for gana, quota in quotas:
        roots = sorted(k for k in grouped if k[0] == gana)
        selected_roots.extend(roots[:quota])
    for key in sorted(grouped):
        if key not in selected_roots:
            selected_roots.append(key)
        if len(selected_roots) * 9 >= sample_size:
            break
    sampled: List[dict] = []
    for key in selected_roots:
        sampled.extend(sorted(grouped[key], key=lambda r: (r["purusha"], r["vacana"])))
        if len(sampled) >= sample_size:
            break
    return sampled[:sample_size]


def pct(pair: List[int]) -> float:
    total, matched = pair
    return (matched / total * 100.0) if total else 0.0


def print_breakdown(title: str, stats: Dict[str, List[int]]) -> None:
    print(title)
    for key in sorted(stats):
        total, matched = stats[key]
        print(f"  {key:12s}: {matched}/{total} ({pct(stats[key]):.2f}%)")


def run_comparison(filter_root: Optional[str], sample_size: int, require_rate: Optional[float]) -> float:
    os.makedirs(os.path.dirname(OUTPUT_TSV), exist_ok=True)
    rows = sample_rows(load_lat_rows(filter_root), sample_size)
    results = []
    total = matched = errors = 0
    by_gana: Dict[str, List[int]] = defaultdict(lambda: [0, 0])
    by_vacana: Dict[str, List[int]] = defaultdict(lambda: [0, 0])
    mismatches: List[dict] = []

    for row in rows:
        purusha = PURUSHA_MAP.get(row["purusha"])
        vacana = VACANA_MAP.get(row["vacana"])
        pada = PADA_MAP.get(row["pada"])
        if not purusha or not vacana or not pada:
            continue
        ours_slp1, ours_deva = call_our_library(row["root"], row["gana"], purusha, vacana, pada)
        is_error = ours_slp1.startswith("ERROR:")
        is_match = int((not is_error) and nfc(ours_deva) == row["oracle_deva"])
        total += 1
        matched += is_match
        errors += int(is_error)
        by_gana[row["gana"]][0] += 1
        by_gana[row["gana"]][1] += is_match
        by_vacana[row["vacana"]][0] += 1
        by_vacana[row["vacana"]][1] += is_match
        result = {
            "root": row["root"], "gana": row["gana"], "lakara": "LAT",
            "purusha": row["purusha"], "vacana": row["vacana"], "pada": row["pada"],
            "our_slp1": ours_slp1, "our_deva": ours_deva,
            "oracle_slp1": row["oracle_slp1"], "oracle_deva": row["oracle_deva"],
            "match": is_match, "note": "engine error" if is_error else "",
        }
        results.append(result)
        if not is_match:
            mismatches.append(result)

    if results:
        with open(OUTPUT_TSV, "w", newline="", encoding="utf-8") as f:
            writer = csv.DictWriter(f, fieldnames=list(results[0].keys()), delimiter="\t")
            writer.writeheader(); writer.writerows(results)

    rate = (matched / total * 100.0) if total else 0.0
    roots = len({(r["gana"], r["root"]) for r in rows})
    print(f"# of {roots} roots x LAT parasmai forms; compared: {total}; matched: {matched}; errors: {errors}; rate: {rate:.2f}%")
    print_breakdown("Per-gana rate:", by_gana)
    print_breakdown("Per-vacana rate:", by_vacana)
    print("Top mismatches:")
    for row in mismatches[:20]:
        print(f"  {row['root']} g{row['gana']} {row['purusha']}-{row['vacana']}: ours={row['our_deva'] or row['our_slp1']} oracle={row['oracle_deva']}")
    print(f"Output: {OUTPUT_TSV}")
    if require_rate is not None and rate < require_rate:
        print(f"FAIL: match rate {rate:.2f}% < {require_rate:.2f}%")
        sys.exit(1)
    return rate


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--root", help="Filter by root SLP1")
    parser.add_argument("--sample-size", type=int, default=DEFAULT_SAMPLE_SIZE)
    parser.add_argument("--require-rate", type=float, default=None)
    parser.add_argument("--validate", action="store_true", help="Informational validation mode")
    args = parser.parse_args()
    run_comparison(args.root, args.sample_size, args.require_rate)


if __name__ == "__main__":
    main()
