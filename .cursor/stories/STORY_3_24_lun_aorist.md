# Story 3.24 — LUN (aorist)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Very High
**Estimated time:** 14 hours (largest tinanta story)
**Depends on:** Story 3.17 (LAN augment), Story 3.18 (reduplication)

---

## Objective

Implement luṅ (aorist, simple past). Luṅ is by far the most
heterogeneous Sanskrit lakāra: each root takes one of **seven
distinct aorist sub-types** (3.1.43–66) plus the standard a-augment
(6.4.71) and secondary endings.

The seven aorists:
1. **cli-lopa** (Class 1) — 3.1.55, no aorist marker (e.g. abhūt)
2. **siC** (Class 2) — 3.1.44, sigmatic with siC suffix (e.g. agaiṣīt)
3. **iSīC** (Class 3) — 3.1.45, sigmatic with iṣ + siC (e.g. abhāvīt)
4. **siC** with vrddhi (Class 4) — 3.1.45 alt
5. **a-aorist** (Class 5) — 3.1.55, simple a + secondary ending (alipat)
6. **caṅ** (Class 6) — 3.1.48, reduplicating + ṇic + a (acīkarat from kṛ)
7. **ḍa-aorist** (Class 7) — 3.1.61, less common

Examples:
- `bhū` LUN → `aBUt` (cli-lopa class 1)
- `kṛ` LUN → `akArṣīt` (siC class 2)
- `gam` LUN → `agamat` (a-aorist class 5)
- `pat` LUN → `apaptat` (a-aorist with reduplication)

---

## Background

This is genuinely complex. The right approach:
1. Encode a small per-root **aorist-class** lookup table (drawn from
   dhātupāṭha settva column + traditional commentaries).
2. For each class, write a dedicated builder.
3. Share the augment (a-) and secondary endings infrastructure with
   LAN (Story 3.17).

The 9 secondary endings shared with LAN:
- prathama: t, tām, an
- madhyama: H, tam, ta
- uttama: am, va, ma

---

## Files

### `prakriya/tinanta/lun.h` / `lun.c` (new, ~600 lines)

```c
typedef enum {
  LUN_CLI_LOPA = 1,    /* 3.1.55 */
  LUN_SIC,             /* 3.1.44 */
  LUN_ISIC,            /* 3.1.45 */
  LUN_SIC_VRDDHI,
  LUN_A_AORIST,        /* 3.1.55 */
  LUN_CANG,            /* 3.1.48 */
  LUN_DA,              /* 3.1.61 */
} AoristClass;

bool lun_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                    ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline:
1. Look up root's aorist class.
2. Apply class-specific stem-building.
3. Prepend a-augment.
4. Append secondary tiṅ ending.

### `prakriya/tinanta/aorist_classes.h` (new)
Closed list of ~50 roots → aorist class mapping. The remaining roots
default to a-aorist (class 5) and produce informationally-correct if
not always oracle-matching forms.

### `prakriya/tinanta/lakara.c`
Add `LUN_PARASMAI[9]` and `LUN_ATMANE[9]` tables (same as LAN).

---

## Acceptance Criteria

- [ ] `lun_derive("BU", 1, P, EKA, P)` → `aBUt`
- [ ] `lun_derive("kf", 8, P, EKA, P)` → `akArzIt`
- [ ] `lun_derive("gam", 1, P, EKA, P)` → `agamat`
- [ ] `lun_derive("pat", 1, P, EKA, P)` → `apaptat`
- [ ] `make validate-phase3` still green
- [ ] LUN oracle reaches ≥ 50% (the seven-class diversity makes 100%
      a multi-story effort).
