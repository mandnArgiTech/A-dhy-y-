#!/usr/bin/env python3
"""
coverage_report.py — Report which sūtras are exercised by the test suite.

Usage:
  python3 tools/coverage_report.py > tests/regression/sutra_coverage_report.txt

See STORY_6_3 for full specification.
"""

import csv, os, sys

SUTRAS_TSV = os.path.join(os.path.dirname(__file__), "../data/sutras.tsv")
OUTPUT_RPT = os.path.join(os.path.dirname(__file__),
                           "../tests/regression/sutra_coverage_report.txt")

# Vedic-only sūtras (approximate range — refine in Story 6.3)
VEDIC_RANGES = [
    (3655, 3747),  # Chhandas section
    (3748, 3810),  # Śākalya pāṭha
]

IMPLEMENTED_SUTRA_IDS = set()  # Populated during test runs


def is_vedic(global_id: int) -> bool:
    for lo, hi in VEDIC_RANGES:
        if lo <= global_id <= hi:
            return True
    return False


def load_sutras():
    if not os.path.exists(SUTRAS_TSV):
        print("ERROR: data/sutras.tsv not found. Run ingestion first.", file=sys.stderr)
        sys.exit(1)
    sutras = []
    with open(SUTRAS_TSV, encoding="utf-8") as f:
        reader = csv.DictReader(f, delimiter="\t")
        for row in reader:
            sutras.append({
                "global_id": int(row["global_id"]),
                "adhyaya": int(row["adhyaya"]),
                "pada": int(row["pada"]),
                "num": int(row["num"]),
                "sutra_type": row["sutra_type"],
            })
    return sutras


def generate_report():
    sutras = load_sutras()
    total = len(sutras)
    vedic = [s for s in sutras if is_vedic(s["global_id"])]
    classical = [s for s in sutras if not is_vedic(s["global_id"])]

    covered = [s for s in classical
               if s["global_id"] in IMPLEMENTED_SUTRA_IDS]

    lines = [
        "===== libAshtadhyayi Sūtra Coverage Report =====\n",
        f"Total sūtras in Aṣṭādhyāyī:  {total}",
        f"Vedic-only sūtras (excluded): {len(vedic)}",
        f"Target (Classical Sanskrit):  {len(classical)}",
        f"",
        f"Covered by test suite:        {len(covered)} ({len(covered)/len(classical)*100:.1f}%)",
        f"",
        f"Critical sūtras (Phase 1–3):",
    ]

    # List a sample of critical sūtras
    critical = [
        (1, 1, 1, "vṛddhirādaic — Vṛddhi definition"),
        (1, 1, 2, "adeṅ guṇaḥ — Guṇa definition"),
        (3, 1, 68, "kartari śap — Class 1 vikaraṇa"),
        (6, 1, 77, "iko yaṇ aci — yaṆ sandhi"),
        (6, 1, 101, "akaḥ savarṇe dīrghaḥ — savarṇadīrgha"),
        (7, 3, 84, "sārvadhātukārdhadhātukayoḥ — guṇa"),
        (8, 3, 23, "mo'nusvāraḥ — anusvāra"),
        (8, 4, 53, "jhalopajhaś jashi — voicing assimilation"),
    ]

    for a, p, n, desc in critical:
        status = "✓ COVERED" if any(
            s["adhyaya"] == a and s["pada"] == p and s["num"] == n
            and s["global_id"] in IMPLEMENTED_SUTRA_IDS
            for s in sutras
        ) else "  (not yet covered by test suite)"
        lines.append(f"  {a}.{p}.{n:3d}  {desc}  {status}")

    report = "\n".join(lines)
    os.makedirs(os.path.dirname(OUTPUT_RPT), exist_ok=True)
    with open(OUTPUT_RPT, "w", encoding="utf-8") as f:
        f.write(report + "\n")
    print(report)


if __name__ == "__main__":
    generate_report()
