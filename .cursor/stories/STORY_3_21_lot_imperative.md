# Story 3.21 — LOT (imperative)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Medium
**Estimated time:** 4 hours
**Depends on:** Story 3.16 (LAT 100%)

---

## Objective

Implement loṭ (imperative, "loṭ" 3.3.162). Loṭ uses the same
vikaraṇas as LAT but with a distinct ending table that includes
zero-ending in some slots (yielding bare-stem imperatives).

Examples:
- `bhū` LOT prathama-eka-P → `bhavatu`
- `bhū` LOT madhyama-eka-P → `bhava` (zero ending — bare stem!)
- `gam` LOT madhyama-eka-P → `gaccha`
- `bhū` LOT uttama-eka-P → `bhavāni`

---

## Background

Steps (3.3.162 → 3.4.86–96):
1. 3.3.162 loṭ ca — assigns loṭ
2. 3.4.86–96 — substitutes for loṭ tip/sip/mip etc.:
   - tip → tu (prathama-eka)
   - sip → "" (zero, bare stem; madhyama-eka)
   - mip → āni (uttama-eka, with augment)
   - tas → tām, jhi → antu, sis → ta, etc.
3. Vikaraṇas: same as LAT (sap, śyan, śa, ṇic).
4. Strong/weak: loṭ uttama is uniformly strong.

The 3.4.91 `seḥ hi-apit` rule: madhyama-eka uses Hi (which is
realised as zero in vowel-final stems, "hi" or "dhi" in
consonant-final). E.g. `BU` + Hi → `Bava` (zero); `ad` + Hi →
`addhi`.

---

## Files

### `prakriya/tinanta/lot.h` / `lot.c` (new, ~250 lines)

```c
bool lot_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                    ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline mirrors LAT but with the LOT_PARASMAI table substituted.
Madhyama-eka requires special zero-ending handling.

### `prakriya/tinanta/lakara.c`
Add `LOT_PARASMAI[9]` and `LOT_ATMANE[9]` tables:
- prathama: tu, tām, antu (P) / tām, etām, antām (Ā)
- madhyama: hi/zero, tam, ta (P) / sva, ethām, dhvam (Ā)
- uttama: āni, āva, āma (P) / ai, āvahai, āmahai (Ā)

---

## Acceptance Criteria

- [ ] `lot_derive("BU", 1, P, EKA, P)` → `Bavatu`
- [ ] `lot_derive("BU", 1, M, EKA, P)` → `Bava`
- [ ] `lot_derive("BU", 1, U, EKA, P)` → `BavAni`
- [ ] `lot_derive("gam", 1, P, EKA, P)` → `gacCatu`
- [ ] `lot_derive("ad", 2, M, EKA, P)` → `aDDi` (consonant-final + hi)
- [ ] All 9 LOT-P forms of bhū match oracle
- [ ] `make validate-phase3` still green
- [ ] LOT oracle reaches ≥ 75%
