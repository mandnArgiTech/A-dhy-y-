# Story 2.4 — Adhikāra (Domain) Scope Tracker

**Phase:** 2 — Sūtra Engine & Saṃjñā System  
**Difficulty:** Medium  
**Estimated time:** 2.5 hours  
**Depends on:** Story 2.1

---

## Objective

Track which adhikāra (domain-marking) sūtra governs each sūtra. Adhikāra sūtras extend their scope ("applies in this domain") over all subsequent sūtras until another adhikāra overrides or the scope ends.

---

## Background

An adhikāra sūtra announces a domain. For example:
- 3.1.1 `pratyayaḥ` — "what follows is a pratyaya" — governs 3.1.1 through 5.4.160
- 6.4.1 `aṅgasya` — "applies to the aṅga" — governs 6.4.1 through 7.4.97
- 8.2.1 `pūrvatrāsiddham` — "earlier [rules are] not operative here"

---

## Files

### `core/metadata/adhikara.h`

```c
#ifndef ADHIKARA_H
#define ADHIKARA_H

#include "sutra.h"

/** Pre-computed adhikāra data — call after sutra_db_load() */
int adhikara_build(SutraDB *db, const char *adhikara_tsv);

/** Is sūtra `child_id` under the scope of adhikāra `adhikara_id`? */
bool adhikara_governs(const SutraDB *db, uint32_t adhikara_id, uint32_t child_id);

/** Get the innermost governing adhikāra of a sūtra. */
uint32_t adhikara_parent_of(const SutraDB *db, uint32_t sutra_id);

/** Get all adhikāra parents (innermost first) of a sūtra. */
int adhikara_parents_of(const SutraDB *db, uint32_t sutra_id,
                         uint32_t *parents, int max_parents);

#endif
```

### Data file: `data/adhikara_scope.tsv`

Create this file manually encoding scholarly consensus:
```
# adhikara_global_id  scope_end_global_id  label_slp1
# (global_ids are from data/sutras.tsv)
<id of 3.1.1>   <id of 5.4.160>  pratyayaH
<id of 6.4.1>   <id of 7.4.97>   aNgasya
<id of 8.2.1>   <id of 8.4.68>   pUrvAtrAsiddham
<id of 3.4.1>   <id of 3.4.117>  laH karmaRi
... (20 major adhikāras encoded)
```

Use `python3 tools/compute_adhikara_scope.py` to cross-reference with ashtadhyayi.com commentary data and generate this file.

---

## Unit Test: `tests/unit/test_adhikara.c`

```c
void test_pratyaya_scope(void) {
  /* 3.1.68 (kartari śap) must be under 3.1.1 (pratyayaḥ) */
  /* Look up global_ids */
  const Sutra *a = sutra_get_by_addr(&g_db, 3, 1, 1);
  const Sutra *s = sutra_get_by_addr(&g_db, 3, 1, 68);
  TEST_ASSERT_TRUE(adhikara_governs(&g_db, a->global_id, s->global_id));
}

void test_anga_scope(void) {
  /* 7.3.84 (sārvadhatuka guṇa) is under 6.4.1 (aṅgasya) */
  const Sutra *a = sutra_get_by_addr(&g_db, 6, 4, 1);
  const Sutra *s = sutra_get_by_addr(&g_db, 7, 3, 84);
  TEST_ASSERT_TRUE(adhikara_governs(&g_db, a->global_id, s->global_id));
}
```

---

## Acceptance Criteria

- [ ] 20 major adhikāras encoded in TSV
- [ ] `adhikara_governs` correct for 10 test pairs
- [ ] `adhikara_parent_of` returns correct innermost adhikāra

---
---

# Story 2.5 — Anuvṛtti (Ellipsis/Carry-Over) Resolver

**Phase:** 2 — Sūtra Engine & Saṃjñā System  
**Difficulty:** Very High  
**Estimated time:** 5 hours  
**Depends on:** Story 2.4

---

## Objective

Implement anuvṛtti — the mechanism by which terms from preceding sūtras are silently carried over into the current sūtra. This is the most complex problem in Pāṇinian computation and requires encoding scholarly tradition.

