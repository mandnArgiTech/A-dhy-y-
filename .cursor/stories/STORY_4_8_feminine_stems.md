# Story 4.8 — Feminine ī/ū stem and short-vowel feminine paradigms

**Phase:** 4 — Subanta Declension
**Difficulty:** High
**Estimated time:** 6 hours
**Depends on:** Story 4.6 (i/u-stem masc/neut), BUG-009

---

## Objective

Story 4.6 covered i/u-stem masc and neut. This story extends the
declension engine to **feminine vowel-stem paradigms**:

- Long ī-feminines (`nadI`, ñīp-type) — 24-slot paradigm
- Long ū-feminines (`vaDU`, ūṅ-type) — 24-slot paradigm
- Short i-feminines (`mati`) — 24-slot paradigm
- Short u-feminines (`Denu`) — 24-slot paradigm

After this story the runtime subanta dispatcher routes feminine i/u/ī/ū
stems to oracle-matching paradigms and the Phase-4 oracle climbs
proportionally to the share of feminine stems in the sample.

---

## Background — paradigms (SLP1)

`nadI` (long-ī feminine, ñīp-type — `s` of prathama-eka drops):

| Vib | Eka | Dvi | Bahu |
|-----|-----|-----|------|
| 1 | nadI | nadyO | nadyaH |
| 2 | nadIm | nadyO | nadIH |
| 3 | nadyA | nadIByAm | nadIBiH |
| 4 | nadyE | nadIByAm | nadIByaH |
| 5 | nadyAH | nadIByAm | nadIByaH |
| 6 | nadyAH | nadyoH | nadInAm |
| 7 | nadyAm | nadyoH | nadIzu |
| 8 | nadi | nadyO | nadyaH |

`vaDU` (long-ū feminine, ūṅ-type — `s` of prathama-eka surfaces as H):

| Vib | Eka | Dvi | Bahu |
|-----|-----|-----|------|
| 1 | vaDUH | vaDvO | vaDvaH |
| 2 | vaDUm | vaDvO | vaDUH |
| 3 | vaDvA | vaDUByAm | vaDUBiH |
| ... | ... | ... | ... |

`mati` (short-i feminine):
- Identical to agni masc except dvitīyā-bahu = `matIH` (with visarga, not `matIn`)
  and caturthi/pancami/shasthi/saptami eka variants.

`Denu` (short-u feminine): parallel to mati with i↔u, e↔o, ay↔av.

---

## Files

### `prakriya/subanta/feminine_stems.c` (new)

Three or four 24-slot tables, plus per-paradigm derivation kernel
following Story 4.6's pattern (drop stem-final vowel, append slot
ending, apply 8.4.1 ṇatva, apply 8.2.66+8.3.15 visarga).

```c
bool ii_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool uu_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool i_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                     ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool u_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                     ASH_Vacana vac, PrakriyaCtx *ctx_out);
```

Wire into `prakriya/pipeline.c::pipeline_subanta` so STRI + i/I/u/U
final routes through the new helpers before legacy stubs.

---

## Acceptance Criteria

- [ ] `ii_stem_fem_full("nadI", PRATHAMA, EKA)` → `nadI`
- [ ] `ii_stem_fem_full("nadI", TRITIYA, EKA)` → `nadyA`
- [ ] `ii_stem_fem_full("nadI", CATURTHI, EKA)` → `nadyE`
- [ ] `uu_stem_fem_full("vaDU", PRATHAMA, EKA)` → `vaDUH`
- [ ] `uu_stem_fem_full("vaDU", PRATHAMA, DVI)` → `vaDvO`
- [ ] `i_stem_fem_full("mati", PRATHAMA, EKA)` → `matiH`
- [ ] `i_stem_fem_full("mati", DVITIYA, BAHU)` → `matIH`
- [ ] `u_stem_fem_full("Denu", PRATHAMA, EKA)` → `DenuH`
- [ ] All 27 existing tests still pass
- [ ] Subanta oracle climbs further on the 1200-row sample

---

## Notes

- `nadI` and `vaDU` differ in prathama-eka: ñīp drops the s, ūṅ keeps
  it as visarga. This is a per-stem-class choice; encode it in the
  slot table.
- `mati` paradigm lacks a few hard rules (sambodhana-eka shortens to
  `mate`/`mati` per stem ending); pick one canonical form per oracle.
- The `nadInAm` (no ṇ) confirms that 8.4.1 doesn't fire when there is
  no triggering r/f/z/F/R in the stem — already handled by Story 4.6's
  `apply_natva` helper.
