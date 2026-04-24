# Story 4.1 — Vibhakti (Case) Suffix Table

**Phase:** 4 — Subanta (Nominal Declension)  
**Difficulty:** Medium  
**Estimated time:** 2.5 hours  
**Depends on:** Story 3.1, 2.3

---

## Objective

Define the complete sUP suffix table — all 24 nominal endings (7 vibhaktis × 3 vacanas + vocative) for all three genders — used in subanta (nominal) derivation.

---

## Background

Sūtra 4.1.2 defines: `sUP` — the group of 21 nominal endings (traditionally counted without vocative duplicates). The full set with Pāṇinian upadesa forms:

### Masculine/Neuter sUP table (prātipadika takes these)

| Vibhakti | Ekavacana | Dvivacana | Bahuvacana |
|---------|-----------|-----------|------------|
| Prathama | su | O | jas |
| Dvitīya | am | Owam | Sas |
| Tṛtīyā | wA | ByAm | Bis |
| Caturthī | Ne | ByAm | Byas |
| Pañcamī | Nasi | ByAm | Byas |
| Ṣaṣṭhī | Nas | os | Am |
| Saptamī | Ni | os | sup |
| Sambodhana | su | O | jas |

(These are upadesa forms with it-markers; stripping will clean them.)

For feminine, the paradigm differs slightly — feminine sUP endings.

---

## Files

### `prakriya/subanta/vibhakti.h`

```c
#ifndef VIBHAKTI_H
#define VIBHAKTI_H

#include "ashtadhyayi.h"
#include "term.h"

typedef struct {
  const char  *upadesa_slp1;
  const char  *clean_slp1;
  ASH_Vibhakti vibhakti;
  ASH_Vacana   vacana;
  ASH_Linga    linga;    /* ASH_PUMS, ASH_STRI, ASH_NAPUMSAKA, or all */
  Samjna       samjna;   /* SJ_SUP + any it-based flags */
} SupEntry;

/** Get the sUP suffix for a given vibhakti + vacana + linga */
const SupEntry *sup_get(ASH_Vibhakti vib, ASH_Vacana vac, ASH_Linga lin);

/** Add the appropriate sUP suffix to a derivation context */
int sup_assign(PrakriyaCtx *ctx, ASH_Vibhakti vib, ASH_Vacana vac, ASH_Linga lin);

#endif
```

### `prakriya/subanta/vibhakti.c`

Implement as a static const table — 8 vibhaktis × 3 vacanas × 3 genders = 72 entries (many shared).

---

## Acceptance Criteria

- [ ] All 72 entries defined (or shared appropriately)
- [ ] `sup_get(PRATHAMA, EKAVACANA, PUMS)` → upadesa="su"
- [ ] `sup_get(DVITIYA, BAHUVACANA, PUMS)` → upadesa="Sas"
- [ ] Unit test covers all 8 vibhaktis for masculine

---
---

# Story 4.2 — a-Stem Masculine Declension (rāma type)

**Phase:** 4 — Subanta  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 4.1, 1.4, 1.5

---

## Objective

Implement the full 24-form declension paradigm for a-stem masculine nouns (the largest class in Sanskrit — words like rāma, deva, vṛkṣa).

---

## Key Sūtras for a-stem masculine

```
7.1.9   — ato 'm — nominative singular: a + su → as (then visarga)
7.1.12  — tasminn apy āmantrite (vocative = nominative sg)
6.1.101 — akaḥ savarṇe dīrghaḥ (long ā in certain positions)
6.1.87  — ādguṇaḥ (a + vowel suffix → guṇa)
7.3.102 — supi ca — guṇa before certain sup endings
6.4.14  — atvam — stem retains a before certain suffixes
8.2.7   — nalopaḥ prātipadikāntasya — final n of stem drops
3.1.44  — (lopa of vibhakti in certain compounds)
```

---

## Expected Paradigm for rāma (rAma):

| Vibhakti | Ekavacana | Dvivacana | Bahuvacana |
|---------|-----------|-----------|------------|
| Prathama | rāmaḥ (rAmaH) | rāmau (rAmO) | rāmāḥ (rAmAH) |
| Dvitīya | rāmam (rAmam) | rāmau (rAmO) | rāmān (rAmAn) |
| Tṛtīyā | rāmeṇa (rAmeRa) | rāmābhyām (rAmAByAm) | rāmaiḥ (rAmEH) |
| Caturthī | rāmāya (rAmAya) | rāmābhyām | rāmebhyaḥ (rAmeByas) |
| Pañcamī | rāmāt (rAmAt) | rāmābhyām | rāmebhyaḥ |
| Ṣaṣṭhī | rāmasya (rAmasya) | rāmayoḥ (rAmayoH) | rāmāṇām (rAmARAm) |
| Saptamī | rāme (rAme) | rāmayoḥ | rāmeṣu (rAmewu) |
| Sambodhana | rāma (rAma) | rāmau | rāmāḥ |

---

## Files

