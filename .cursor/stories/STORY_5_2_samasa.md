# Story 5.2 — Samāsa (Compound Formation)

**Phase:** 5 — Pipeline, Samāsa, Kṛt, Taddhita  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 5.1

---

## Objective

Implement the six types of Sanskrit compounds (samāsa). Compound formation is governed by 2.1.3–2.4.71, with individual rules for each compound type.

---

## Background: The Six Samāsa Types

| Type | Sanskrit | Key Sūtra | Example | Meaning |
|------|---------|-----------|---------|---------|
| Tatpuruṣa | tatpuruṣa | 2.1.22 | rāja-puruṣa | "king's man" |
| Karmadhāraya | karmadhāraya | 2.1.57 | nīla-utpala | "blue lotus" |
| Bahuvrīhi | bahuvrīhi | 2.2.23 | datta-dhana | "one who has given wealth" |
| Dvandva | dvandva | 2.2.29 | rāma-kṛṣṇau | "Rāma and Kṛṣṇa" |
| Avyayībhāva | avyayībhāva | 2.1.6 | upagaṅgam | "near the Gaṅgā" |
| Dvigu | dvigu | 2.1.52 | pañca-gavī | "a group of 5 cows" |

---

## Compound Formation Rules

### General process (all types):

1. Both members take their `prātipadika` (stem) form — strip vibhakti (2.4.71)
2. Apply internal sandhi at the junction
3. Add a final vibhakti suffix appropriate to the compound type
4. If bahuvrīhi: the compound functions as an adjective, takes linga of qualified noun

### Type-specific rules:

**Tatpuruṣa (2.1.22):** dependent member + head member. The first member's case meaning is recovered from context. Output = head in prātipadika + appropriate case suffix for the whole compound.

**Karmadhāraya (2.1.57):** same-referent members. Adjective + noun. Like tatpuruṣa but both members have the same referent.

**Bahuvrīhi (2.2.23):** neither member is the head — the compound describes an external noun. Final member in prātipadika form, compound declines to match its referent's linga.

**Dvandva (2.2.29):** coordinate compound. Both members in prātipadika, final form reflects the total number. Two members → dual.

**Avyayībhāva (2.1.6):** first member is an avyaya (indeclinable). Output is always neuter singular accusative form.

**Dvigu (2.1.52):** first member is a numeral. Output is usually feminine (group noun).

---

## Files to Create

### `samasa/samasa.h`

```c
#ifndef SAMASA_H
#define SAMASA_H

#include "ashtadhyayi.h"
#include "context.h"
#include "sutra.h"

typedef struct {
  char purva_stem[64];    /* First member stem (SLP1, prātipadika form) */
  char uttara_stem[64];   /* Second member stem (SLP1, prātipadika form) */
  ASH_SamasaType type;
  ASH_Linga      linga;   /* Linga of referent (for bahuvrīhi) */
  /* Derivation log */
  uint32_t rules_applied[16];
  int      rule_count;
} SamasaCtx;

/**
 * Form a compound from two stems.
 * @param db            Loaded database
 * @param purva_slp1    First member stem (in prātipadika form, SLP1)
 * @param uttara_slp1   Second member stem (in prātipadika form, SLP1)
 * @param type          Compound type
 * @param linga         Gender of the resulting compound
 * @param ctx_out       Optional: output derivation context
 * @return ASH_Form with the compound form
 */
ASH_Form samasa_derive(const SutraDB *db,
                        const char *purva_slp1, const char *uttara_slp1,
                        ASH_SamasaType type, ASH_Linga linga,
                        SamasaCtx *ctx_out);

/**
 * Attempt to split a compound form into its components.
 * @param compound_slp1  The compound in SLP1 (stem form)
 * @param splits_out     Output: array of (purva, uttara, type) structs
 * @param max_splits     Maximum number of analyses to return
 * @return Number of analyses found
 */
typedef struct {
  char           purva[64];
  char           uttara[64];
  ASH_SamasaType type;
  float          confidence;  /* 0.0–1.0 */
} SamasaSplit;

int samasa_split(const SutraDB *db, const char *compound_slp1,
                 SamasaSplit *splits_out, int max_splits);

/** Get the traditional name of a samāsa type (SLP1) */
const char *samasa_type_name(ASH_SamasaType type);

#endif /* SAMASA_H */
```

### `samasa/samasa.c`

Implement each compound type as a separate static function:

```c
static ASH_Form derive_tatpurusha(const SutraDB *db,
                                    const char *purva, const char *uttara,
                                    ASH_Linga linga);
static ASH_Form derive_bahuvrihi(const SutraDB *db,
                                   const char *purva, const char *uttara,
                                   ASH_Linga linga);
static ASH_Form derive_dvandva(const SutraDB *db,
                                  const char *purva, const char *uttara,
                                  ASH_Linga linga);
/* ... */
```

The core operation for all types:
1. Take prātipadika of purva member (for most types: no suffix)
2. Apply sandhi at purva+uttara junction (call `sandhi_vowel_join` or `sandhi_hal_join`)
3. The result is the compound prātipadika
4. This can then be passed to `ash_subanta` for full declension

---

## Test Cases

### Tatpuruṣa
```
rAja + puruzwa → rAjapuruzwa (king's man)
rAma + Adeza → rAmAdeSa (Rāma's command) — vowel sandhi at junction
```

### Karmadhāraya
```
nIla + utpala → nIlotpala (blue lotus) — a+u → o sandhi
maha + rAja → mahArAja (great king) — a+r → ā sandhi (dirgha)
```

### Bahuvrīhi
```
pIta + aMbara → pItAMbara (yellow-clad, epithet of Viṣṇu) — declines as masc adj
```

### Dvandva
```
rAma + kfzRa → rAmakfzRO (Rāma and Kṛṣṇa) — dual form
```

### Avyayībhāva
```
upa + gaNgA → upagaNgam (near the Gaṅgā) — always neuter sing acc
```

---

## Unit Test: `tests/unit/test_samasa.c`

```c
void test_tatpurusha_basic(void) {
  ASH_Form f = samasa_derive(&db, "rAja", "puruzwa",
                               ASH_SAMASA_TATPURUSHA, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAjapuruzwa", f.slp1);
  ash_form_free(&f);
}

void test_karmadhaaraya_sandhi(void) {
  /* nīla + utpala: a + u → o (6.1.87 guṇa) */
  ASH_Form f = samasa_derive(&db, "nIla", "utpala",
                               ASH_SAMASA_KARMADHAARAYA, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("nIlotpala", f.slp1);
  ash_form_free(&f);
}

void test_dvandva_dual(void) {
  /* rāma + kṛṣṇa → rāmakṛṣṇau (nom dual) */
  ASH_Form f = samasa_derive(&db, "rAma", "kfzRa",
                               ASH_SAMASA_DVANDVA, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAmakfzRO", f.slp1);
  ash_form_free(&f);
}

void test_avyayibhava(void) {
  ASH_Form f = samasa_derive(&db, "upa", "gaNgA",
                               ASH_SAMASA_AVYAYIBHAVA, ASH_NAPUMSAKA, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("upagaNgam", f.slp1);
  ash_form_free(&f);
}
```

---

## Acceptance Criteria

- [ ] All 6 compound types return valid `ASH_Form`
- [ ] Tatpuruṣa: correct junction sandhi applied
- [ ] Karmadhāraya: `nīla + utpala → nīlotpala`
- [ ] Dvandva: nom dual form correct
- [ ] Avyayībhāva: always neuter acc sg
- [ ] Each compound's prakriyā trace includes the governing sūtra (2.1.x)
- [ ] Unit test 0 failures
