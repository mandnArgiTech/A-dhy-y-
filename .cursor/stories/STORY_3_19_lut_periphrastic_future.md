# Story 3.19 — LUT (periphrastic future)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Medium
**Estimated time:** 4 hours
**Depends on:** Story 3.20 (LRT — they share the iṭ + tās augments)

---

## Objective

Implement luṭ (periphrastic future, distant future "anadyatane luṭ"
3.3.15). Luṭ inserts the "tās" augment (technically `tās-i` but
realised as -tā- in singular) between the root and a special set
of endings derived from `as` (the root "to be") + iṭ.

Forms like:
- `bhū` LUT prathama-eka-P → `bhavitā`
- `kṛ` LUT prathama-eka-P → `kartā`
- `gam` LUT prathama-eka-P → `gantā`

---

## Background

Steps (3.3.15 → 7.2.61 → 7.3.96):
1. 3.3.15 anadyatane luṭ — assigns luṭ
2. 7.3.96 astisicʼoʼpṛkte — root vowel guṇa applies
3. 3.1.33 syatāsi luṭos — tās suffix inserted
4. 7.2.61 acas tāsvad — iṭ augment inserted before tās for set roots
5. 3.4.78 → ṇal-pattern endings (only first three of the puruṣa table
   exist for luṭ; remaining slots use compound with as-forms)

Periphrastic structure: the singular forms are built directly
(`bhavitā`, `bhavitāsi`, `bhavitāsmi`); other forms historically use
periphrastic constructions with `as` conjugated for them. For oracle
purposes most lexicons record the simple kartṛ-style forms.

---

## Files

### `prakriya/tinanta/lut.h` / `lut.c` (new, ~200 lines)

```c
bool lut_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                    ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline:
1. Strip anubandhas; classify root iṭ-class (seṭ/aniṭ).
2. Apply guṇa to root final ik vowel (7.3.96).
3. Insert `i` augment if seṭ.
4. Append `tās` (or `tā` in pratyāya-eka).
5. Append puruṣa-vacana ending from luṭ table.

### `prakriya/tinanta/lakara.c`
Add `LUT_PARASMAI[9]` table.

---

## Acceptance Criteria

- [ ] `lut_derive("BU", 1, P, EKA, P)` → `BavitA`
- [ ] `lut_derive("kf", 8, P, EKA, P)` → `kartA`
- [ ] `lut_derive("gam", 1, P, EKA, P)` → `gantA`
- [ ] `lut_derive("pat", 1, P, EKA, P)` → `patitA` (set)
- [ ] All 9 LUT-P forms of bhū match oracle
- [ ] `make validate-phase3` still green
- [ ] LUT oracle reaches ≥ 60%