### `prakriya/subanta/a_stem.h`

```c
bool a_stem_masc_derive(const SutraDB *db, const char *stem_slp1,
                         ASH_Vibhakti vib, ASH_Vacana vac,
                         PrakriyaCtx *ctx_out);

bool a_stem_masc_paradigm(const SutraDB *db, const char *stem_slp1,
                           ASH_Form forms[24]);
```

Each step must log the sūtra that caused the change.

---

## Validation

```bash
python3 tools/run_scl_oracle.py --stem rAma --linga PUMS
```

Compare all 24 forms. Must match ≥ 22/24.

---

## Unit Test: `tests/unit/test_a_stem.c`

```c
void test_rama_nom_sg(void) {
  PrakriyaCtx ctx;
  a_stem_masc_derive(&db, "rAma", ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx);
  char form[64]; prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmaH", form);
}

void test_rama_acc_pl(void) {
  PrakriyaCtx ctx;
  a_stem_masc_derive(&db, "rAma", ASH_DVITIYA_VIB, ASH_BAHUVACANA, &ctx);
  char form[64]; prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmAn", form);
}
```

Create `tests/data/subanta_a_stem_expected.tsv` with all 24 expected SLP1 forms for rāma, deva, vṛkṣa.

---

## Acceptance Criteria

- [ ] All 24 forms of rāma correct per expected TSV
- [ ] All 24 forms of deva correct
- [ ] scl oracle ≥ 22/24 match
- [ ] Each step logged with sūtra ID

---
---

# Story 4.3 — ā-stem Feminine, i/ī-stems, u/ū-stems

**Phase:** 4 — Subanta  
**Difficulty:** High  
**Estimated time:** 5 hours  
**Depends on:** Story 4.2

---

## Objective

Implement declension for the most common non-a stems: ā-stem feminine (ramā type), i-stems (kavi/agni), ī-stems (nadī), and u/ū-stems (madhu/guru).

---

## Key Sūtras

```
ā-stems:
  7.3.105 — āṭ ca (vocative shortens ā to a in some cases)
  6.1.101 — savarṇadīrgha (ā + ā → ā)

i-stems (kavi):
  7.3.108 — iyaṅ upadhāyāḥ — stem vowel i→iy before vowel suffix
  7.3.109 — (special forms for loc. sg)
  6.4.14  — ata upadhāyāḥ

ī-stems (nadī):
  6.4.77  — aci vibhaktau — ī drops before vowel suffix
  7.3.111 — gherṅiti — ī→iy before ṅit suffix

u-stems (madhu/guru):
  7.3.110 — uvaṅ sphoṭāyanasya — u→uv before vowel
  7.3.84  — (guṇa before sārvadhatuka — not applicable here, wrong context)
```

---

## Expected Forms (spot-check)

### ramā (rAmA) — ā-stem feminine

| Vibhakti | Ekavacana | Bahuvacana |
|---------|-----------|------------|
| Prathama | rāmā (rAmA) | rāmāḥ (rAmAH) |
| Dvitīya | rāmām (rAmAm) | rāmāḥ (rAmAH) |
| Tṛtīyā | rāmayā (rAmayA) | rāmābhiḥ (rAmABiH) |

### kavi (kavi) — i-stem masculine

| Vibhakti | Ekavacana | Bahuvacana |
|---------|-----------|------------|
| Prathama | kaviḥ (kaviH) | kavayaḥ (kavayaH) |
| Dvitīya | kavim (kavim) | kavīn (kavIn) |
| Tṛtīyā | kavinā (kavinA) | kavibhiḥ (kaviBiH) |

---

## Files

### `prakriya/subanta/aaiu_stems.h`

```c
bool aa_stem_fem_derive(const SutraDB *db, const char *stem,
                         ASH_Vibhakti vib, ASH_Vacana vac,
                         PrakriyaCtx *ctx_out);

bool i_stem_derive(const SutraDB *db, const char *stem, ASH_Linga lin,
                    ASH_Vibhakti vib, ASH_Vacana vac, PrakriyaCtx *ctx_out);

bool u_stem_derive(const SutraDB *db, const char *stem, ASH_Linga lin,
                    ASH_Vibhakti vib, ASH_Vacana vac, PrakriyaCtx *ctx_out);
```

---

## Acceptance Criteria

- [ ] All 24 forms of ramā correct per expected TSV
- [ ] All 24 forms of kavi correct
- [ ] All 24 forms of nadī correct
- [ ] All 24 forms of madhu correct (neuter u-stem)
- [ ] scl oracle ≥ 21/24 match for each stem type

---
---

# Story 4.4 — Consonant Stems (n, at, as, ṛ finals)

**Phase:** 4 — Subanta  
**Difficulty:** Very High  
**Estimated time:** 5 hours  
**Depends on:** Story 4.3

---

## Objective

Implement the most complex nominal paradigms: consonant-final stems with significant alternation.

---

## Stems to Implement

### rājan (rAjan) — n-stem masculine

