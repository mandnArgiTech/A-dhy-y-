> **Status: CLOSED in commit 2a642ca + 1ce380d.** This file is kept for reference; do not re-implement.

# [CLOSED] BUG-002 — Oracle "normalize" function rigs the match rate

**Severity:** CRITICAL — invalidates published 88.89% / 100% pass rates  
**Phase blocked:** 6 (validation), Phase 6.1 / 6.2 / 6.3 acceptance  
**Files affected:** `tools/run_vyakarana_oracle.py`, `tools/run_scl_oracle.py`  
**Depends on:** BUG-001 must be fixed first

---

## Symptom

`tools/run_vyakarana_oracle.py::normalize_tinanta()` performs aggressive lossy transformations on **both** the library output and the oracle string before comparing, producing artificially high "match" rates against the (also broken) oracle.

Current normalize() applies:
1. Strip every `a` between two consonants from our output (`Bavati` → `Bvti`)
2. Hardcoded substitutions: `atas → tH`, `aTas → TH`, `ami → Ami`, `avas → AvH`, `amas → AmH`, `ate → te`, `ase → se`, `aDve → Dve`, `aAte → ete`, `aATe → eTe`, `avahe → Avhe`, `amahe → Amhe`
3. Final-`s` → `H` substitution

These collectively force our (correct) `Bavati` output to match the oracle's (incorrect) `Bvti`. Result: 88.89% match rate on a sample of **9 rows**, all for **1 root** (`BU`), reported as PASS against the configured ≥85% threshold.

The same pattern exists in `tools/run_scl_oracle.py` for subanta validation.

## What's actually being measured today

```
tests/regression/tinanta_oracle_results.tsv
  9 rows total — all root=BU, lakara=LAT
  our_slp1 column:    'Bavati', 'Bavatas', 'Bavanti', 'Bavasi', ...
  oracle_slp1 column: 'Bvti',   'BvtH',    'Bvnti',   'Bvsi',   ...
  match=1 for 8 rows because normalize hides the a-drop bug
```

## Why this is wrong

A validation harness must:
1. Compare canonical forms for true equality (after at most trivial normalization like Unicode NFC)
2. Report the raw match rate, not a smoothed-over rate
3. Cover a representative sample of dhātus / stems, not 1 root

The current code does the opposite of all three.

## Required fix

After BUG-001 is fixed, the oracle TSVs will contain correct SLP1. Then:

1. **Delete `normalize_tinanta()` and `normalize_subanta()` entirely.** Replace with strict string equality after Unicode NFC normalization on Devanāgarī inputs.
2. **Compare on `form_deva` (Devanāgarī), not `form_slp1`.** The Devanāgarī column is the source of truth — SLP1 is our internal encoding only. Convert our library's SLP1 output to Devanāgarī using `enc_slp1_to_devanagari()` and compare.
3. **Expand the test sample.** Remove `SUPPORTED_VALIDATE_ROOTS = {"BU"}`. Default to a 50-root sample from `data/dhatupatha.tsv` (10 from each of gaṇas 1, 4, 6, 10 + 10 ubhayapada roots) for tiṅanta. For subanta, use the 50 most-frequent stems from `data/shabda_forms.tsv`.
4. **Report raw and per-category rates** — overall, per-gaṇa, per-vibhakti, per-vacana. Don't average them away.
5. **Lower the PASS threshold to match reality.** Once the bug fixes land and we measure honestly, the rate will likely fall to 5–20% on a 50-root sample. That's the truth, and that's what the README and Phase 6 stories should reflect. As Phase 3/4 implementations improve (BUG-003), the rate climbs.

## Acceptance Criteria

- [ ] `normalize_tinanta` and `normalize_subanta` deleted (no replacement that hides errors)
- [ ] Comparison is exact equality on `form_deva` after Unicode NFC
- [ ] Sample size: ≥50 dhātus for tiṅanta, ≥50 stems for subanta
- [ ] Output report includes:
  - overall rate
  - rate per gaṇa (1–10)
  - rate per vibhakti (subanta only)
  - rate per vacana
  - top-20 mismatches with both forms shown
- [ ] No PASS threshold gate during the broken-implementation period — script should run informationally and print a banner: `# of 50 roots × 18 forms = 900 total; matched: NN; rate: X.XX%`
- [ ] After BUG-003 (real derivation) is implemented, threshold is re-introduced at empirically-justified levels (start with ≥40%, raise as implementation matures)

## Estimated effort

3 hours (mostly rewriting comparison logic and report generation)
