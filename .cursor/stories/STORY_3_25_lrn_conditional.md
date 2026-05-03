# Story 3.25 — LRN (conditional)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Medium
**Estimated time:** 3 hours
**Depends on:** Story 3.17 (LAN augment), Story 3.20 (LRT sya-augment)

---

## Objective

Implement lṛṅ (conditional, "would have"). Lṛṅ is essentially LRT's
syA augment + LAN's a-augment + secondary endings. It is the
counterfactual past-conditional.

Examples:
- `bhū` LRN prathama-eka-P → `abhaviṣyat`
- `kṛ` LRN prathama-eka-P → `akariṣyat`
- `gam` LRN prathama-eka-P → `agamiṣyat`

---

## Background

Steps (3.3.139 → 3.4.111):
1. 3.3.139 liṅnimitte lṛṅ kriyātipattau — assigns lṛṅ
2. 6.4.71 luṅ-laṅ-lṛṅ-kṣv aḍudāttaḥ — a-augment
3. 3.1.33 syatāsi luṭos — sya inserted (same as LRT)
4. 7.2.10 ekāca upadeśe — iṭ for set roots
5. Secondary endings (same as LAN).

Implementation is a thin combination of LAN augment + LRT stem-
building + LAN secondary endings.

---

## Files

### `prakriya/tinanta/lrn.h` / `lrn.c` (new, ~150 lines)

```c
bool lrn_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                    ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline:
1. Build LRT stem (root + iṭ + sya) using lrt_derive_ctx as helper.
2. Replace primary endings with secondary endings.
3. Prepend a-augment.

Implementation can largely re-use lrt + lan helpers.

### `prakriya/tinanta/lakara.c`
LRN-parasmai uses LAN_PARASMAI clean endings.

---

## Acceptance Criteria

- [ ] `lrn_derive("BU", 1, P, EKA, P)` → `aBavizyat`
- [ ] `lrn_derive("kf", 8, P, EKA, P)` → `akarizyat`
- [ ] `lrn_derive("gam", 1, P, EKA, P)` → `agamizyat`
- [ ] All 9 LRN-P forms of bhū match oracle
- [ ] `make validate-phase3` still green
- [ ] LRN oracle reaches ≥ 70%