- "Strong" forms: nominative sg/du, accusative sg — stem = rājān (rAjAn)
- "Weak" forms: all others — stem = rājaṃ/rājan (rAjam/rAjan)
- Applies rule 8.2.7 (final n drops before consonant)

### bhavat (Bavat) — at/ant-stem (present participle)

- Strong: nom sg masc = bhavan (Bavan)
- Weak: nom-acc neuter = bhavat
- Complex alternation in oblique cases

### manas (manas) — as-stem neuter

- Visarga in nom/acc sg: manāḥ? → manaḥ
- s-deletion in some cases (8.3.16: virama)

### pitṛ (pitf) — ṛ-stem masculine (kinship)

- Nom sg: pitā
- Different behavior from regular ṛ-stems

---

## Key Sūtras

```
8.2.7  — nalopaḥ prātipadikāntasya — final n drops before consonant
7.1.70 — ugidac ca — n-stem special forms
7.1.72 — (rājan-type nominative sg)
8.3.16 — aḥ parasminn asati virama — final s/as at pause → ḥ
6.4.14 — ata upadhāyāḥ
```

---

## Unit Test Spot-checks

```c
/* rājan nom sg → rājā */
/* rājan acc sg → rājānam */
/* rājan inst pl → rājabhiḥ */
/* manas nom sg → manaḥ */
/* pitṛ nom sg → pitā */
```

---

## Acceptance Criteria

- [ ] rājan: all 24 forms with correct strong/weak alternation
- [ ] bhavat: nominative neuter = bhavat, masculine = bhavan
- [ ] manas: nom/acc sg = manaḥ, inst sg = manasā
- [ ] pitṛ: nom sg = pitā, gen sg = pituḥ
- [ ] scl oracle ≥ 19/24 for each (consonant stems are complex)

---
---

# Story 4.5 — Kāraka (Semantic Role) Resolver

**Phase:** 4 — Subanta  
**Difficulty:** Medium  
**Estimated time:** 2 hours  
**Depends on:** Story 4.1

---

## Objective

Implement the kāraka system — Pāṇini's semantic theory of grammatical roles that maps syntactic meaning to vibhakti (case) selection.

---

## Kāraka Rules (1.4.24–1.4.55)

| Kāraka | Sūtra | Meaning | Default Vibhakti |
|--------|-------|---------|-----------------|
| Kartṛ | 1.4.54 | Agent of action | Prathama (nominative) |
| Karman | 1.4.49 | Object of action | Dvitīya (accusative) |
| Karaṇa | 1.4.42 | Instrument | Tṛtīyā (instrumental) |
| Sampradāna | 1.4.32 | Recipient (for whom) | Caturthī (dative) |
| Apādāna | 1.4.24 | Source (from which) | Pañcamī (ablative) |
| Adhikaraṇa | 1.4.45 | Locus (where) | Saptamī (locative) |

Special:
- Possessor (sambandha) → Ṣaṣṭhī (genitive) — NOT a kāraka per Pāṇini (2.3.50)
- Vocative → Sambodhana

---

## Files

### `prakriya/subanta/karaka.h`

```c
#ifndef KARAKA_H
#define KARAKA_H

#include "ashtadhyayi.h"

/** Map a kāraka to its default vibhakti per Pāṇinian rules */
ASH_Vibhakti karaka_default_vibhakti(ASH_Karaka k);

/** Human-readable name of a kāraka */
const char *karaka_name(ASH_Karaka k);

/** Human-readable name of a vibhakti */
const char *vibhakti_name(ASH_Vibhakti v);

#endif
```

---

## Unit Test

```c
void test_karta_nominative(void) {
  TEST_ASSERT_EQUAL_INT(ASH_PRATHAMA_VIB, karaka_default_vibhakti(ASH_KARTA));
}
void test_karman_accusative(void) {
  TEST_ASSERT_EQUAL_INT(ASH_DVITIYA_VIB, karaka_default_vibhakti(ASH_KARMAN));
}
void test_karana_instrumental(void) {
  TEST_ASSERT_EQUAL_INT(ASH_TRITIYA_VIB, karaka_default_vibhakti(ASH_KARANA));
}
/* + 3 more */

void test_phase4_validate(void) {
  /* Integration: derive "rāmeṇa" (rāma + inst sg) and verify
     it correctly corresponds to kāraka KARANA */
  ASH_Vibhakti v = karaka_default_vibhakti(ASH_KARANA);
  TEST_ASSERT_EQUAL_INT(ASH_TRITIYA_VIB, v);
  /* Then derive rAma + TRITIYA + EKAVACANA */
  PrakriyaCtx ctx;
  a_stem_masc_derive(&db, "rAma", ASH_TRITIYA_VIB, ASH_EKAVACANA, &ctx);
  char form[64]; prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmeRa", form);
}
```

---

## Acceptance Criteria

- [ ] All 6 kāraka → vibhakti mappings correct
- [ ] `make validate-phase4` green
