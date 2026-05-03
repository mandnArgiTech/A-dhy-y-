# Story 3.22 — VIDHILIN (optative / potential mood)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Medium-High
**Estimated time:** 5 hours
**Depends on:** Story 3.16 (LAT 100%)

---

## Objective

Implement vidhi-liṅ (optative, "expressing wish/possibility"). Vidhi-
liṅ inserts the **`yāsuṭ`** augment (realised as `yā` in singular
forms, `ī` in dual/plural) between vikaraṇa and ending.

Examples:
- `bhū` VIDHILIN prathama-eka-P → `bhavet` (= bhav + e + t, with sap-a + iyat collapsed)
- `gam` VIDHILIN prathama-eka-P → `gacchet`
- `kṛ` VIDHILIN prathama-eka-P → `kuryāt` (athematic; gaṇa-8)

---

## Background

Steps (3.3.161 → 3.4.103 → 7.2.79):
1. 3.3.161 vidhinimantraṇāmantraṇādhīṣṭasaṃpraśnaprārthaneṣu liṅ
2. 3.4.103 yāsuṭ pareṣmaipadeṣu udāttaḥ — yāsuṭ augment for parasmaipada
3. 3.4.105 jheryur — jhi → ur in liṅ
4. 7.2.79 lopo vyor vali — drop y/v before vali (consonant cluster)
5. 7.2.80 ato yeyaḥ — replace certain vowels with ya/iya
6. Standard secondary endings (same as LAT but with -t replacing -ti etc.)

For thematic gaṇas: stem + i + t = stem-e + t (after savarṇa-dīrgha
or guna). For bhū: bhava + ī + t → bhavet.

For athematic gaṇas: stem + yā + t = stem + yāt. For kṛ (gaṇa-8):
ku + ru + yā + t → kuryāt.

---

## Files

### `prakriya/tinanta/vidhilin.h` / `vidhilin.c` (new, ~250 lines)

```c
bool vidhilin_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                         ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline:
1. Build the present-tense stem (same as LAT for thematic, with
   athematic strong/weak for gaṇa-2/3/5/7/8/9).
2. Insert yāsuṭ augment: `yA` in singular, `I` in dual/plural.
3. Apply secondary tiṅ endings (t/tām/ur etc.).
4. Apply boundary sandhi (e.g. yA + t → yat, then 7.3.101-style if needed).

### `prakriya/tinanta/lakara.c`
Add `VIDHILIN_PARASMAI[9]` table with secondary endings (t, tām, ur,
s, tam, ta, am, va, ma).

---

## Acceptance Criteria

- [ ] `vidhilin_derive("BU", 1, P, EKA, P)` → `Bavet`
- [ ] `vidhilin_derive("gam", 1, P, EKA, P)` → `gacCet`
- [ ] `vidhilin_derive("kf", 8, P, EKA, P)` → `kuryAt`
- [ ] `vidhilin_derive("BU", 1, P, BAHU, P)` → `BaveyuH`
- [ ] All 9 VIDHILIN-P forms of bhū match oracle
- [ ] `make validate-phase3` still green
- [ ] VIDHILIN oracle reaches ≥ 60%
