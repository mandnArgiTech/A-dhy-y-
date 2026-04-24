# Story 5.4 — Taddhita Secondary Derivatives

**Phase:** 5 — Pipeline, Samāsa, Kṛt, Taddhita  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 5.1, 4.1

---

## Objective

Implement fifteen common taddhita suffixes — secondary nominal derivatives formed from prātipadika stems (not from verb roots). Governed by 4.1.76–5.4.160.

---

## Background

Taddhita suffixes (4.1.76: `taddhitāḥ`) create secondary derivatives expressing:
- Descent/lineage (e.g., *Gārgya* = "descendant of Garga")
- Abstract nouns (e.g., *śuklata* = "whiteness")
- Locative relation (e.g., *grāmīṇa* = "of the village")
- Possession (e.g., *dhanin* = "wealthy")
- Comparative/superlative (e.g., *śreyas*, *śreṣṭha*)

Key defining sūtra: 4.1.76 `taddhitāḥ` (these are called taddhita).

---

## The Fifteen Taddhita Suffixes to Implement

| # | Suffix SLP1 | IAST | Type | Example | Meaning |
|---|------------|------|------|---------|---------|
| 1 | aR | aṆ | Descent | garga + aṆ → gArgya | descendant of Garga |
| 2 | Ya | ya | Relation | vidyA + Ya → vEdya | relating to knowledge |
| 3 | in | in | Possession | Dana + in → DanIn | wealthy |
| 4 | mat | mat | Possession (a-less) | Dana + mat → Danamat | having wealth |
| 5 | tA | tā | Abstract (fem) | Sukla + tA → SuklatA | whiteness |
| 6 | tva | tva | Abstract (neut) | Sukla + tva → Suklatva | whiteness (neuter) |
| 7 | ka | ka | Diminutive | putra + ka → putraka | little son |
| 8 | Ika | ika | Relation | grAma + Ika → grAmIRa | of the village |
| 9 | IYa | īya | Relation | rAjan + IYa → rAjanIya | royal, fit for a king |
| 10 | tama | tama | Superlative | Sreyas + tama → SreyzWa (by phonological change) | best |
| 11 | tara | tara | Comparative | laghu + tara → laGutara | lighter |
| 12 | vat | vat | Simile | siMha + vat → siMhavat | like a lion |
| 13 | maya | maya | Made of | suvarna + maya → suvarNameya | made of gold |
| 14 | Ana | āna | Connected to | grIzma + Ana → grIzmARa | of summer |
| 15 | vya | vya | Fit for | yajYa + vya → yajYIya | fit for sacrifice |

---

## Key Sūtras

```
4.1.76  — taddhitāḥ — defines the class
4.1.83  — prāg dīvyato 'ṇ — aṆ (ṇ as it) suffixes
4.1.92  — tasminn iti nirdiṣṭe — location-based taddhitas
4.2.1   — (taddhitas in 4.2 are for relation to place)
5.1.119 — tena dīvyati khanatyupajīvati — game suffixes
5.2.94  — tadarham — worthy/fit suffixes  
5.2.94+ — tā tva abstract suffixes (5.1.119)
5.1.115 — tena nirmitam — made-of (maya) suffix
4.1.15  — inac (in suffix) — possession
4.2.67  — grāmajanabandhujñātisakhyeṣu — community relationships
```

---

## Phonological Changes Before Taddhita

### vṛddhi of first vowel (7.2.117)

