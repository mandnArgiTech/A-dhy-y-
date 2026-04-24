# Story 6.1 — Tinanta Regression: 1000-Form Oracle Comparison

**Phase:** 6 — Validation Harness & API Polish  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 5.1 (pipeline complete)

---

## Objective

Run the complete tiṅanta engine against the `vyakarana` Python oracle across 1,000 verb forms (50 roots × 10 lakaras × 2 forms) and document all discrepancies.

---

## Background

The `vyakarana` project (github.com/sanskrit/vyakarana) implements a Pāṇinian simulator in Python. It is the primary external oracle for tiṅanta validation. Our goal is ≥ 90% match rate.

---

## Setup

```bash
pip install vyakarana  # or clone and install locally
# Reference: https://github.com/sanskrit/vyakarana
```

---

## Files to Create

### `tools/run_vyakarana_oracle.py`

```python
#!/usr/bin/env python3
"""
run_vyakarana_oracle.py — Compare libAshtadhyayi output against vyakarana oracle.

Usage:
  python3 tools/run_vyakarana_oracle.py                  # full run
  python3 tools/run_vyakarana_oracle.py --root BU        # single root
  python3 tools/run_vyakarana_oracle.py --validate       # CI mode (exit 1 if < 90%)
"""

import subprocess, json, sys, csv

# Test matrix: 50 roots × (prathama eka parasmai only for quick smoke test)
TEST_ROOTS = [
  ("BU", 1, "bhū"),
  ("gam", 1, "gam"),
  ("pat", 1, "pat"),
  ("kf", 8, "kṛ"),
  ("dA", 3, "dā"),
  ("ji", 1, "ji"),
  ("nI", 1, "nī"),
  ("liq", 4, "liś"),  # actually: liS
  ("diz", 6, "diś"),
  ("cur", 10, "cur"),
  # ... 40 more
]

FORMS_TO_TEST = [
  ("LAT",  "PRATHAMA", "EKAVACANA",  "PARASMAI"),
  ("LAT",  "PRATHAMA", "BAHUVACANA", "PARASMAI"),
  ("LAN",  "PRATHAMA", "EKAVACANA",  "PARASMAI"),
  ("LOT",  "PRATHAMA", "EKAVACANA",  "PARASMAI"),
  ("VIDHILIM", "PRATHAMA", "EKAVACANA", "PARASMAI"),
  # ... more
]
```

The script must:
1. For each (root, gaṇa, forms_to_test):
   a. Call `./ash_demo tinanta <root> <gana> <lakara> ...` (or use the C library via cffi)
   b. Call vyakarana Python API
   c. Compare outputs
2. Write results to `tests/regression/tinanta_oracle_results.tsv`
3. Print summary: N matched / M total (percentage)
4. In `--validate` mode: exit 1 if match rate < 90%

### `tests/regression/tinanta_oracle_results.tsv`

Output format:
```
root  gana  lakara  purusha  vacana  pada  our_slp1  oracle_slp1  match  note
BU    1     LAT     PRAT     EKA     PAR   Bavati    bhavati      1      (IAST mismatch — encoding)
```

### `tests/KNOWN_DIFFERENCES.md`

Document every class of mismatch with linguistic justification:

```markdown
# Known Differences Between libAshtadhyayi and vyakarana Oracle

## 1. Encoding Differences
Some apparent mismatches are encoding artifacts (our SLP1 vs oracle IAST).
Resolution: normalize before comparison.

## 2. Accent Marking
vyakarana sometimes marks udātta accent; we don't yet.
Affected forms: ~5%

## 3. Alternative Forms
Some roots have multiple valid derivations. We implement one;
oracle may implement another. Both correct per Pāṇini.
Example: gam → gacchati (we) vs. gam → ... (oracle edge cases)

## 4. Vedic vs Classical
vyakarana sometimes includes Vedic forms; we target classical Sanskrit only.
```

---

## Acceptance Criteria

