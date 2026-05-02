# Story 4.9 — ā-stem feminine paradigm (rāmā / latā)

**Phase:** 4 — Subanta Declension
**Difficulty:** Medium
**Estimated time:** 3 hours
**Depends on:** Story 4.8 (`feminine_stems.c` infrastructure)

---

## Objective

The legacy `aa_stem_fem_derive` covers only 3-4 vibhakti slots and emits
forms that don't match the oracle (e.g. `rAmyA` for tṛtīyā-eka, where
the oracle says `ramayA`). This story adds an `aa_stem_fem_full` helper
with the complete 24-slot paradigm and wires it into the runtime
dispatcher.

---

## Background — paradigm (SLP1) for `ramA`

| Vib | Eka | Dvi | Bahu |
|-----|-----|-----|------|
| 1 | ramA | rame | ramAH |
| 2 | ramAm | rame | ramAH |
| 3 | ramayA | ramAByAm | ramABiH |
| 4 | ramAyE | ramAByAm | ramAByaH |
| 5 | ramAyAH | ramAByAm | ramAByaH |
| 6 | ramAyAH | ramayoH | ramARAm |
| 7 | ramAyAm | ramayoH | ramAsu |
| 8 | rame | rame | ramAH |

(For `latA`, ṣaṣṭhī-bahu = `latAnAm` — no ṇ because no triggering
r/f/z/F/R in stem.)

---

## Files

### `prakriya/subanta/feminine_stems.c` (extension)

Add a new 24-slot table `AA_FEM[24]` and `aa_stem_fem_full()` helper
following the existing kernel pattern (drop stem-final A, append slot
ending, apply 8.4.1 ṇatva).

### `prakriya/subanta/aaiu_stems.h`

Add prototype:
```c
bool aa_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
```

### `prakriya/pipeline.c`

Add a route before the legacy `aa_stem_fem_can_handle`:
```c
} else if (li == ASH_STRI && last == 'A') {
  ok = aa_stem_fem_full(normalized, vib, v, &ctx);
} else if (...)
```

---

## Acceptance Criteria

- [ ] `aa_stem_fem_full("ramA", PRATHAMA, EKA)` → `ramA`
- [ ] `aa_stem_fem_full("ramA", PRATHAMA, DVI)` → `rame`
- [ ] `aa_stem_fem_full("ramA", TRITIYA, EKA)` → `ramayA`
- [ ] `aa_stem_fem_full("ramA", CATURTHI, EKA)` → `ramAyE`
- [ ] `aa_stem_fem_full("ramA", SHASTHI, BAHU)` → `ramARAm` (ṇ via 8.4.1)
- [ ] `aa_stem_fem_full("latA", SHASTHI, BAHU)` → `latAnAm` (no ṇ)
- [ ] `aa_stem_fem_full("ramA", SAMBODHANA, EKA)` → `rame`
- [ ] All 27 existing unit tests still pass
- [ ] Subanta oracle climbs further on the 1200-row sample
