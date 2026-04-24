# Phase 6 — Validation Harness & API Polish (Stories 6.1–6.6)

**Phase:** 6  
**Depends on:** All Phase 5 stories complete

The validation strategy was revised after discovering that the `ashtadhyayi-com/data`
repository contains three pre-computed reference oracles (see Story 0.5) that are far
higher quality than the external oracles we initially planned to use.

---

# Story 6.1 — Tiṅanta Regression vs `dhatuforms.tsv` Oracle

**Difficulty:** Medium  
**Estimated time:** 3 hours  
**Depends on:** Story 5.1 (pipeline complete), Story 0.5 (oracle data loaded)

## Objective

Compare our tiṅanta output against `data/dhatuforms.tsv` (254,736 pre-computed forms).
Target: ≥99% exact match on LAT (present tense) for all 2,229 dhātus × 18 forms.

## Implementation

Create `tools/oracle_tinanta.py`:

```python
#!/usr/bin/env python3
"""
oracle_tinanta.py — Compare our library against dhatuforms.tsv

Reads data/dhatuforms.tsv and compares each pre-computed form against
what our library produces via the ash_tinanta() API.
"""

import csv, subprocess, sys

def compare_lakara(lakara='LAT', limit=None):
    matched = total = 0
    mismatches = []
    with open('data/dhatuforms.tsv', encoding='utf-8') as f:
        reader = csv.DictReader(f, delimiter='\t')
        for row in reader:
            if row['lakara'] != lakara: continue
            if limit and total >= limit: break
            total += 1
            # Look up the dhatu's SLP1 root from dhatupatha.tsv
            # Then call our library
            # Compare form_deva
            # ... (full impl)

if __name__ == '__main__':
    compare_lakara(limit=1000)
```

## Acceptance Criteria

- [ ] `tools/oracle_tinanta.py --validate` exits 0
- [ ] LAT parasmai match rate ≥ 95% on 1000-root sample
- [ ] LAT ātmane match rate ≥ 90%
- [ ] All mismatches documented in `tests/KNOWN_DIFFERENCES.md` with sūtra-level cause

---

# Story 6.2 — Subanta Regression vs `shabda_forms.tsv` Oracle

**Difficulty:** Medium  
**Depends on:** Story 5.1 (pipeline complete), Story 0.5

## Objective

Compare our subanta output against `data/shabda_forms.tsv` (216,168 pre-computed forms).

## Implementation

`tools/oracle_subanta.py` — same pattern as oracle_tinanta.py but reads
shabda_forms.tsv. Filter by most common linga and test 50 representative stems
× 24 forms = 1,200 forms.

## Acceptance Criteria

- [ ] a-stem masculine match rate ≥ 99%
- [ ] ā-stem feminine match rate ≥ 98%
- [ ] i/u-stem match rate ≥ 95%
- [ ] Consonant-stem match rate ≥ 85%

---

# Story 6.2b — Prakriyā Trace Validation vs `shabdaprakriya.tsv`

**Difficulty:** High  
**Depends on:** Story 6.2

## Objective

Validate that our step-by-step prakriyā output matches the authoritative traces
in `data/shabdaprakriya.tsv`. For each of 100 representative forms, check:
1. Same number of steps (±1 for alternative derivations)
2. Each step fires a sūtra that is in the oracle's sutras list for that step
3. Final form matches

## Implementation

```python
def compare_prakriya(stem_deva, form_deva):
    # Read oracle steps from shabdaprakriya.tsv
    # Call our library and get ASH_Form
    # For each step in our output:
    #   Check sutra_id is in oracle_step.sutras
    # Report match percentage per-step and overall
```

## Acceptance Criteria

- [ ] ≥ 70% of steps have correct sūtra attribution
- [ ] Final form matches in ≥ 95% of cases

---

# Story 6.3 — Sūtra Coverage Report

**Difficulty:** Medium

## Objective

Report what fraction of the 3,983 sūtras are actively exercised by our test suite.

## Implementation

`tools/coverage_report.py` — already exists. Run the full test suite under a
coverage harness that captures every sūtra global_id that appears in
`ASH_PrakriyaStep.sutra_id` fields. Cross-reference with `data/sutras.tsv`.

## Acceptance Criteria

- [ ] Classical (non-Vedic) sūtra coverage ≥ 60% after Phase 5
- [ ] Target ≥ 85% by v1.0
- [ ] Critical path sūtras (1.1.1, 1.1.2, 3.1.68, 6.1.77, 7.3.84, 8.3.15)
      all present in coverage set

---

# Story 6.4 — Public API Finalization & Doxygen

**Difficulty:** Easy

## Objective

Verify all 21 public functions in `include/ashtadhyayi.h` are implemented
(stubs are OK if they return valid `ASH_Form` with informative error messages),
add Doxygen comments, and finalize `examples/demo.c`.

## Acceptance Criteria

- [ ] All 21 public functions present
- [ ] Every public function has a `@brief` and `@param` Doxygen block
- [ ] `examples/demo.c` exercises: ash_tinanta, ash_subanta, ash_samasa, ash_sandhi_apply
- [ ] Only `include/ashtadhyayi.h` needs to be #included by users

---

# Story 6.5 — pkg-config Install & CI

**Difficulty:** Easy

## Objective

Verify `cmake --install build --prefix /tmp/inst` creates:
- `/tmp/inst/lib/libashtadhyayi.a`
- `/tmp/inst/include/ashtadhyayi.h`
- `/tmp/inst/lib/pkgconfig/libashtadhyayi.pc`

Create `.github/workflows/ci.yml` that:
1. Builds with gcc 13 + clang 17
2. Runs all validate-phaseN targets
3. Runs valgrind on unit tests
4. Runs the oracle comparison scripts

## Acceptance Criteria

- [ ] pkg-config output correct
- [ ] CI green on main branch
- [ ] Valgrind zero leaks

---

# Story 6.6 — v0.1.0 Release

**Difficulty:** Easy

Tag v0.1.0 with CHANGELOG, coverage report, and oracle match rates in the release notes.

## Acceptance Criteria

- [ ] All 6 phase validation targets green
- [ ] CHANGELOG.md finalized
- [ ] v0.1.0 tag pushed
- [ ] Release notes include oracle match rates (LAT ≥95%, subanta ≥99%)
