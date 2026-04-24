# Story 2.1 — Sūtra Data Model & Loader

**Phase:** 2 — Sūtra Engine & Saṃjñā System  
**Difficulty:** Medium  
**Estimated time:** 3 hours  
**Depends on:** Story 0.2 (data/sutras.tsv), Story 1.3 (encoding)

---

## Objective

Load all 3,959 sūtras from `data/sutras.tsv` into a fast in-memory database. This is the backbone of the entire library — all rule applications look up sūtras from this structure.

---

## Files to Create

### `core/sutrapatha/sutra.h`

```c
#ifndef SUTRA_H
#define SUTRA_H

#include "ashtadhyayi.h"
#include <stdbool.h>
#include <stdint.h>

#define SUTRA_TEXT_LEN 256
#define SUTRA_MAX_COUNT 4100  /* headroom above 3996 */

typedef struct {
  uint16_t      adhyaya;
  uint8_t       pada;
  uint16_t      num;
  uint32_t      global_id;
  char          text_slp1[SUTRA_TEXT_LEN];
  ASH_SutraType type;
  uint32_t      adhikara_parent;  /* 0 if none */
  uint32_t      anuvrtti_from;    /* 0 if none */
} Sutra;

typedef struct {
  Sutra   *sutras;       /* Array of all sūtras */
  uint32_t count;        /* Total sūtra count */
  /* Hash index: addr → global_id */
  uint32_t addr_index[9][5][250]; /* [adhyaya][pada][num] → global_id */
} SutraDB;

int sutra_db_load(SutraDB *db, const char *tsv_path);
void sutra_db_free(SutraDB *db);

const Sutra *sutra_get_by_id(const SutraDB *db, uint32_t global_id);
const Sutra *sutra_get_by_addr(const SutraDB *db, int adhyaya, int pada, int num);

/* Print sūtra to stream (IAST) */
void sutra_print(const Sutra *s, FILE *stream);

/* Iterate sūtras in a range [from, to] inclusive */
typedef void (*SutraVisitor)(const Sutra *s, void *userdata);
void sutra_foreach_range(const SutraDB *db, uint32_t from_id, uint32_t to_id,
                         SutraVisitor fn, void *userdata);

/* Look up all sūtras of a given type */
int sutra_filter_by_type(const SutraDB *db, ASH_SutraType type,
                          const Sutra **out, int max_out);

#endif /* SUTRA_H */
```

### `core/sutrapatha/sutra_loader.c`

Parse the TSV produced by Story 0.2. Use only stdlib (no external CSV library):
- `fopen` / `fgets` loop
- Split on `\t`
- `strtoul` for numeric fields
- Map `sutra_type` string to enum
- Populate `SutraDB.addr_index` for O(1) lookup

---

## Unit Test: `tests/unit/test_sutra_loader.c`

```c
void test_load_count(void) {
  SutraDB db;
  TEST_ASSERT_EQUAL_INT(0, sutra_db_load(&db, "data/sutras.tsv"));
  TEST_ASSERT_GREATER_OR_EQUAL(3959, db.count);
  sutra_db_free(&db);
}

void test_lookup_by_addr(void) {
  SutraDB db; sutra_db_load(&db, "data/sutras.tsv");
  const Sutra *s = sutra_get_by_addr(&db, 1, 1, 1);
  TEST_ASSERT_NOT_NULL(s);
  TEST_ASSERT_EQUAL_UINT32(1, s->global_id);
  /* vṛddhirādaic */
  TEST_ASSERT_TRUE(strstr(s->text_slp1, "vfD") != NULL ||
                   strstr(s->text_slp1, "vfd") != NULL);
  sutra_db_free(&db);
}

void test_lookup_6_1_77(void) {
  SutraDB db; sutra_db_load(&db, "data/sutras.tsv");
  const Sutra *s = sutra_get_by_addr(&db, 6, 1, 77);
  TEST_ASSERT_NOT_NULL(s);
  /* iko yaṆ aci */
  TEST_ASSERT_TRUE(strstr(s->text_slp1, "yaR") != NULL ||
                   strstr(s->text_slp1, "yaN") != NULL);
  sutra_db_free(&db);
}
```

---

## Acceptance Criteria

- [ ] All 3959+ sūtras load without error
- [ ] O(1) lookup by `(adhyāya, pāda, sūtra)` address
- [ ] O(1) lookup by `global_id`
- [ ] Spot-check: 1.1.1, 3.1.68, 6.1.77, 7.3.84, 8.2.1 all found correctly
- [ ] Unit test passes; no memory leaks
