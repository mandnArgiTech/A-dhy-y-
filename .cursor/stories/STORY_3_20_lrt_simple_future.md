# Story 3.20 — LRT (simple future)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Medium
**Estimated time:** 4 hours
**Depends on:** Story 3.16 (LAT 100%)

---

## Objective

Implement lṛṭ (simple future, "lṛṭ ca" 3.3.13). Lṛṭ inserts the
**`syati`** augment (sya-vikaraṇa with possible iṭ-augment) between
root and standard primary endings (same as LAT).

Forms like:
- `bhū` LRT prathama-eka-P → `bhaviṣyati`
- `kṛ` LRT prathama-eka-P → `kariṣyati`
- `gam` LRT prathama-eka-P → `gamiṣyati`

---

## Background

Steps (3.3.13 → 3.1.33 → 7.2.10):
1. 3.3.13 lṛṭ śeṣe ca — assigns lṛṭ
2. 3.1.33 syatāsi luṭos — sya suffix inserted
3. 7.2.10 ekāca upadeśe — iṭ augment for set roots before sya
4. 7.3.84 sārvadhātukārdhadhātukayoḥ — guṇa of root final ik vowel
5. 8.3.59 ādeśapratyayoḥ — s of sya becomes ṣ after iṇ vowel
6. Endings are LAT-style primary endings (ti, taḥ, anti, etc.)

---

## Files

### `prakriya/tinanta/lrt.h` / `lrt.c` (new, ~150 lines)

```c
bool lrt_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                    ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline:
1. Strip anubandhas → clean root.
2. Apply 7.3.84 guṇa to root final ik vowel.
3. Insert iṭ augment if seṭ root (use SET_ROOTS list from kṛt).
4. Append `sya` (auto-converted to `zya` after iṇ via 8.3.59).
5. Append LAT-style primary tiṅ ending.

### `prakriya/tinanta/lakara.c`
LRT-parasmai uses LAT_PARASMAI clean endings.

---

## Acceptance Criteria

- [ ] `lrt_derive("BU", 1, P, EKA, P)` → `Bavizyati`
- [ ] `lrt_derive("kf", 8, P, EKA, P)` → `karizyati`
- [ ] `lrt_derive("gam", 1, P, EKA, P)` → `gamizyati`
- [ ] `lrt_derive("ad", 2, P, EKA, P)` → `atsyati` (no iṭ for ad; aniṭ)
- [ ] All 9 LRT-P forms of bhū match oracle
- [ ] `make validate-phase3` still green
- [ ] LRT oracle reaches ≥ 75%
