# Story 4.7 — Consonant-stem subanta paradigms

**Phase:** 4 — Subanta Declension
**Difficulty:** Very High
**Estimated time:** 8 hours
**Depends on:** Story 4.6, BUG-009

---

## Objective

Consonant-stem nouns (rājan, ātman, vāc, marut, manas, viśva-pā, …) are
phonologically the most rule-rich part of the subanta system. The current
`consonant_stems.c` implementation hard-codes only three vibhakti slots
for n-stems, as-stems, and ṛ-stems. This story replaces those stubs with
a proper rule pipeline covering the four major consonant-stem patterns:

- **n-stems** (rājan, ātman, brahman) — 8.2.7 nalopaḥ at pada boundary,
  6.4.8 sarvanāmasthāne lengthening
- **as-stems** (manas, śiras, payas) — 8.2.66 sasajuṣo ruḥ + 8.3.15 visarga
- **ṛ-stems** (pitṛ, mātṛ, dātṛ) — 7.1.93–94 ūṅ guṇa, 7.1.96 ṛ→ar
- **multi-consonant stems** (vāc, marut, suhṛd) — 8.2.30 corcuḥ, 8.2.39 jhalāṃ jaśo'nte

---

## Background

For an n-stem like `rājan`:

| Vib | Eka | Dvi | Bahu |
|-----|-----|-----|------|
| 1 | rājā | rājānau | rājānaḥ |
| 2 | rājānam | rājānau | rājñaḥ |
| 3 | rājñā | rājabhyām | rājabhiḥ |
| 4 | rājñe | rājabhyām | rājabhyaḥ |
| 5 | rājñaḥ | rājabhyām | rājabhyaḥ |
| 6 | rājñaḥ | rājñoḥ | rājñām |
| 7 | rājñi | rājñoḥ | rājasu |
| 8 | rājan | rājānau | rājānaḥ |

Notable rule firings:
- 6.4.8 sarvanāmasthāne ca: vowel of upadha lengthens before
  sarvanāmasthāna suffixes (PRATHAMA, DVITIYA EKA/DVI/BAHU; SAMBODHANA EKA)
- 8.2.7 nalopaḥ prātipadikāntasya: the final `n` is dropped before pada-
  level boundary in PRATHAMA EKA → `rājā`
- 6.4.134 allopo'naḥ: the `a` of the rāja(n) base is elided before
  vowel-initial suffixes in some forms (giving `rājñaḥ`)

For as-stems like `manas`:
- prathama-eka: `manaḥ` (final s → visarga via 8.2.66 + 8.3.15)
- tṛtīyā-eka: `manasā`
- All forms with consonant-initial suffix: stem stays as `manas`/`mano`/`manaḥ`
  depending on sandhi context

---

## Files

### `prakriya/subanta/consonant_stems.c` (rewrite)

Split into per-pattern helper modules and a dispatch table.

```c
typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *suffix_clean;
  uint32_t     primary_sutra;
  bool         is_sarvanamasthana;
} ConsonantSlot;

static const ConsonantSlot CONS_SLOTS[24] = { ... };

static bool n_stem_derive_full(const char *stem, ASH_Linga li,
                                ASH_Vibhakti vib, ASH_Vacana vac,
                                PrakriyaCtx *ctx);

static bool as_stem_derive_full(const char *stem, ASH_Linga li,
                                 ASH_Vibhakti vib, ASH_Vacana vac,
                                 PrakriyaCtx *ctx);

static bool r_stem_derive_full(const char *stem, ASH_Linga li,
                                ASH_Vibhakti vib, ASH_Vacana vac,
                                PrakriyaCtx *ctx);

static bool generic_consonant_stem_derive(const char *stem, ASH_Linga li,
                                           ASH_Vibhakti vib, ASH_Vacana vac,
                                           PrakriyaCtx *ctx);
```

The pipeline for each derivation:

1. Detect stem class (n, as, ṛ, generic) via final-syllable test
2. Select suffix from the slot table
3. Apply per-class transformations:
   - n-stem: 6.4.8 lengthening for sarvanāmasthāna; 8.2.7 nalopaḥ for
     prathama-eka; 6.4.134 a-elision for vowel-initial suffixes
   - as-stem: 8.2.66 + 8.3.15 visarga for non-vowel-initial suffixes;
     stem-final s preserved before vowel-initial suffixes
   - ṛ-stem: 7.1.96 (ṛ → ar) before sarvanāmasthāna; 7.3.110 ṛto ṅi-sarvanāma-
     sthānayoḥ (lengthening); 7.1.93 ūṅ
4. Concatenate stem + suffix
5. Apply cross-junction sandhi (consonant assimilation, visarga rules)
6. Final-form sandhi: 8.2.66 + 8.3.15 if final s remains

### Test data: `tests/data/consonant_paradigms.tsv`

```
stem	linga	vib	vac	expected_slp1	sutra
rAjan	M	1	E	rAjA	802007
rAjan	M	1	D	rAjAnO	604008
rAjan	M	1	B	rAjAnaH	604008
rAjan	M	2	E	rAjAnam	604008
rAjan	M	3	E	rAjYA	604134
manas	N	1	E	manaH	802066
manas	N	3	E	manasA	304002
pitf	M	1	E	pitA	703110
pitf	M	1	D	pitarO	701096
...
```

Loaded by the test harness to verify derivation matches.

---

## Test Cases

```c
void test_n_stem_rajan_full(void) {
  /* Verify all 24 rājan forms against tests/data/consonant_paradigms.tsv */
  ...
}

void test_as_stem_manas_full(void) {
  /* Verify all 24 manas forms */
  ...
}

void test_r_stem_pitr_full(void) {
  /* Verify all 24 pitṛ forms */
  ...
}
```

---

## Acceptance Criteria

- [ ] `n_stem_derive_full("rAjan", M, PRATHAMA, EKA)` → `rAjA` (with 8.2.7 traced)
- [ ] `n_stem_derive_full("rAjan", M, TRITIYA, EKA)` → `rAjYA` (with 6.4.134 traced)
- [ ] `as_stem_derive_full("manas", N, PRATHAMA, EKA)` → `manaH` (with 8.2.66 traced)
- [ ] `r_stem_derive_full("pitf", M, PRATHAMA, DVI)` → `pitarO` (with 7.1.96 traced)
- [ ] All 24 forms of rājan, manas, pitṛ produce exact-match SLP1
- [ ] All 27 existing tests still pass
- [ ] Subanta oracle rate climbs ≥ 35% on the 1200-row sample

---

## Notes

- 6.4.134 allopo'naḥ is one of the trickier rules — it deletes the `a`
  of the upadha when followed by a vowel-initial Sup ending. Implement
  it as a specific transformation step rather than via a general
  vowel-fusion engine.
- Keep each derivation function under 100 lines; split per stem class.
- Do not attempt to handle adverb / pronoun / numeral consonant stems
  here — those are Phase-4.x extensions.