Many taddhita suffixes beginning with ṇ-it cause vṛddhi of the first vowel:
- garga → Garga (g→G = a→ā, but actually it's a→ā for the first syllable)
  Actually: `a` in first syllable → `A` (ā)
- vidya → vaidya (vi→vai = i→ai, which is vṛddhi)
- grāma → grāmīṇa (no vṛddhi here — suffix is different)

Rule 7.2.117: `taddhiteṣvacanudātteṣv apy ajāder guṇasya` — before taddhita, first vowel gets vṛddhi.

### Stem changes before taddhita

Some stems undergo changes:
- Final `a` may be dropped before vowel-initial suffix (6.4.148)
- `in` suffix: preceding `a` drops (so `dhana + in → dhanin` not `dhanain`)
- `ā`-final feminine stems may require special handling

---

## Files to Create

### `prakriya/taddhita/taddhita.h`

```c
#ifndef TADDHITA_H
#define TADDHITA_H

#include "ashtadhyayi.h"
#include "context.h"

typedef enum {
  TD_AN = 1,     /* aṆ — descent */
  TD_YA,         /* ya — relation */
  TD_IN,         /* in — possession */
  TD_MAT,        /* mat — possession */
  TD_TA,         /* tā — abstract fem */
  TD_TVA,        /* tva — abstract neut */
  TD_KA,         /* ka — diminutive */
  TD_IKA,        /* ika — relation */
  TD_IYA,        /* īya — relation/fit */
  TD_TAMA,       /* tama — superlative */
  TD_TARA,       /* tara — comparative */
  TD_VAT,        /* vat — simile */
  TD_MAYA,       /* maya — made of */
  TD_AANA,       /* āna — connected to */
  TD_VYA,        /* vya — fit for */
} TaddhitaType;

/**
 * Derive a taddhita form.
 * @param db         Loaded database
 * @param base_slp1  Prātipadika stem (SLP1)
 * @param suffix     Taddhita suffix type
 * @return ASH_Form — caller must ash_form_free()
 */
ASH_Form taddhita_derive(const SutraDB *db, const char *base_slp1,
                          TaddhitaType suffix);

/** Does this suffix cause vṛddhi of the first vowel? (7.2.117) */
bool taddhita_causes_vrddhi(TaddhitaType suffix);

/** Name of taddhita type for logging */
const char *taddhita_type_name(TaddhitaType suffix);

#endif /* TADDHITA_H */
```

### `prakriya/taddhita/taddhita.c`

```c
ASH_Form taddhita_derive(const SutraDB *db, const char *base_slp1,
                          TaddhitaType suffix) {
  PrakriyaCtx ctx = {0};

  /* Step 1: Initialize base term */
  term_init(&ctx.terms[0], base_slp1, SJ_PRATIPADIKA);
  ctx.term_count = 1;

  /* Step 2: Apply vṛddhi to first vowel if suffix is ṇit (7.2.117) */
  if (taddhita_causes_vrddhi(suffix)) {
    char vrddhi_form[64];
    apply_vrddhi_to_first_syllable(base_slp1, vrddhi_form, sizeof(vrddhi_form));
    term_substitute(&ctx.terms[0], vrddhi_form, SUTRA_7_2_117);
    prakriya_log(&ctx, SUTRA_7_2_117, "vṛddhi of first vowel before taddhita");
  }

  /* Step 3: Handle stem-final phoneme changes */
  /* e.g., a-stems may drop final a before vowel-initial suffix */
  const char *suffix_upadesa = taddhita_suffix_upadesa(suffix);
  bool suffix_vowel_initial = varna_is_vowel(suffix_upadesa[0]);
  if (suffix_vowel_initial && term_final(&ctx.terms[0]) == 'a') {
    /* 6.4.148: final a drops before certain taddhitas */
    /* (only for specific suffixes — check the list) */
  }

  /* Step 4: Add suffix term */
  term_init(&ctx.terms[1], suffix_upadesa, SJ_TADDHITA | SJ_PRATYAYA);
  ctx.term_count = 2;

  /* Step 5: Strip anubandhas */
  AnubandhaResult ar;
  anubandha_strip(suffix_upadesa, SJ_TADDHITA, &ar);
  term_substitute(&ctx.terms[1], ar.clean_slp1, 0);

  /* Step 6: Apply sandhi */
  char joined[256];
  sandhi_vowel_join(ctx.terms[0].value, ctx.terms[1].value,
                     joined, sizeof(joined));

  /* Step 7: Build result */
  return prakriya_build_result(&ctx);
}
```

---

## Expected Output Table

| Base SLP1 | Suffix | Expected Output SLP1 | IAST |
|-----------|--------|---------------------|------|
| garga | AN | gArgya | gārgya |
| vidyA | YA | vEdya | vaidya |
| Danamat | IN | DanIn | dhanin |
| Sukla | TA | SuklatA | śuklatā |
| Sukla | TVA | Suklatva | śuklatva |
| putra | KA | putraka | putraka |
| siMha | VAT | siMhavat | siṃhavat |
| suvarna | MAYA | suvarnImaya | (suvarṇamaya) |
| laghu | TARA | laGutara | laghutara |

---

## Unit Test: `tests/unit/test_taddhita.c`

```c
void test_an_garga(void) {
  ASH_Form f = taddhita_derive(&db, "garga", TD_AN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gArgya", f.slp1);
  ash_form_free(&f);
}

void test_ta_abstract(void) {
  ASH_Form f = taddhita_derive(&db, "Sukla", TD_TA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("SuklatA", f.slp1);
  ash_form_free(&f);
}

void test_in_possession(void) {
  ASH_Form f = taddhita_derive(&db, "Dana", TD_IN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("DanIn", f.slp1);
  ash_form_free(&f);
}

void test_tara_comparative(void) {
  ASH_Form f = taddhita_derive(&db, "laghu", TD_TARA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("laGutara", f.slp1);
  ash_form_free(&f);
}

void test_vat_simile(void) {
  ASH_Form f = taddhita_derive(&db, "siMha", TD_VAT);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("siMhavat", f.slp1);
  ash_form_free(&f);
}
```

---

## Acceptance Criteria

- [ ] All 15 taddhita types implemented
- [ ] vṛddhi of first vowel applied correctly for ṇit suffixes
- [ ] Stem-final `a` dropped before vowel-initial suffixes where required
- [ ] Expected output table spot-checks pass (8/9 at minimum)
- [ ] Each form logged with governing sūtra ID (4.1.76 + specific)
- [ ] Unit test 0 failures