- [ ] `tools/run_vyakarana_oracle.py` runs without errors
- [ ] Results TSV generated with ≥ 1000 rows
- [ ] Match rate ≥ 90% on laṭ (present tense)
- [ ] Match rate ≥ 80% across all lakaras implemented
- [ ] `tests/KNOWN_DIFFERENCES.md` documents all mismatch classes
- [ ] `--validate` mode exits 0 at ≥ 90% match

---
---

# Story 6.2 — Subanta Regression: 500-Form SCL Oracle Comparison

**Phase:** 6 — Validation Harness & API Polish  
**Difficulty:** High  
**Estimated time:** 3 hours  
**Depends on:** Story 6.1 (oracle framework established)

---

## Objective

Validate subanta (nominal declension) output against the SCL (Sanskrit Computational Linguistics) morphological analyzer from IIT Hyderabad (Amba Kulkarni's group).

---

## Setup

```bash
# SCL web API (if available):
# https://sanskrit.uohyd.ac.in/scl/

# Or local installation:
# git clone https://github.com/samsaadhanii/scl
# Follow their build instructions
```

---

## Files to Create

### `tools/run_scl_oracle.py`

```python
#!/usr/bin/env python3
"""
run_scl_oracle.py — Compare nominal forms against SCL morphological analyzer.

Usage:
  python3 tools/run_scl_oracle.py              # full run
  python3 tools/run_scl_oracle.py --stem rAma  # single stem
  python3 tools/run_scl_oracle.py --validate   # CI mode
"""

# Test stems
TEST_STEMS = [
  ("rAma",  "PUMS",      "a-stem masculine"),
  ("deva",  "PUMS",      "a-stem masculine"),
  ("rAmA",  "STRI",      "ā-stem feminine"),
  ("kavi",  "PUMS",      "i-stem masculine"),
  ("nadI",  "STRI",      "ī-stem feminine"),
  ("maDu",  "NAPUMSAKA", "u-stem neuter"),
  ("guru",  "PUMS",      "u-stem masculine"),
  ("rAjan", "PUMS",      "n-stem masculine"),
  ("manas", "NAPUMSAKA", "as-stem neuter"),
  ("pitf",  "PUMS",      "ṛ-stem kinship"),
]

VIBHAKTIS = ["PRATHAMA", "DVITIYA", "TRITIYA", "CATURTHI",
              "PANCAMI",  "SHASTHI", "SAPTAMI", "SAMBODHANA"]
VACANAS   = ["EKAVACANA", "DVIVACANA", "BAHUVACANA"]
```

For each combination:
1. Call our library: `ash_subanta(db, stem, linga, vibhakti, vacana)`
2. Query SCL API / local SCL binary with the expected form
3. Check if SCL confirms our output is a valid analysis of that stem
4. Write to `tests/regression/subanta_oracle_results.tsv`

---

### `tests/data/subanta_expected_all.tsv`

Create a hand-verified expected paradigm file for the 10 test stems × 24 forms = 240 rows. This is the ground truth for CI even when SCL is unavailable.

```
# stem  linga  vibhakti  vacana  expected_slp1
rAma    PUMS   PRATHAMA  EKA     rAmaH
rAma    PUMS   PRATHAMA  DVI     rAmO
rAma    PUMS   PRATHAMA  BAH     rAmAH
...
```

---

## Acceptance Criteria

- [ ] `tools/run_scl_oracle.py` runs without errors
- [ ] Results TSV generated with ≥ 500 rows
- [ ] Match rate ≥ 90% for a-stem masculine
- [ ] Match rate ≥ 85% for i/u-stem
- [ ] Match rate ≥ 80% for consonant stems
- [ ] `tests/data/subanta_expected_all.tsv` committed with 240 verified rows

---
---

# Story 6.3 — Sūtra Coverage Report

**Phase:** 6 — Validation Harness & API Polish  
**Difficulty:** Medium  
**Estimated time:** 2.5 hours  
**Depends on:** Story 6.1

---

## Objective

Generate a coverage report showing what fraction of the 3,316 non-Vedic sūtras are actively exercised during derivation of the test suite. Target: ≥ 85% coverage of Phases 0–5 scope.

---

## Background

Not all 3,959 sūtras are equally important for Classical Sanskrit:
- ~643 sūtras deal with Vedic Sanskrit (śakti sūtras, chandas rules) — excluded from target
- ~3,316 sūtras are relevant to Classical Sanskrit
- We target ≥ 85% of sūtras in our implementation scope

---

## Files to Create

### `tools/coverage_report.py`

```python
#!/usr/bin/env python3
"""
coverage_report.py — Report which sūtras are exercised by the test suite.

Output: tests/regression/sutra_coverage_report.txt
"""

import subprocess, csv

def run_test_suite_with_coverage():
    """
    Run the full test suite and collect all sūtra IDs from prakriyā traces.
    Use the --coverage flag of ash_demo or parse test output.
    """
    ...

def load_sutra_list(tsv_path):
    """Load all sūtras and classify as Vedic vs Classical."""
    ...

def generate_report(covered_ids, all_sutras):
    """
    Output:
    - Total Classical sūtras: 3316
    - Covered by test suite: N (X%)
    - Implemented (have code): M (Y%)
    - Not yet implemented: (list by phase)
    """
    ...
```

### `tests/regression/sutra_coverage_report.txt`

Generated output format:
```
===== libAshtadhyayi Sūtra Coverage Report =====

Total sūtras in Aṣṭādhyāyī:  3959
Vedic-only sūtras (excluded): 643
Target (Classical Sanskrit):  3316

Covered by test suite:        2800 (84.5%)
Exercised via prakriyā trace: 1950 (58.8%)

By Phase:
  Phase 1 (Phonology):     95/100 implemented  (95.0%)
  Phase 2 (Sūtra Engine):  85/90  implemented  (94.4%)
  Phase 3 (Tiṅanta):      180/220 implemented  (81.8%)
  Phase 4 (Subanta):      130/160 implemented  (81.3%)
  Phase 5 (Compounds/Kṛt): 90/120 implemented  (75.0%)

Critical sūtras NOT yet implemented:
  7.1.58 (special nom plural forms)
  6.4.111 (upadhā shortening)
  ...
```

---

## Acceptance Criteria

- [ ] `tools/coverage_report.py` runs without errors
- [ ] Coverage report generated correctly
- [ ] Classical sūtra coverage ≥ 75% (Phase 6 target)
- [ ] All critical path sūtras (for bhū, rāma, gam derivations) covered

---
---

# Story 6.4 — Public API Finalization & `ash_db` Opaque Type

**Phase:** 6 — Validation Harness & API Polish  
**Difficulty:** Medium  
**Estimated time:** 2 hours  
**Depends on:** Story 5.1

---

## Objective

Finalize the public API surface, ensuring ABI stability, complete documentation, and that `include/ashtadhyayi.h` is the only header users need to include.

---

## Tasks

### 1. Verify all public functions are implemented

Check every function declared in `include/ashtadhyayi.h` has an implementation:
- `ash_db_load` ✓
- `ash_db_free` ✓
- `ash_sutra_by_addr` ✓
- `ash_sutra_by_id` ✓
- `ash_sutra_print` ✓
- `ash_sandhi_apply` ✓
- `ash_sandhi_split` ✓
- `ash_form_print_prakriya` ✓
- `ash_form_free` ✓
- `ash_tinanta` ✓
- `ash_tinanta_paradigm` ✓
- `ash_subanta` ✓
- `ash_subanta_paradigm` ✓
- `ash_krit` ✓
- `ash_samasa` ✓
- `ash_pratyahara_expand` ✓
- `ash_in_pratyahara` ✓
- `ash_guna` ✓
- `ash_vrddhi` ✓
- `ash_version` ✓
- `ash_encode` ✓

### 2. Implement missing convenience wrappers

Any function declared but not yet implemented gets a stub that returns an error form:
```c
ASH_Form ash_krit(const ASH_DB *db, const char *root_slp1, int gana,
                   ASH_KritType krit) {
  /* Delegate to internal krit_derive */
  return krit_derive(&db->pipeline.sutras, root_slp1, gana, krit);
}
```

### 3. Implement `ash_tinanta_paradigm` and `ash_subanta_paradigm`

These are missing convenience wrappers that generate all 18 / 24 forms at once.

### 4. Doxygen documentation

Add Doxygen comment blocks to every public function in `include/ashtadhyayi.h`.

### 5. API usage example

Create/update `examples/demo.c`:

```c
#include <stdio.h>
#include <stdlib.h>
#include "ashtadhyayi.h"

int main(void) {
  ASH_DB *db = ash_db_load("data/");
  if (!db) { fprintf(stderr, "Failed to load data\n"); return 1; }

  printf("libAshtadhyayi version: %s\n\n", ash_version());

  /* === Tiṅanta: bhū → bhavati === */
  printf("=== Verbal Derivation: bhū (Class 1, Present) ===\n");
  ASH_Form f = ash_tinanta(db, "BU", 1, ASH_LAT,
                             ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  if (f.valid) {
    printf("SLP1:      %s\n", f.slp1);
    printf("IAST:      %s\n", f.iast);
    printf("Devanāgarī: %s\n\n", f.devanagari);
    printf("Derivation trace:\n");
    ash_form_print_prakriya(&f, stdout);
  }
  ash_form_free(&f);

  /* === Subanta: rāma → rāmeṇa === */
  printf("\n=== Nominal Declension: rāma (inst. sg.) ===\n");
  f = ash_subanta(db, "rAma", ASH_PUMS, ASH_TRITIYA_VIB, ASH_EKAVACANA);
  if (f.valid) {
    printf("IAST: %s\n", f.iast);
    ash_form_print_prakriya(&f, stdout);
  }
  ash_form_free(&f);

  /* === Compound: rāja + puruṣa === */
  printf("\n=== Compound: rāja-puruṣa (tatpuruṣa) ===\n");
  f = ash_samasa(db, "rAja", "puruzwa", ASH_SAMASA_TATPURUSHA, ASH_PUMS);
  if (f.valid) printf("IAST: %s\n", f.iast);
  ash_form_free(&f);

  /* === Sandhi === */
  printf("\n=== Sandhi: rāma + āste ===\n");
  char sandhi_out[128];
  ash_sandhi_apply("rAma", "Aste", sandhi_out, sizeof(sandhi_out), ASH_ENC_IAST);
  printf("Result: %s\n", sandhi_out);

  ash_db_free(db);
  return 0;
}
```

---

## Acceptance Criteria

- [ ] All 21 public functions implemented (or stub returning error form)
- [ ] `examples/demo.c` compiles and runs producing correct output
- [ ] Doxygen comments on all public functions
- [ ] `#include "ashtadhyayi.h"` is the ONLY include needed by users
- [ ] No internal headers in `include/`

---
---

# Story 6.5 — pkg-config Install & CI Pipeline

**Phase:** 6 — Validation Harness & API Polish  
**Difficulty:** Easy  
**Estimated time:** 1.5 hours  
**Depends on:** Story 6.4

---

## Objective

Set up the install target, pkg-config integration, and a GitHub Actions CI pipeline that runs all validation targets automatically.

---

## Files to Create

### `.github/workflows/ci.yml`

```yaml
name: libAshtadhyayi CI

on:
  push:
    branches: [main, develop]
  pull_request:
    branches: [main]

jobs:
  build-and-test:
    runs-on: ubuntu-24.04
    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y cmake gcc valgrind python3 python3-pip
          pip3 install requests

      - name: Download Unity test framework
        run: |
          mkdir -p tests/unity
          curl -L https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity.c \
            -o tests/unity/unity.c
          curl -L https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity.h \
            -o tests/unity/unity.h
          curl -L https://raw.githubusercontent.com/ThrowTheSwitch/Unity/master/src/unity_internals.h \
            -o tests/unity/unity_internals.h

      - name: Ingest data
        run: |
          python3 tools/ingest_source.py
          python3 tools/ingest_dhatupatha.py
          python3 tools/ingest_ganapatha.py

      - name: Build
        run: |
          cmake -B build -DCMAKE_BUILD_TYPE=Debug
          cmake --build build -- -j$(nproc)

      - name: Run tests
        run: ctest --test-dir build --output-on-failure

      - name: Validate Phase 0
        run: make validate-phase0

      - name: Validate Phase 1
        run: make validate-phase1

      - name: Validate Phase 2
        run: make validate-phase2

      - name: Validate Phase 3
        run: make validate-phase3

      - name: Memory check (valgrind)
        run: |
          valgrind --error-exitcode=1 --leak-check=full \
            ./build/test_build
```

---

## Install target

Verify `cmake --install build --prefix /tmp/ash_test` installs:
- `/tmp/ash_test/lib/libashtadhyayi.a`
- `/tmp/ash_test/include/ashtadhyayi.h`
- `/tmp/ash_test/lib/pkgconfig/libashtadhyayi.pc`

Test pkg-config works:
```bash
export PKG_CONFIG_PATH=/tmp/ash_test/lib/pkgconfig
pkg-config --cflags --libs libashtadhyayi
# Should output: -I/tmp/ash_test/include -L/tmp/ash_test/lib -lashtadhyayi
```

---

## Acceptance Criteria

- [ ] `.github/workflows/ci.yml` triggers on push to main
- [ ] All phases validate in CI
- [ ] Valgrind reports zero leaks on unit tests
- [ ] `cmake --install` creates correct directory structure
- [ ] pkg-config output correct

---
---

# Story 6.6 — Final Regression, Documentation & Release Tag

**Phase:** 6 — Validation Harness & API Polish  
**Difficulty:** Medium  
**Estimated time:** 3 hours  
**Depends on:** Story 6.5

---

## Objective

Run the final full regression across all implemented functionality, update all documentation, generate the coverage report, and tag the v0.1.0 release.

---

## Tasks

### 1. Final regression run

```bash
make validate-phase0
make validate-phase1
make validate-phase2
make validate-phase3
make validate-phase4
make validate-phase5
make validate-phase6
```

All must be green. Fix any remaining issues.

### 2. Update `README.md`

- Add actual match rates from oracle comparisons
- Add "Current Status" section with phase completion table
- Add actual example output from `examples/demo.c`

### 3. Regenerate coverage report

```bash
python3 tools/coverage_report.py > tests/regression/sutra_coverage_report.txt
```

### 4. Update `tests/KNOWN_DIFFERENCES.md`

- Final count of known differences vs vyakarana
- Final count vs SCL
- Linguistic justification for each

### 5. Tag v0.1.0

```bash
git tag -a v0.1.0 -m "Initial release: Phases 0–5 complete, ≥85% Classical Sanskrit coverage"
git push origin v0.1.0
```

### 6. Create `CHANGELOG.md`

```markdown
# Changelog

## v0.1.0 (2026-04-xx) — Initial Release

### Implemented
- Phase 0: Full data ingestion (3959 sūtras, 1943 dhātus, 260+ gaṇas)
- Phase 1: Complete phonology engine (52 phonemes, 44 pratyāhāras, all sandhi types)
- Phase 2: Sūtra loader, saṃjñā registry, anubandha stripping, adhikāra/anuvṛtti/paribhāṣā
- Phase 3: Tiṅanta for Classes 1, 4, 6, 10 (laṭ fully, other lakaras partial)
- Phase 4: Subanta for a, ā, i, ī, u, ū stems + n/at/as/ṛ-stems
- Phase 5: Unified pipeline, 6 samāsa types, 9 kṛt types, 15 taddhita types, Uṇādi lookup

### Validation Results
- Tiṅanta vs vyakarana oracle: X% match (N/1000 forms)
- Subanta vs SCL oracle: Y% match (M/500 forms)
- Classical sūtra coverage: Z%

### Known Limitations
- Lakaras 2–10 partially implemented
- Vedic Sanskrit not targeted
- Accent marking not implemented
```

---

## Acceptance Criteria

- [ ] All 6 phase validation targets green
- [ ] Coverage report shows ≥ 75% Classical sūtra coverage
- [ ] `CHANGELOG.md` created
- [ ] `v0.1.0` tag pushed to GitHub
- [ ] `tests/KNOWN_DIFFERENCES.md` finalized
- [ ] Zero compiler warnings on all source files