---

## Background

Pāṇini wrote sūtras so briefly that many lack key terms. These are supplied by anuvṛtti — ellipsis from adjacent sūtras. The Kāśikāvṛtti and Laghu Siddhānta Kaumudī are the primary sources for determining which terms carry over.

Example:
- 1.1.1: `vṛddhirādaic` — Vṛddhi is [the name for] ā, ai, au
- 1.1.2: `adeṅ guṇaḥ` — [a,] eṅ [is] Guṇa
  - anuvṛtti: "guṇaḥ" carries over to 1.1.3
- 1.1.3: `iko guṇavṛddhī` — of ik, [the adeṅ terms are] guṇa, [the ādaic terms are] vṛddhi
  - anuvṛtti: "vṛddhiḥ" from 1.1.1, "guṇaḥ" from 1.1.2

---

## Approach

Encode anuvṛtti as a static data file `data/anuvrtti.tsv` derived from the Laghu Siddhānta Kaumudī ordering. This is scholarly data, not computable:

```
# sutra_global_id  term_slp1  source_sutra_global_id
<id 1.1.2>    vfDDiH    <id 1.1.1>
<id 1.1.3>    guRaH     <id 1.1.2>
<id 1.1.3>    vfDDiH    <id 1.1.1>
<id 3.1.68>   pratyayaH <id 3.1.1>
<id 3.1.68>   kartari   (self — stated in sūtra)
...
```

Create `tools/encode_anuvrtti.py` to build this file from a reference commentary JSON.

---

## Files

### `core/metadata/anuvrtti.h`

```c
#ifndef ANUVRTTI_H
#define ANUVRTTI_H

#include <stdint.h>

typedef struct {
  char     term_slp1[64];
  uint32_t source_sutra_id;
} AnuvrttiBTerm;

typedef struct {
  uint32_t      sutra_id;
  AnuvrttiBTerm *terms;
  int            term_count;
} AnuvrttiEntry;

typedef struct {
  AnuvrttiEntry *entries;
  int            count;
} AnuvrttiDB;

int anuvrtti_load(AnuvrttiDB *db, const char *tsv_path);
void anuvrtti_db_free(AnuvrttiDB *db);

/** Get all anuvṛtti terms for a given sūtra. */
const AnuvrttiEntry *anuvrtti_get(const AnuvrttiDB *db, uint32_t sutra_id);

/** Get the full "resolved" sūtra text including anuvṛtti terms. */
int anuvrtti_resolve(const AnuvrttiDB *db, uint32_t sutra_id,
                     char *out, size_t out_len);

#endif
```

---

## Initial Data

For Phase 2 bootstrap, encode anuvṛtti for at minimum these key sūtras:
- 1.1.1–1.1.10 (foundational terms)
- 3.1.1–3.1.68 (pratyaya scope)
- 7.3.84–7.3.86 (guṇa rules)
- 8.4.53–8.4.55 (jhal sandhi)

The full encoding (all 3959 sūtras) can be populated incrementally in later phases.

---

## Acceptance Criteria

- [ ] `anuvrtti_load` succeeds on `data/anuvrtti.tsv`
- [ ] `anuvrtti_get` for 3.1.68 returns `["pratyayaḥ", "kartari"]`
- [ ] `anuvrtti_resolve` for 7.3.84 includes "aṅgasya" from 6.4.1
- [ ] Unit test covers 5 key sūtras

---
---

# Story 2.6 — Paribhāṣā (Metarule) Engine

**Phase:** 2 — Sūtra Engine & Saṃjñā System  
**Difficulty:** High  
**Estimated time:** 3 hours  
**Depends on:** Story 2.1, 2.2

---

## Objective

Implement the paribhāṣā system — metarules that govern how ordinary rules are applied. Without these, rule application would be ambiguous.

---

## Background

There are ~122 traditional paribhāṣās. The most critical:

