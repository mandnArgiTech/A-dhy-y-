# Story 3.23 — ĀŚIRLIN (benedictive / precative)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Medium-High
**Estimated time:** 4 hours
**Depends on:** Story 3.22 (VIDHILIN — they share liṅ-ending structure)

---

## Objective

Implement āśīrliṅ (benedictive, "expressing blessing"). Āśīrliṅ
inserts the **`yāsuṭ`** augment + an additional **`s`** (the
`sIyuṭ` formative) between root and secondary endings, with no
vikaraṇa.

Examples:
- `bhū` ASIRLIN prathama-eka-P → `bhūyāt`
- `gam` ASIRLIN prathama-eka-P → `gamyāt`
- `kṛ` ASIRLIN prathama-eka-P → `kriyāt`

---

## Background

Steps (3.3.173 → 3.4.116):
1. 3.3.173 āśiṣi liṅ-loṭau — assigns liṅ in benediction sense
2. 3.4.116 liṅaḥ sīyuṭ — sīyuṭ augment after liṅ
3. 3.4.103-104 yāsuṭ + āṭ — combined to give yās
4. No vikaraṇa (athematic style — applies to all gaṇas uniformly)
5. Secondary endings + special sandhi at root-yās junction

For benedictive, the root surfaces in its bare form (no guṇa, since
the suffix is treated as kit by 1.2.10 halaḥ śnaḥ śānajbhyām):
- bhū + yāst → bhūyāt
- gam + yāst → gamyāt
- kṛ + yās → kriyāt (saṃprasāraṇa: kṛ → kr + i)

Final s drops before t (8.2.66/8.3.15 visarga rules) — for ASIRLIN
prathama-eka the result is just `bhūyāt` (no -s remnant).

---

## Files

### `prakriya/tinanta/asirlin.h` / `asirlin.c` (new, ~200 lines)

```c
bool asirlin_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                        ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline:
1. Strip anubandhas → clean root (no guṇa applied; suffix is kit).
2. Apply saṃprasāraṇa for kṛ → kr-i / dṛś → dṛś-i etc.
3. Append `yA` (with internal s for some cases).
4. Append secondary tiṅ endings.

### `prakriya/tinanta/lakara.c`
Add `ASIRLIN_PARASMAI[9]` table.

---

## Acceptance Criteria

- [ ] `asirlin_derive("BU", 1, P, EKA, P)` → `BUyAt`
- [ ] `asirlin_derive("gam", 1, P, EKA, P)` → `gamyAt`
- [ ] `asirlin_derive("kf", 8, P, EKA, P)` → `kriyAt`
- [ ] All 9 ASIRLIN-P forms of bhū match oracle
- [ ] `make validate-phase3` still green
- [ ] ASIRLIN oracle reaches ≥ 60%
