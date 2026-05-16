# Story 3.26 — Ātmanepada full coverage across all lakāras

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** High
**Estimated time:** 8 hours
**Depends on:** Stories 3.16–3.25 (parasmai LAT through LRN)

---

## Objective

The current engine derives parasmai forms for every lakāra; the
ātmanepada side runs through the same code path but the engine has
not been validated against an oracle for ātmane forms. This story
brings ātmanepada to oracle parity by:

1. Reviewing each ātmane ting table (already populated in lakara.c
   for LAT_ATMANE; equivalent tables to be added for LAN, LIT, LUT,
   LRT, LOT, VIDHILIN, ASIRLIN, LUN, LRN).
2. Handling ātmane-specific sandhi: `i` of "ate"/"ase"/"ete" endings
   triggers 8.3.59 ṣatva inconsistently (already partially fixed).
3. Specific ātmane rules:
   - 7.3.78 ṅit ending behaviour for vahe/mahe (no guṇa)
   - 7.3.101 ato dīrgho yaṅi for `vahe`/`mahe` is already
     implemented; verify.
   - 7.4.46-50 ātmanepada-specific augments for liṭ ātmane
     (cakre, cakrāte, cakrire from kṛ).
4. Pada selection: roots flagged P-only or A-only in dhātupāṭha
   (column `pada_flag` = P/A/U) should reject the wrong pada
   gracefully.

Goal: ātmane oracle rate matches parasmai for every lakāra
implemented (ideally ≥ 90% for LAT-A, ≥ 70% for others).

---

## Background

Ātmane vs Parasmai differences:
- Different ending sets (te, ete, ante, se, ethe, dhve, e, vahe, mahe).
- Strong/weak pattern differs: ātmane LAT has uniformly weak stem
  in athematic gaṇas (no pit-anubandha endings).
- Liṭ ātmane: distinct e-final pattern (cakre, jagme, etc.).

---

## Files

### Each `prakriya/tinanta/<lakara>.c`
Verify ātmane branches; add missing ones.

### `prakriya/tinanta/lakara.c`
Add the missing _ATMANE tables for LAN/LIT/LUT/LRT/LOT/VIDHILIN/
ASIRLIN/LUN/LRN.

### `tools/run_vyakarana_oracle.py`
Extend sample loading to include `pada_flag != "P"` rows when the
caller passes `--pada A`.

### Pada-flag enforcement (optional)
- Read pada_flag from `data/dhatupatha.tsv` and reject derivations
  that don't match.

---

## Acceptance Criteria

- [ ] `ash_demo tinanta BU 1 LAT PRATHAMA EKAVACANA ATMANE` → `Bavate`
- [ ] `ash_demo tinanta gam 1 LAT PRATHAMA EKAVACANA ATMANE` → fail
      gracefully (gam is parasmai-only); or produce `gacCate`
      informationally.
- [ ] LAT-ātmane oracle rate ≥ 95% on a 100-row sample.
- [ ] All 27 unit tests still pass.
- [ ] `make validate-phase3` still green.
