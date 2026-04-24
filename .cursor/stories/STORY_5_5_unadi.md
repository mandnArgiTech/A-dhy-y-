# Story 5.5 — Uṇādi Suffixes

**Phase:** 5 — Pipeline, Samāsa, Kṛt, Taddhita  
**Difficulty:** Medium  
**Estimated time:** 3 hours  
**Depends on:** Story 5.3 (kṛt), 5.4 (taddhita patterns)

---

## Objective

Implement the Uṇādi suffix system — a supplementary text (~750 sūtras) that extends the kṛt derivative system with irregular/suppletive forms. Many everyday Sanskrit words are derived via Uṇādi suffixes.

---

## Background

The Uṇādi Sūtras were composed as an appendix to handle words whose derivation doesn't fit neatly into the main Aṣṭādhyāyī kṛt system. The text begins: `kṛ + u = ku` (hence "Uṇādi" — starting with uṆ). These derivations often involve:
- Phonological irregularities
- Historical forms that have become lexicalized
- Sound symbolism patterns

Key Uṇādi forms (commonly cited in traditional grammar):

| Root | Suffix | Form | IAST | Meaning |
|------|--------|------|------|---------|
| kṛ | uṆ | kuru | kuru | do! (imperative, lexicalized) |
| sthā | u | sānu | sānu | ridge, plateau |
| jan | u | jānu | jānu | knee |
| vā | yu | vāyu | vāyu | wind |
| sah | u | sāhu | sāhu | (rare) |
| bandh | u | bandhu | bandhu | relative (kin) |
| mṛ | u | maru | maru | desert |
| pā | tu | pītu | pītu | drink |
| śru | ti | śruti | śruti | hearing, Veda |
| smṛ | ti | smṛti | smṛti | memory |
| gati | i | — | — | going |
| rakṣ | as | rakṣas | rakṣas | demon |
| man | as | manas | manas | mind |
| tap | as | tapas | tapas | austerity |
| car | as | caras | caras | moving |
| bhū | mi | bhūmi | bhūmi | earth |
| kṛ | ṣṇa | kṛṣṇa | kṛṣṇa | dark one |
| arj | una | arjuna | arjuna | white one |

---

## Implementation Approach

The Uṇādi system is primarily a **dictionary** — each derivation is individually attested rather than being rule-generated. The C implementation should:

1. **Load** Uṇādi data from `data/unadipatha.tsv` (generated in ingestion)
2. **Match** root + suffix → form
3. **Return** the attested form with the applicable Uṇādi sūtra ID

This differs from the generative approach of kṛt — Uṇādi forms are looked up, not derived.

---

## Data File: `data/unadipatha.tsv`

Generate from `tools/ingest_unadipatha.py`:
```
# unadi_id  root_slp1  suffix_slp1  form_slp1  form_iast  meaning_en  sutra_ref
1           kf         uR           kuru       kuru       do (imper)  unadi_1.1
2           vA         yu           vAyu       vāyu       wind        unadi_1.2
3           jan        u            jAnu       jānu       knee        unadi_1.5
4           banD       u            banDu      bandhu     kinsman     unadi_1.8
...
```

---

## Files to Create

### `ancillary/unadipatha/unadi.h`

```c
#ifndef UNADI_H
#define UNADI_H

#include "ashtadhyayi.h"

typedef struct {
  uint32_t  unadi_id;
  char      root_slp1[32];
  char      suffix_slp1[16];
  char      form_slp1[64];
  char      meaning_en[128];
  char      sutra_ref[32];
} UnadiEntry;

typedef struct {
  UnadiEntry *entries;
  int         count;
} UnadiDB;

int unadi_db_load(UnadiDB *db, const char *tsv_path);
void unadi_db_free(UnadiDB *db);

/**
 * Look up an attested Uṇādi form.
 * @param root_slp1  Dhātu root in SLP1
 * @param suffix     Optional suffix hint (NULL = return first match)
 * @return First matching entry, or NULL if not in Uṇādi corpus
 */
const UnadiEntry *unadi_lookup(const UnadiDB *db,
                                const char *root_slp1,
                                const char *suffix_slp1);

/**
 * Check if a given word form is an attested Uṇādi derivative.
 * Useful for morphological analysis.
 */
bool unadi_is_attested(const UnadiDB *db, const char *form_slp1);

/**
 * Return an ASH_Form for a Uṇādi word (with provenance).
 * The form is a lookup, not a derivation, but prakriyā trace records
 * the Uṇādi sūtra as the governing rule.
 */
ASH_Form unadi_form(const UnadiDB *db, const char *root_slp1,
                     const char *suffix_slp1);

#endif /* UNADI_H */
```

