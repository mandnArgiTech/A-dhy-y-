# Story 4.11 — in-stems and vat/mat-stems

**Phase:** 4 — Subanta Declension
**Difficulty:** Medium
**Estimated time:** 4 hours
**Depends on:** Story 4.7 (consonant-stem framework)

---

## Objective

Two pervasive consonant-stem patterns dominate the remaining Phase-4
oracle gap:

- **in-stems** (`guRin`, `tundin`, `vAggmin`, `Datin`) — possessive
  suffix `-in`. ~ 100 stems in the lexicon.
- **at-stems / vat-stems / mat-stems** (`SfRvat`, `BagavatI`,
  `DImat`, `mahat`) — present participle and possessive forms.

Adding these two helpers should close ~25-30% of the remaining
consonant-stem misses.

---

## Background

`guRin` (PUMS) — drop final `n` and use one of three bases per slot:

- LONG_I (`guRI`): prathama-eka only — n drops by 8.2.7, the i lengthens
  per 6.4.13 in-saṃjñā/sarvanāmasthāna context.
- WEAK_FULL (`guRin`): used before vowel-initial endings.
- WEAK_DROP_N (`guRi`): used before consonant-initial endings (Bi-, Bya-,
  zu — the latter via 8.3.59 i-ad-style ṣatva).

`SfRvat` (PUMS, present-participle vat-stem) — four bases:

- STRONG (`SfRvant`): sarvanāmasthāna with `nt` augment per 7.1.70
  ugidacāṃ sarvanāmasthāne'dhātoḥ.
- VOICED (`SfRvad`): before voiced consonant suffixes (ByAm, BiH, ByaH)
  per 8.2.39 jhalāṃ jaśo'nte.
- N_FINAL (`SfRvan`): prathama-eka and sambodhana-eka — t → n by
  8.2.66 sasajuṣo ru and then n preservation.
- WEAK (`SfRvat`): elsewhere.

---

## Files

### `prakriya/subanta/consonant_stem_full.c` (extension)

Add two new helpers + slot tables alongside the existing an/as/ṛ work:

```c
bool in_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib,
                       ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool vat_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);
```

### `prakriya/pipeline.c`

Route PUMS + "in"-final → `in_stem_masc_full` and PUMS + "at"/"vat"/"mat"
final → `vat_stem_masc_full`, before the legacy fallbacks.

---

## Acceptance Criteria

- [ ] `in_stem_masc_full("guRin", PRATHAMA, EKA)` → `guRI`
- [ ] `in_stem_masc_full("guRin", PRATHAMA, DVI)` → `guRinO`
- [ ] `in_stem_masc_full("guRin", TRITIYA, DVI)` → `guRiByAm`
- [ ] `in_stem_masc_full("guRin", SAPTAMI, BAHU)` → `guRizu`
- [ ] `vat_stem_masc_full("SfRvat", PRATHAMA, EKA)` → `SfRvan`
- [ ] `vat_stem_masc_full("SfRvat", PRATHAMA, DVI)` → `SfRvantO`
- [ ] `vat_stem_masc_full("SfRvat", TRITIYA, DVI)` → `SfRvadByAm`
- [ ] All 27 unit tests still pass
- [ ] Subanta oracle climbs further on the 1200-row sample
