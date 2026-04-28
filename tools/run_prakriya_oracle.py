#!/usr/bin/env python3
"""Informational prakriya trace oracle sampler.

The current demo CLI exposes final forms but not serialized ASH_Form.steps. This
script still samples the bundled trace oracle and records the forms that are ready
for trace comparison, without pretending to have a passing trace rate.
"""

from __future__ import annotations

import argparse
import csv
import os
from collections import defaultdict
from typing import Dict, List, Tuple

ROOT = os.path.dirname(__file__)
ORACLE_TSV = os.path.join(ROOT, "../data/shabdaprakriya.tsv")
SUMMARY = os.path.join(ROOT, "../tests/regression/prakriya_oracle_summary.tsv")


def load_sample(sample_size: int) -> Dict[Tuple[str, str, str, str], List[str]]:
    groups: Dict[Tuple[str, str, str, str], List[str]] = defaultdict(list)
    with open(ORACLE_TSV, encoding="utf-8") as f:
        for row in csv.DictReader(f, delimiter="\t"):
            key = (row["stem_deva"], row["form_deva"], row["vibhakti"], row["vacana"])
            if len(groups) < sample_size or key in groups:
                groups[key].append(row["sutras"])
    return dict(list(groups.items())[:sample_size])


def run(sample_size: int) -> None:
    os.makedirs(os.path.dirname(SUMMARY), exist_ok=True)
    sample = load_sample(sample_size)
    with open(SUMMARY, "w", encoding="utf-8", newline="") as f:
        writer = csv.writer(f, delimiter="\t")
        writer.writerow(["stem_deva", "form_deva", "vibhakti", "vacana", "oracle_steps", "trace_status"])
        for (stem, form, vib, vac), sutras in sample.items():
            writer.writerow([stem, form, vib, vac, len(sutras), "pending_cli_trace_export"])
    print(f"Prakriya oracle sampled forms: {len(sample)}")
    print("Trace comparison rate: 0.00% (CLI trace export pending; informational only)")
    print(f"Output: {SUMMARY}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--sample-size", type=int, default=100)
    parser.add_argument("--validate", action="store_true", help="Informational validation mode")
    args = parser.parse_args()
    run(args.sample_size)


if __name__ == "__main__":
    main()
