#!/usr/bin/env python3
"""Informational prakriya trace oracle comparison using ash_demo --trace."""

from __future__ import annotations

import argparse
import csv
import os
import random
import re
import subprocess
from collections import defaultdict
from typing import Dict, Iterable, List, Set, Tuple

ROOT = os.path.dirname(__file__)
ORACLE_TSV = os.path.join(ROOT, "../data/shabdaprakriya.tsv")
SHABDA_TSV = os.path.join(ROOT, "../data/shabda_forms.tsv")
DEMO_BIN = os.path.join(ROOT, "../build/ash_demo")
SUMMARY = os.path.join(ROOT, "../tests/regression/prakriya_oracle_summary.tsv")
RNG_SEED = 42

VIB_MAP = {
    "1": "PRATHAMA", "2": "DVITIYA", "3": "TRITIYA", "4": "CATURTHI",
    "5": "PANCAMI", "6": "SHASTHI", "7": "SAPTAMI", "8": "SAMBODHANA",
}
VAC_MAP = {"1": "EKAVACANA", "2": "DVIVACANA", "3": "BAHUVACANA"}
LINGA_PRIORITY = ["PUMS", "STRI", "NAPUMSAKA"]


def sutra_addr_to_id(addr: str) -> int:
    nums = re.findall(r"\d+", addr or "")
    if len(nums) < 3:
        return 0
    a, p, n = (int(nums[0]), int(nums[1]), int(nums[2]))
    return a * 100000 + p * 1000 + n


def load_oracle_groups() -> Dict[Tuple[str, str, str, str], Set[int]]:
    groups: Dict[Tuple[str, str, str, str], Set[int]] = defaultdict(set)
    with open(ORACLE_TSV, encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            key = (row["stem_deva"], row["form_deva"], row["vibhakti"], row["vacana"])
            for sutra in (row["sutras"] or "").split(','):
                sid = sutra_addr_to_id(sutra.strip())
                if sid:
                    groups[key].add(sid)
    return groups


def load_form_index() -> Dict[Tuple[str, str, str], List[dict]]:
    index: Dict[Tuple[str, str, str], List[dict]] = defaultdict(list)
    with open(SHABDA_TSV, encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            index[(row["stem_deva"], row["vibhakti"], row["vacana"])].append(row)
    return index


def choose_samples(groups: Dict[Tuple[str, str, str, str], Set[int]], sample_size: int) -> List[Tuple[str, str, str, str]]:
    keys = sorted(groups)
    rng = random.Random(RNG_SEED)
    rng.shuffle(keys)
    return keys[:sample_size]


def cli_trace_ids(row: dict) -> Tuple[str, Set[int]]:
    if not os.path.exists(DEMO_BIN):
        return "ERROR:missing-demo", set()
    cmd = [
        DEMO_BIN, "subanta", row["stem_slp1"], row["linga"],
        VIB_MAP[row["vibhakti_num"]], VAC_MAP[row["vacana_num"]], "--trace",
    ]
    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=10, check=False)
    if proc.returncode != 0:
        return f"ERROR:{(proc.stderr or proc.stdout).strip()}", set()
    ids: Set[int] = set()
    first = ""
    for line in proc.stdout.splitlines():
        text = line.strip()
        if text and not text.startswith("[") and not first:
            first = text
        m = re.match(r"\[(\d+)\]", text)
        if m:
            ids.add(int(m.group(1)))
    return first, ids


def find_cli_row(index: Dict[Tuple[str, str, str], List[dict]], key: Tuple[str, str, str, str]) -> dict | None:
    stem_deva, _form_deva, vib_num, vac_num = key
    vib_name = {
        "1": "prathama", "2": "dvitiya", "3": "tritiya", "4": "caturthi",
        "5": "pancami", "6": "shasthi", "7": "saptami", "8": "sambodhana",
    }.get(vib_num)
    vac_name = {"1": "ekavacana", "2": "dvivacana", "3": "bahuvacana"}.get(vac_num)
    rows = index.get((stem_deva, vib_name or "", vac_name or ""), [])
    for linga in LINGA_PRIORITY:
        for row in rows:
            if row["linga"] == linga:
                row = dict(row)
                row["vibhakti_num"] = vib_num
                row["vacana_num"] = vac_num
                return row
    return None


def run(sample_size: int) -> None:
    os.makedirs(os.path.dirname(SUMMARY), exist_ok=True)
    groups = load_oracle_groups()
    index = load_form_index()
    keys = choose_samples(groups, sample_size)
    rows_out = []
    any_total = any_matched = ordered_total = ordered_matched = 0

    for key in keys:
        oracle_ids = groups[key]
        row = find_cli_row(index, key)
        if not row:
            ours_form, our_ids = "ERROR:no-cli-row", set()
        else:
            ours_form, our_ids = cli_trace_ids(row)
        overlap = oracle_ids & our_ids
        any_match = int(bool(overlap))
        ordered_match = int(bool(oracle_ids) and oracle_ids.issubset(our_ids))
        any_total += 1; any_matched += any_match
        ordered_total += 1; ordered_matched += ordered_match
        rows_out.append({
            "stem_deva": key[0], "form_deva": key[1], "vibhakti": key[2], "vacana": key[3],
            "oracle_sutra_ids": ",".join(str(x) for x in sorted(oracle_ids)),
            "our_form_or_error": ours_form,
            "our_sutra_ids": ",".join(str(x) for x in sorted(our_ids)),
            "any_match": any_match,
            "ordered_match": ordered_match,
        })

    with open(SUMMARY, "w", encoding="utf-8", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows_out[0].keys()), delimiter="\t")
        writer.writeheader(); writer.writerows(rows_out)
    any_rate = any_matched / any_total * 100.0 if any_total else 0.0
    ordered_rate = ordered_matched / ordered_total * 100.0 if ordered_total else 0.0
    print(f"Prakriya oracle sampled forms: {len(keys)}")
    print(f"Any-match rate: {any_matched}/{any_total} ({any_rate:.2f}%)")
    print(f"Ordered/all-sutra match rate: {ordered_matched}/{ordered_total} ({ordered_rate:.2f}%)")
    print(f"Output: {SUMMARY}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--sample-size", type=int, default=100)
    parser.add_argument("--validate", action="store_true", help="Informational validation mode")
    args = parser.parse_args()
    run(args.sample_size)


if __name__ == "__main__":
    main()
