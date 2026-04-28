#!/usr/bin/env python3
"""
coverage_report.py — compute weighted line coverage from gcov output.

This script expects to be run from a build directory after tests have run.
It scans for *.gcno files, runs gcov, filters project C sources, and reports
weighted line coverage.

By default this script enforces a 95% threshold. Override with:
  ASH_COVERAGE_TARGET=90 python3 tools/coverage_report.py
"""

from __future__ import annotations

import os
import re
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
OUTPUT_RPT = REPO_ROOT / "tests" / "regression" / "sutra_coverage_report.txt"
DEFAULT_TARGET = 95.0


@dataclass
class CoverageRow:
    path: str
    percent: float
    lines: int

    @property
    def uncovered(self) -> float:
        return self.lines * (100.0 - self.percent) / 100.0


def discover_gcno_files(build_dir: Path) -> list[str]:
    return [str(p) for p in build_dir.rglob("*.gcno")]


def run_gcov(build_dir: Path, gcno_files: list[str]) -> str:
    # Keep command line size bounded by chunking.
    chunks: list[list[str]] = []
    step = 200
    for i in range(0, len(gcno_files), step):
        chunks.append(gcno_files[i : i + step])

    output_parts: list[str] = []
    for chunk in chunks:
        proc = subprocess.run(
            ["gcov", *chunk],
            cwd=build_dir,
            text=True,
            capture_output=True,
            check=False,
        )
        output_parts.append(proc.stdout)
    return "".join(output_parts)


def parse_gcov_summary(text: str) -> list[CoverageRow]:
    pattern = re.compile(r"File '([^']+)'\nLines executed:([0-9]+\.[0-9]+)% of ([0-9]+)")
    rows: list[CoverageRow] = []
    for path, pct, lines in pattern.findall(text):
        rows.append(CoverageRow(path=path, percent=float(pct), lines=int(lines)))
    return rows


def is_project_source(path: str) -> bool:
    if not path.endswith(".c"):
        return False
    if "/tests/" in path:
        return False
    if "/examples/" in path:
        return False
    if "unity.c" in path:
        return False
    if "CMakeCCompilerId" in path:
        return False
    return True


def weighted_coverage(rows: list[CoverageRow]) -> float:
    total = sum(r.lines for r in rows)
    if total == 0:
        return 0.0
    covered = sum((r.percent * r.lines) / 100.0 for r in rows)
    return (covered / total) * 100.0


def build_report(rows: list[CoverageRow], target: float) -> str:
    cov = weighted_coverage(rows)
    total_lines = sum(r.lines for r in rows)
    status = "PASS" if cov >= target else "FAIL"
    top_uncovered = sorted(rows, key=lambda r: r.uncovered, reverse=True)[:20]

    lines = [
        "===== libAshtadhyayi Line Coverage Report =====",
        "",
        f"Project C source files:        {len(rows)}",
        f"Project executable lines:      {total_lines}",
        f"Weighted line coverage:        {cov:.2f}%",
        f"Target threshold:              {target:.2f}%",
        f"Result:                        {status}",
        "",
        "Top uncovered files:",
    ]
    for row in top_uncovered:
        lines.append(
            f"  {row.path}  --  {row.percent:.2f}% of {row.lines} "
            f"(uncovered {row.uncovered:.1f})"
        )
    return "\n".join(lines) + "\n"


def main() -> int:
    build_dir = Path.cwd()
    target = float(os.environ.get("ASH_COVERAGE_TARGET", DEFAULT_TARGET))

    gcno_files = discover_gcno_files(build_dir)
    if not gcno_files:
        report = (
            "===== libAshtadhyayi Line Coverage Report =====\n\n"
            "No *.gcno files found in this build directory.\n"
            "Build with coverage flags (e.g. -fprofile-arcs -ftest-coverage) and rerun tests.\n"
        )
        OUTPUT_RPT.parent.mkdir(parents=True, exist_ok=True)
        OUTPUT_RPT.write_text(report, encoding="utf-8")
        print(report, end="")
        return 0

    gcov_text = run_gcov(build_dir, gcno_files)
    rows = parse_gcov_summary(gcov_text)
    project_rows = [r for r in rows if is_project_source(r.path)]
    report = build_report(project_rows, target)

    OUTPUT_RPT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT_RPT.write_text(report, encoding="utf-8")
    print(report, end="")

    return 0 if weighted_coverage(project_rows) >= target else 1


if __name__ == "__main__":
    sys.exit(main())