| # | Paribhāṣā (SLP1) | Meaning |
|---|-------------------|---------|
| 1 | pUrvAparAnityAntaraNG... | Priority order for competing rules |
| 2 | sTA-nIvad AdeSo'nalviDAu | Substitute behaves like original (except in al-rules) |
| 3 | asiddham bahiraNG... | Bahiraṅga rule is inoperative vs antaraṅga |
| 4 | kArya-kAlaM saMjYA-paribuDDiH | Technical terms understood at time of application |
| 5 | ekA vibhaktiH | One vibhakti at a time |
| 6 | yat pUrvaM tat pUrva-siddham | What comes earlier is settled first |

Source: Paribhāṣenduśekhara (Nāgeśabhaṭṭa, ~1700 CE) — 122 paribhāṣās with full discussion.

---

## Files

### `core/metadata/paribhasha.h`

```c
#ifndef PARIBHASHA_H
#define PARIBHASHA_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  PB_STHANIVAD = 1,       /* Substitute = original (except al-rules) */
  PB_ASIDDHA_BAHIRANG,    /* 8.2.1: bahiraṅga inoperative */
  PB_PURVA_PATA,          /* Later rule wins (paratva) */
  PB_NITYA_OVER_ANITYA,   /* Obligatory > optional */
  PB_APAVADA_OVER_UTSARGA, /* Exception > general */
  PB_ANTARA_OVER_BAHIRA,  /* Inner > outer */
  PB_SARVADHATUKE_ARDHA,  /* Sārvadhatuka > ārdhadhātuka */
  PB_ANGA_ASIDDHA,        /* 6.4.22: certain rules unseen for later aṅga rules */
  PB_COUNT
} ParibhashaId;

typedef struct {
  const char *label_slp1;
  const char *description_en;
  bool (*applies)(ParibhashaId pb, const void *ctx);
} Paribhasha;

/** Check whether paribhāṣā pb_id applies in the given derivation context. */
bool paribhasha_applies(ParibhashaId pb_id, const void *ctx);

/** Get paribhāṣā by id (for description/logging). */
const Paribhasha *paribhasha_get(ParibhashaId id);

/**
 * The sthānivad principle (PB_STHANIVAD):
 * When rule R applies to original X producing substitute Y,
 * subsequent rules that would apply to X also apply to Y
 * (except rules whose scope is defined by phoneme (al) identity).
 */
bool paribhasha_sthanivad_applies(const char *original, const char *substitute,
                                   uint32_t next_rule_id);

#endif
```

### `core/metadata/paribhasha.c`

Implement each paribhāṣā as a predicate function. For now, `applies()` can return `true` as a stub for non-critical ones and be filled out as derivation scenarios require them.

Fully implement:
1. **PB_STHANIVAD** — needed immediately for Step 1.3.1 (dhātu designation)
2. **PB_PURVA_PATA** — last rule wins (default tiebreak)
3. **PB_APAVADA_OVER_UTSARGA** — exceptions beat generals (needed for conflict resolution)
4. **PB_ASIDDHA_BAHIRANG** — 8.2.1 scope (needed for Phase 3+)

---

## Unit Test: `tests/unit/test_paribhasha.c`

```c
void test_sthanivad_basic(void) {
  /* If original is 'BU' (bhū) and substitute is 'Bo' (bho after guṇa),
     then rules applicable to BU's ū-ness apply to Bo's o */
  bool r = paribhasha_sthanivad_applies("BU", "Bo", 0 /* any rule */);
  TEST_ASSERT_TRUE(r);
}

void test_all_paribhashas_defined(void) {
  for (int i = 1; i < PB_COUNT; i++) {
    const Paribhasha *pb = paribhasha_get((ParibhashaId)i);
    TEST_ASSERT_NOT_NULL(pb);
    TEST_ASSERT_NOT_NULL(pb->label_slp1);
  }
}
```

---

## Acceptance Criteria

- [ ] All 8 paribhāṣā IDs defined with descriptions
- [ ] `PB_STHANIVAD` implemented (not stub)
- [ ] `PB_APAVADA_OVER_UTSARGA` implemented
- [ ] `make validate-phase2` green
