# Story 5.1 — Unified Derivation Pipeline

**Phase:** 5 — Pipeline, Samāsa, Kṛt, Taddhita  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 4.5 (all Phase 3 + Phase 4 done)

---

## Objective

Build the unified top-level derivation pipeline that routes any input (dhātu + parameters) through the correct sequence of engines — tiṅanta, subanta, kṛt, taddhita, samāsa — producing a final `ASH_Form` with complete prakriyā trace.

This file implements `ash_tinanta()`, `ash_subanta()`, and the dispatching logic referenced in `include/ashtadhyayi.h`.

---

## Background

The public API from `ashtadhyayi.h` exposes:
```c
ASH_Form ash_tinanta(const ASH_DB *db, const char *root_slp1, int gana,
                     ASH_Lakara lakara, ASH_Purusha purusha,
                     ASH_Vacana vacana, ASH_Pada pada);

ASH_Form ash_subanta(const ASH_DB *db, const char *stem_slp1,
                     ASH_Linga linga, ASH_Vibhakti vibhakti,
                     ASH_Vacana vacana);
```

The pipeline must:
1. Load the dhātu from `dhatupatha.tsv` by matching `root_slp1`
2. Look up the gaṇa (or use caller-provided gaṇa if non-zero)
3. Dispatch to the correct vikaraṇa engine (Story 3.5, 3.6)
4. Apply sandhi at boundaries
5. Convert final SLP1 form to IAST and Devanāgarī
6. Copy the prakriyā trace into the returned `ASH_Form`

---

## Files to Create

### `prakriya/pipeline.h`

```c
#ifndef PIPELINE_H
#define PIPELINE_H

#include "ashtadhyayi.h"
#include "context.h"

typedef struct {
  SutraDB        *sutras;
  /* Dhātupāṭha data (flat array loaded from TSV) */
  struct DhatuEntry {
    char  upadesa_slp1[64];
    char  clean_slp1[32];
    int   gana;
    char  pada_flag;   /* P / A / U */
    char  meaning_en[128];
  } *dhatus;
  int  dhatu_count;
  /* Gaṇapāṭha data */
  /* (add as needed) */
} Pipeline;

/** Initialize pipeline by loading all TSV data from data_dir */
int pipeline_init(Pipeline *p, const char *data_dir);
void pipeline_free(Pipeline *p);

/** Look up a dhātu by clean SLP1 root + gaṇa hint (0 = any gaṇa) */
const struct DhatuEntry *pipeline_find_dhatu(const Pipeline *p,
                                              const char *clean_slp1,
                                              int gana_hint);

/** Run the full tiṅanta pipeline */
ASH_Form pipeline_tinanta(Pipeline *p, const char *root_slp1, int gana,
                           ASH_Lakara l, ASH_Purusha pu, ASH_Vacana v,
                           ASH_Pada pd);

/** Run the full subanta pipeline */
ASH_Form pipeline_subanta(Pipeline *p, const char *stem_slp1, ASH_Linga li,
                           ASH_Vibhakti vib, ASH_Vacana v);

#endif /* PIPELINE_H */
```

### `prakriya/pipeline.c`

Implement the pipeline. Key steps:

```c
ASH_Form pipeline_tinanta(Pipeline *p, const char *root_slp1, int gana, ...) {
  /* 1. Find dhātu in dhatupatha */
  const DhatuEntry *dhatu = pipeline_find_dhatu(p, root_slp1, gana);
  if (!dhatu) { return make_error_form("dhātu not found: %s", root_slp1); }

  /* 2. Create PrakriyaCtx */
  PrakriyaCtx ctx;
  prakriya_init_tinanta(&ctx, dhatu->upadesa_slp1, dhatu->gana, l, pu, v, pd);

  /* 3. Dispatch to correct gaṇa engine */
  bool ok;
  switch (dhatu->gana) {
    case 1:  ok = lat_bhvadi_derive_all(p->sutras, ...); break;
    case 4:  ok = lat_divadi_derive_one(p->sutras, ...); break;
    case 6:  ok = lat_tudadi_derive_one(p->sutras, ...); break;
    case 10: ok = lat_curadi_derive_one(p->sutras, ...); break;
    default: return make_error_form("gaṇa %d not yet implemented", dhatu->gana);
  }

  /* 4. Apply final sandhi */
  /* (usually already done within the gaṇa engine) */

  /* 5. Build result */
  ASH_Form f = prakriya_build_result(&ctx);

  /* 6. Encode to IAST and Devanāgarī */
  char *iast = enc_slp1_to_iast(f.slp1);
  strncpy(f.iast, iast, sizeof(f.iast) - 1);
  free(iast);
  char *deva = enc_slp1_to_devanagari(f.slp1);
  strncpy(f.devanagari, deva, sizeof(f.devanagari) - 1);
  free(deva);

  return f;
}
```

### `ASH_DB` struct (in a private header `pipeline_internal.h`)

```c
struct ASH_DB {
  Pipeline pipeline;
};
```

Implement `ash_db_load()` and `ash_db_free()` in `pipeline.c` as thin wrappers.

---

## Integration Test: `tests/unit/test_pipeline.c`

```c
static ASH_DB *g_db = NULL;

void setUp(void)    { g_db = ash_db_load("data/"); }
void tearDown(void) { ash_db_free(g_db); g_db = NULL; }

void test_bhu_bhavati(void) {
  ASH_Form f = ash_tinanta(g_db, "BU", 1, ASH_LAT,
                             ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("Bavati", f.slp1);
  TEST_ASSERT_NOT_NULL(strstr(f.iast, "bhavati"));
  ash_form_free(&f);
}

void test_div_divyati(void) {
  ASH_Form f = ash_tinanta(g_db, "dIv", 4, ASH_LAT,
                             ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("dIvyati", f.slp1);
  ash_form_free(&f);
}

void test_cur_corayati(void) {
  ASH_Form f = ash_tinanta(g_db, "cur", 10, ASH_LAT,
                             ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("corayati", f.slp1);
  ash_form_free(&f);
}

void test_rama_nominative(void) {
  ASH_Form f = ash_subanta(g_db, "rAma", ASH_PUMS,
                             ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAmaH", f.slp1);
  ash_form_free(&f);
}

void test_prakriya_trace(void) {
  ASH_Form f = ash_tinanta(g_db, "BU", 1, ASH_LAT,
                             ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_GREATER_THAN(3, f.step_count);
  /* Verify all steps have valid sūtra IDs */
  for (int i = 0; i < f.step_count; i++)
    TEST_ASSERT_GREATER_THAN(0, f.steps[i].sutra_id);
  ash_form_free(&f);
}
```

---

## Acceptance Criteria

- [ ] `ash_db_load("data/")` succeeds
- [ ] `ash_tinanta(db, "BU", 1, LAT, PRATHAMA, EKAVACANA, PARASMAI)` → "Bavati"
- [ ] `ash_subanta(db, "rAma", PUMS, PRATHAMA, EKAVACANA)` → "rAmaH"
- [ ] Returned `ASH_Form` has `step_count > 3` with valid sūtra IDs
- [ ] `ash_form_free` does not leak (valgrind clean)
- [ ] Demo in `examples/demo.c` compiles and runs showing 5 example words
