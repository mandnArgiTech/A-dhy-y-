# Story 3.18 — LIT (perfect tense) and reduplication infrastructure

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Very High
**Estimated time:** 12 hours
**Depends on:** Story 3.17 (LAN scaffolding), Story 3.27 (reduplication)

---

## Objective

Implement liṭ (perfect tense). The perfect is morphologically the
most complex Sanskrit lakāra: it requires **abhyāsa (reduplication)**
of the root, special perfect endings (Ṇal, atus, us, etc.), and a
host of root-specific allomorphs (e.g. cakāra from kṛ, jagāma from
gam, paspraṣa from spṛś).

This story builds:
1. A general **reduplication engine** (Story 3.27) usable by liṭ,
   gaṇa-3, slu-derivatives, etc.
2. The liṭ tiṅ table (Ṇal, atus, us, Tal, aThuH, a, ṇal-ami, va, ma).
3. Per-root allomorph rules from 6.4.120 ata ekahalmadhye and friends.

---

## Background — bhū LIT-parasmai paradigm

| | Eka | Dvi | Bahu |
|-|-|-|-|
| Prathama | baBUva | baBUvatuH | baBUvuH |
| Madhyama | baBUviTa | baBUvaTuH | baBUva |
| Uttama | baBUva | baBUviva | baBUvima |

Steps (3.4.78 → 7.4.10 → 6.1.8):
1. 3.4.78 LIT replaces lit-anubandha → ting endings (Ṇal etc.)
2. 6.1.8 līṭi dhātor anabhyāsasya — reduplicate root
3. 7.4.66 ut paratasya — abhyāsa vowel may shorten/change
4. 7.2.13 dvitvābhyāsa special rules
5. Final guṇa/vṛddhi in singular forms (sārvadhātuka apit logic from LAT)

---

## Files

### `prakriya/tinanta/reduplication.h` (new — Story 3.27)
```c
/** Reduplicate a dhātu's first syllable per the abhyāsa rules. */
bool reduplicate(const char *clean_root, char *out, size_t out_len);
```

### `prakriya/tinanta/lit.h` / `lit.c` (new, ~300 lines)
```c
bool lit_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                    ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Implementation outline:
1. Strip anubandhas → clean root.
2. Call reduplicate() to get abhyāsa form.
3. Apply 7.4.60–66 rules to abhyāsa (consonant simplification,
   vowel shortening, palatal substitution).
4. Append liṭ ending (with strong/weak per pit-anubandha).
5. Apply guṇa/vṛddhi to root vowel for strong forms.
6. Handle special-case roots (set list: kf → cakāra, gam → jagāma,
   pad → peda, spṛś → pasparśa).

### `prakriya/tinanta/lakara.c`
Add `LIT_PARASMAI[9]` and `LIT_ATMANE[9]` tables.

---

## Acceptance Criteria

- [ ] `reduplicate("BU")` → `baBU`
- [ ] `reduplicate("kf")` → `cakf`
- [ ] `reduplicate("gam")` → `jagam`
- [ ] `lit_derive("BU", 1, P, EKA, P)` → `baBUva`
- [ ] `lit_derive("kf", 8, P, EKA, P)` → `cakAra`
- [ ] `lit_derive("gam", 1, P, EKA, P)` → `jagAma`
- [ ] `lit_derive("BU", 1, P, BAHU, P)` → `baBUvuH`
- [ ] `make validate-phase3` still green
- [ ] LIT oracle (`run_vyakarana_oracle.py --lakara LIT`) reaches ≥ 70%
- [ ] `tests/unit/test_reduplication.c` covers all 7 reduplication patterns