### `ancillary/unadipatha/unadi.c`

```c
int unadi_db_load(UnadiDB *db, const char *tsv_path) {
  /* Parse TSV line by line */
  /* Allocate entries array */
  /* Fill UnadiEntry structs */
  /* Return 0 on success */
}

const UnadiEntry *unadi_lookup(const UnadiDB *db, const char *root_slp1,
                                const char *suffix_slp1) {
  for (int i = 0; i < db->count; i++) {
    if (strcmp(db->entries[i].root_slp1, root_slp1) == 0) {
      if (!suffix_slp1 ||
          strcmp(db->entries[i].suffix_slp1, suffix_slp1) == 0) {
        return &db->entries[i];
      }
    }
  }
  return NULL;
}

ASH_Form unadi_form(const UnadiDB *db, const char *root_slp1,
                     const char *suffix_slp1) {
  const UnadiEntry *e = unadi_lookup(db, root_slp1, suffix_slp1);
  if (!e) {
    ASH_Form f = {0};
    f.valid = false;
    snprintf(f.error, sizeof(f.error), "Not in Uṇādi corpus: %s + %s",
             root_slp1, suffix_slp1 ? suffix_slp1 : "(any)");
    return f;
  }

  ASH_Form f = {0};
  f.valid = true;
  strncpy(f.slp1, e->form_slp1, sizeof(f.slp1) - 1);
  char *iast = enc_slp1_to_iast(e->form_slp1);
  strncpy(f.iast, iast, sizeof(f.iast) - 1);
  free(iast);

  /* Log Uṇādi sūtra as governing rule */
  f.steps = malloc(sizeof(ASH_PrakriyaStep));
  f.step_count = 1;
  f.steps[0].sutra_id = 0; /* Uṇādi sūtras have separate numbering */
  snprintf(f.steps[0].note, sizeof(f.steps[0].note),
           "Uṇādi sūtra %s: %s + %s → %s",
           e->sutra_ref, root_slp1,
           e->suffix_slp1, e->form_slp1);

  return f;
}
```

### `tools/ingest_unadipatha.py`

Fetch Uṇādi data from:
```
https://raw.githubusercontent.com/ashtadhyayi-com/data/master/unadipatha/unadipatha.json
```

Output `data/unadipatha.tsv` with the schema above. Validate:
- ≥ 200 entries
- All forms have non-empty `form_slp1`
- Key words present: "vAyu", "jAnu", "banDu", "manas", "tapas", "smfti"

---

## Integration with Main Pipeline

In `pipeline.c`, add:
```c
/* Before failing on a kṛt derivation, check if the form is Uṇādi-attested */
ASH_Form f = krit_derive(p->sutras, root_slp1, gana, krit);
if (!f.valid) {
  ASH_Form u = unadi_form(&p->unadi_db, root_slp1, NULL);
  if (u.valid) return u;
}
```

---

## Unit Test: `tests/unit/test_unadi.c`

```c
void test_vayu_lookup(void) {
  UnadiDB db; unadi_db_load(&db, "data/unadipatha.tsv");
  const UnadiEntry *e = unadi_lookup(&db, "vA", "yu");
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_STRING("vAyu", e->form_slp1);
  unadi_db_free(&db);
}

void test_janu_lookup(void) {
  UnadiDB db; unadi_db_load(&db, "data/unadipatha.tsv");
  const UnadiEntry *e = unadi_lookup(&db, "jan", "u");
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_STRING("jAnu", e->form_slp1);
  unadi_db_free(&db);
}

void test_not_in_corpus(void) {
  UnadiDB db; unadi_db_load(&db, "data/unadipatha.tsv");
  const UnadiEntry *e = unadi_lookup(&db, "zzz", NULL);
  TEST_ASSERT_NULL(e);
  unadi_db_free(&db);
}

void test_is_attested(void) {
  UnadiDB db; unadi_db_load(&db, "data/unadipatha.tsv");
  TEST_ASSERT_TRUE(unadi_is_attested(&db, "manas"));
  TEST_ASSERT_TRUE(unadi_is_attested(&db, "tapas"));
  TEST_ASSERT_FALSE(unadi_is_attested(&db, "xyz"));
  unadi_db_free(&db);
}
```

---

## Acceptance Criteria

- [ ] `tools/ingest_unadipatha.py` generates `data/unadipatha.tsv` with ≥ 200 entries
- [ ] "vāyu", "jānu", "bandhu", "manas", "tapas", "smṛti" all found by lookup
- [ ] `unadi_form` returns valid `ASH_Form` with SLP1 and IAST
- [ ] `is_attested` correctly identifies known Uṇādi words
- [ ] Unit test 0 failures
- [ ] `make validate-phase5` green
