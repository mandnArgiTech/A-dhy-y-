# Story 3.1 — Term (Pada) Data Model & Derivation Context

**Phase:** 3 — Pratyaya System & Conflict Resolution  
**Difficulty:** Medium  
**Estimated time:** 3 hours  
**Depends on:** Story 2.2, 2.3

---

## Objective

Define the core data structures for the derivation engine: the `Term` (a morpheme with its full metadata) and the `PrakriyaCtx` (derivation context that carries all terms and state through the entire derivation).

---

## Files

### `prakriya/term.h`

```c
#ifndef TERM_H
#define TERM_H

#include "samjna.h"
#include "anubandha.h"
#include <stdint.h>

#define TERM_VALUE_LEN   128
#define TERM_UPADESA_LEN 128
#define MAX_RULE_HISTORY 64

typedef struct {
  char     value[TERM_VALUE_LEN];      /* Current SLP1 form (evolves during derivation) */
  char     upadesa[TERM_UPADESA_LEN];  /* Original upadesa (fixed) */
  Samjna   samjna;                     /* All current saṃjñās */
  uint64_t it_flags;                   /* Anubandha presence bitmask */
  /* Rule application history */
  uint32_t rule_history[MAX_RULE_HISTORY];
  uint8_t  rule_count;
  /* Position in derivation sequence */
  uint8_t  index;    /* 0-based index in PrakriyaCtx.terms[] */
  bool     is_final; /* true after prakriyā complete */
} Term;

/** Initialize term from upadesa string + context (determines anubandha stripping) */
void term_init(Term *t, const char *upadesa_slp1, Samjna context);

/** Apply a substitution to term's value */
void term_substitute(Term *t, const char *new_value, uint32_t rule_id);

/** Record that a rule was applied to this term */
void term_record_rule(Term *t, uint32_t rule_id);

/** Does this term have a given saṃjñā? */
static inline bool term_has(const Term *t, Samjna flag) {
  return samjna_has(t->samjna, flag);
}

/** Get the first character of term's value */
static inline char term_initial(const Term *t) {
  return t->value[0];
}

/** Get the last character of term's value */
char term_final(const Term *t);

/** Get the penultimate (second-to-last) character */
char term_penultimate(const Term *t);

/** Does this term end with a vowel? */
bool term_ends_vowel(const Term *t);

/** Does this term begin with a vowel? */
bool term_begins_vowel(const Term *t);

/** Print term for debugging */
void term_print(const Term *t, FILE *stream);

#endif /* TERM_H */
```

### `prakriya/context.h`

```c
#ifndef PRAKRIYA_CTX_H
#define PRAKRIYA_CTX_H

#include "term.h"
#include "ashtadhyayi.h"
#include <stdio.h>

#define MAX_TERMS         8
#define MAX_PRAKRIYA_STEPS 128

/** A single logged step in the derivation */
typedef struct {
  uint32_t sutra_id;
  char     description[128];
  char     form_before[128];
  char     form_after[128];
} PrakriyaStep;

/** The full derivation context */
typedef struct {
  /* Terms (morphemes) in order */
  Term    terms[MAX_TERMS];
  int     term_count;

  /* Current derivation parameters */
  ASH_Lakara  lakara;
  ASH_Purusha purusha;
  ASH_Vacana  vacana;
  ASH_Pada    pada;
  ASH_Linga   linga;
  ASH_Vibhakti vibhakti;
  int         gana;

  /* Derivation log */
  PrakriyaStep steps[MAX_PRAKRIYA_STEPS];
  int          step_count;

  /* Error state */
  bool  error;
  char  error_msg[256];
} PrakriyaCtx;

/** Initialize for tiṅanta derivation */
void prakriya_init_tinanta(PrakriyaCtx *ctx, const char *dhatu_slp1, int gana,
                            ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd);

/** Initialize for subanta derivation */
void prakriya_init_subanta(PrakriyaCtx *ctx, const char *stem_slp1, ASH_Linga li,
                            ASH_Vibhakti vib, ASH_Vacana v);

/** Log a derivation step */
void prakriya_log(PrakriyaCtx *ctx, uint32_t sutra_id, const char *description);

/** Get the current concatenated form (all terms joined) */
void prakriya_current_form(const PrakriyaCtx *ctx, char *out, size_t len);

/** Print full derivation trace to stream */
void prakriya_print_trace(const PrakriyaCtx *ctx, FILE *stream);

/** Build ASH_Form result from completed context */
ASH_Form prakriya_build_result(const PrakriyaCtx *ctx);

#endif /* PRAKRIYA_CTX_H */
```

---

## Unit Test: `tests/unit/test_term.c`

```c
void test_term_init_dhatu(void) {
  Term t;
  term_init(&t, "BU", SJ_DHATU);
  TEST_ASSERT_EQUAL_STRING("BU", t.value);
  TEST_ASSERT_TRUE(term_has(&t, SJ_DHATU));
  TEST_ASSERT_EQUAL_CHAR('U', term_final(&t));
  TEST_ASSERT_TRUE(term_ends_vowel(&t));
}

void test_prakriya_init(void) {
  PrakriyaCtx ctx;
  prakriya_init_tinanta(&ctx, "BU", 1, ASH_LAT,
                         ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_EQUAL_INT(1, ctx.term_count);
  TEST_ASSERT_TRUE(term_has(&ctx.terms[0], SJ_DHATU));
}
```

---

## Acceptance Criteria

- [ ] `Term` and `PrakriyaCtx` structs compile
- [ ] `term_init` correctly sets saṃjñā and strips anubandhas
- [ ] `prakriya_init_tinanta` creates 1-term context with dhātu
- [ ] `prakriya_log` appends steps correctly
- [ ] `prakriya_print_trace` outputs readable derivation

---
---

# Story 3.2 — Conflict Resolution Engine

**Phase:** 3 — Pratyaya System  
**Difficulty:** High  
**Estimated time:** 3.5 hours  
**Depends on:** Story 2.6, 3.1

---

## Objective

When multiple sūtras are applicable at the same point in a derivation, determine which one fires. This implements the 4-level priority hierarchy from the paribhāṣā tradition.

---

## Priority Order (highest to lowest)

```
1. apavāda (exception rule) > utsarga (general rule)
2. nitya (obligatory) > anitya (optional/conditional)
3. antaraṅga (inner operation) > bahiraṅga (outer operation)  [8.2.1]
4. paratva (later rule in sūtra order wins)  [last resort]
```

---

## Files

### `core/conflict/conflict.h`

```c
#ifndef CONFLICT_H
#define CONFLICT_H

#include "sutra.h"
#include <stdint.h>

/* Metadata needed to resolve conflicts */
typedef struct {
  uint32_t sutra_id;
  bool     is_apavada;    /* Is this an exception rule? */
  bool     is_nitya;      /* Is this obligatorily applicable? */
  bool     is_antaranga;  /* Is this an inner operation? */
  bool     is_optional;   /* Can the rule be bypassed? */
} ConflictCandidate;

/**
 * Resolve competition between multiple applicable rules.
 * @param db         Loaded sūtra database
 * @param candidates Array of candidate rule metadata
 * @param count      Number of candidates
 * @param ctx        Current derivation context (for antaraṅga test)
 * @return Index into candidates[] of the winning rule, or -1 on tie
 */
int conflict_resolve(const SutraDB *db,
                     const ConflictCandidate *candidates, int count,
                     const void *ctx);

/**
 * Build a ConflictCandidate for a given sūtra.
 * Determines apavāda/nitya/antaraṅga status from rule metadata.
 */
ConflictCandidate conflict_candidate_build(const SutraDB *db, uint32_t sutra_id);

/** Check if rule A is an apavāda (exception) of rule B */
bool conflict_is_apavada_of(const SutraDB *db, uint32_t a_id, uint32_t b_id);

#endif /* CONFLICT_H */
```

### `core/conflict/conflict.c`

Implement `conflict_resolve` step by step:

```c
int conflict_resolve(...) {
  // Step 1: Filter to only apavādas if any exist
  // If exactly one candidate is an apavāda of any other, it wins.

  // Step 2: If still tied, prefer nitya over anitya.

  // Step 3: If still tied, prefer antaraṅga over bahiraṅga.
  // (antaraṅga = operation whose environment is internal to the current derivation unit)

  // Step 4: Last resort — highest sutra_id wins (paratva: later in sūtra order)
  // NOTE: In the Aṣṭādhyāyī, HIGHER global_id = later = wins paratva.
}
```

#### Classic test case

Rules 6.1.77 (yaṆ) and 6.1.101 (savarṇadīrgha) can both apply when:
- Form ends in `i` and next begins with `i`

In this case:
- 6.1.101 (savarṇadīrgha) is more specific → apavāda → WINS
- Result: `i + i → ī` not `y + i`

---

## Unit Test: `tests/unit/test_conflict.c`

```c
void test_apavada_wins(void) {
  /* 6.1.101 (later, more specific) beats 6.1.77 (earlier, general)
     when i+i is the junction */
  SutraDB db; sutra_db_load(&db, "data/sutras.tsv");
  uint32_t id_yan    = sutra_get_by_addr(&db, 6, 1, 77)->global_id;
  uint32_t id_dirgha = sutra_get_by_addr(&db, 6, 1, 101)->global_id;

  ConflictCandidate cands[2] = {
    conflict_candidate_build(&db, id_yan),
    conflict_candidate_build(&db, id_dirgha),
  };
  int winner = conflict_resolve(&db, cands, 2, NULL);
  TEST_ASSERT_EQUAL_INT(1, winner);  /* index 1 = savarṇadīrgha wins */
  sutra_db_free(&db);
}

void test_paratva_fallback(void) {
  /* When two non-apavāda rules compete, later one (higher id) wins */
  SutraDB db; sutra_db_load(&db, "data/sutras.tsv");
  uint32_t id_a = sutra_get_by_addr(&db, 7, 3, 102)->global_id;
  uint32_t id_b = sutra_get_by_addr(&db, 7, 3, 103)->global_id;

  ConflictCandidate cands[2] = {
    conflict_candidate_build(&db, id_a),
    conflict_candidate_build(&db, id_b),
  };
  int winner = conflict_resolve(&db, cands, 2, NULL);
  TEST_ASSERT_EQUAL_INT(1, winner);  /* 7.3.103 is later → wins */
  sutra_db_free(&db);
}
```

---

## Acceptance Criteria

- [ ] 4-level priority order implemented
- [ ] `conflict_resolve` correctly handles all 4 cases
- [ ] Classic 6.1.77 vs 6.1.101 test passes
- [ ] Paratva fallback test passes
- [ ] Unit test 0 failures

---
---

# Story 3.3 — Lakaras & Tiṅ Suffix Table

**Phase:** 3 — Pratyaya System  
**Difficulty:** Medium  
**Estimated time:** 2.5 hours  
**Depends on:** Story 3.1, 2.3

---

## Objective

Define all 18 lakaras and the complete tiṅ (personal ending) suffix table. These are the 18 endings × 2 padas × 3 puruṣas × 3 vacanas = 108 suffix forms.

---

## Background

The tiṅ suffixes are listed in 3.4.78–3.4.112. Each suffix has:
- An upadesa form (with it-markers)
- A clean form (after stripping)
- A saṃjñā (sārvadhatuka or ārdhadhātuka)

### Parasmaipada tiṅ suffixes (3.4.78):

| | Ekavacana | Dvivacana | Bahuvacana |
|-|-----------|-----------|------------|
| Prathama (3rd) | tip | tas | jhi |
| Madhyama (2nd) | sip | Tas | Ta |
| Uttama (1st) | mip | vas | mas |

### Ātmanepada tiṅ suffixes (3.4.79):

| | Ekavacana | Dvivacana | Bahuvacana |
|-|-----------|-----------|------------|
| Prathama (3rd) | ta | AtAm | jha |
| Madhyama (2nd) | TAs | ATAm | Dvam |
| Uttama (1st) | iw | vahiN | mahiN |

(In SLP1 — capitals as per encoding table in AGENTS.md)

Each lakāra replaces these tiṅ suffixes with specific alternants per 3.4.77 scope rules.

---

## Files

### `prakriya/tinanta/lakara.h`

```c
#ifndef LAKARA_H
#define LAKARA_H

#include "ashtadhyayi.h"
#include "term.h"

typedef struct {
  const char *upadesa_slp1;  /* Full upadesa with it-markers */
  const char *clean_slp1;    /* After anubandha stripping */
  Samjna      samjna;        /* SJ_SARVADHATUKA or SJ_ARDHADHATUKA */
  ASH_Purusha purusha;
  ASH_Vacana  vacana;
  ASH_Pada    pada;
} TingEntry;

/** Get the tiṅ suffix for a given lakāra + puruṣa + vacana + pada */
const TingEntry *ting_get(ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd);

/** Assign tiṅ suffix to a derivation context (adds suffix term) */
int ting_assign(PrakriyaCtx *ctx, ASH_Lakara l, ASH_Purusha p,
                ASH_Vacana v, ASH_Pada pd);

/** Is this lakāra's tiṅ sārvadhatuka? (3.4.113) */
bool lakara_is_sarvadhatuka(ASH_Lakara l);

/** Get the traditional name of a lakāra (SLP1) */
const char *lakara_name(ASH_Lakara l);

#endif
```

### `prakriya/tinanta/ting.c`

Hardcode the full 108-entry tiṅ table as a static const array. For laṭ (present), the suffixes are as above. For other lakaras, they are modified substitutes — implement laṭ fully, stub others as TODO.

---

## Unit Test: `tests/unit/test_lakara.c`

```c
void test_lat_parasmai_prathama_eka(void) {
  const TingEntry *t = ting_get(ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_NOT_NULL(t);
  TEST_ASSERT_EQUAL_STRING("tip", t->upadesa_slp1);
  TEST_ASSERT_EQUAL_STRING("ti", t->clean_slp1);
  TEST_ASSERT_TRUE(samjna_has(t->samjna, SJ_SARVADHATUKA));
}

void test_lat_atmane_prathama_eka(void) {
  const TingEntry *t = ting_get(ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_ATMANE);
  TEST_ASSERT_NOT_NULL(t);
  TEST_ASSERT_EQUAL_STRING("ta", t->clean_slp1);
}

void test_all_108_defined(void) {
  for (int l = 0; l < ASH_LAKARA_COUNT; l++)
    for (int p = 0; p < 3; p++)
      for (int v = 0; v < 3; v++)
        for (int pd = 0; pd < 2; pd++) {
          const TingEntry *t = ting_get(l, p, v, pd);
          /* For laṭ: must be defined; others may be NULL for now */
          if (l == ASH_LAT)
            TEST_ASSERT_NOT_NULL(t);
        }
}
```

---

## Acceptance Criteria

- [ ] All 18 laṭ tiṅ suffixes defined (9 parasmaipada + 9 ātmanepada)
- [ ] `ting_get(LAT, PRATHAMA, EKAVACANA, PARASMAI)` → upadesa="tip", clean="ti"
- [ ] Saṃjñā correctly SJ_SARVADHATUKA for laṭ
- [ ] Unit test passes

---
---

# Story 3.4 — Guṇa & Vṛddhi Substitution Rules

**Phase:** 3 — Pratyaya System  
**Difficulty:** Medium  
**Estimated time:** 2 hours  
**Depends on:** Story 3.1, 1.1

---

## Objective

Implement the guṇa and vṛddhi vowel-strengthening operations applied during derivation. These fire frequently — in nearly every verbal and nominal derivation.

---

## Key Sūtras to Implement

```
7.3.82  — anudāttopadeśa-vanato 'nāpityasya — roots with anudātta vowel take guṇa
7.3.84  — sārvadhātukārdhadhātukayoḥ — before sārvadhatuka/ārdhadhātuka → guṇa of ik
7.3.86  — pugantalaghūpadhasya ca — laghu upadha root takes guṇa before vowel suffix
7.2.115 — aco ñṇiti — ṇit suffix triggers vṛddhi
7.3.101 — ato dīrgho yañi — a-final aṅga + ya/yañ → long ā
1.1.5   — kṅiti ca — k/ṅ-it suffixes block guṇa/vṛddhi (exception)
```

---

## Files

### `prakriya/guna_vrddhi.h`

```c
#ifndef GUNA_VRDDHI_H
#define GUNA_VRDDHI_H

#include "term.h"
#include "context.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Apply guṇa to a term's final vowel (7.3.84).
 * @return true if change made, false if no ik vowel found or blocked by 1.1.5
 */
bool guna_apply_to_final(Term *anga, const Term *pratyaya, uint32_t *rule_fired);

/**
 * Apply guṇa to a term's penultimate (upadha) vowel (7.3.86).
 * @return true if change made
 */
bool guna_apply_to_upadha(Term *anga, const Term *pratyaya, uint32_t *rule_fired);

/**
 * Apply vṛddhi to a term's final vowel (7.2.115).
 */
bool vrddhi_apply_to_final(Term *anga, const Term *pratyaya, uint32_t *rule_fired);

/**
 * Check if guṇa/vṛddhi is blocked by kit/ṅit suffix (1.1.5).
 * @return true if blocked (should NOT apply guṇa)
 */
bool guna_is_blocked(const Term *pratyaya);

#endif
```

---

## Test Cases

```c
void test_guna_bu(void) {
  /* BU + sārvadhatuka suffix → guṇa of ū → o → "Bo" */
  Term anga; term_init(&anga, "BU", SJ_DHATU | SJ_ANGA);
  Term pratyaya; term_init(&pratyaya, "Sap", SJ_PRATYAYA | SJ_SARVADHATUKA);
  uint32_t rule;
  bool changed = guna_apply_to_final(&anga, &pratyaya, &rule);
  TEST_ASSERT_TRUE(changed);
  TEST_ASSERT_EQUAL_STRING("Bo", anga.value);
}

void test_guna_blocked_by_kit(void) {
  /* kit suffix (k as it) blocks guṇa per 1.1.5 */
  Term anga; term_init(&anga, "BU", SJ_DHATU | SJ_ANGA);
  Term pratyaya; term_init(&pratyaya, "ktvA", SJ_PRATYAYA | SJ_KIT);
  bool blocked = guna_is_blocked(&pratyaya);
  TEST_ASSERT_TRUE(blocked);
}
```

---

## Acceptance Criteria

- [ ] `guna_apply_to_final("BU", sārvadhatuka)` → "Bo"
- [ ] Kit suffix blocks guṇa (1.1.5)
- [ ] Vṛddhi changes i→ai, u→au
- [ ] All 6 sūtras have associated `rule_fired` global_id output
- [ ] Unit test passes

---
---

# Story 3.5 — Class 1 (Bhvādi) Present Tense Derivation

**Phase:** 3 — Pratyaya System  
**Difficulty:** Very High  
**Estimated time:** 5 hours  
**Depends on:** Story 3.1, 3.2, 3.3, 3.4

---

## Objective

Implement the complete laṭ (present tense) prakriyā for Class 1 (bhvādi) dhātus, producing all 18 forms for given roots. This is the milestone of Phase 3 — the first end-to-end derivation.

---

## Derivation Steps for bhū → bhavati

Full step sequence with sūtra IDs:

```
Step 1: Select root bhū (BU) from Dhātupāṭha gaṇa 1
        → Term[0] = BU, saṃjñā = DHATU

Step 2: 3.2.123: laṭ → assign lakāra
        → Append Term[1] = "laT" (lakāra placeholder)

Step 3: 3.4.77–78: tiṅ replaces laṭ
        → (prathama, ekavacana, parasmai) → Replace Term[1] = "tip"
        → Mark "tip" as TING | SARVADHATUKA

Step 4: 3.1.68: kartari śap — insert vikaraṇa "Sap" between dhātu and tiṅ
        → Term sequence: BU | Sap | tip
        → Sap gets SARVADHATUKA | PRATYAYA

Step 5: 7.3.84: sārvadhātuke guṇaḥ — apply guṇa to dhātu's final vowel
        → BU → Bo (ū → o is guṇa)
        → Now: Bo | Sap | tip

Step 6: Anubandha stripping — Sap → a (strip Ś and p)
        → Now: Bo | a | tip

Step 7: 6.1.78: eco 'yavāyāvaḥ — o + a → ava (yaṆ sandhi)
        → Bo + a → Bav (using 6.1.77 yaṆ for o→av then + a)
        → Now: Bava | ti (after merging Sap into dhātu and tip losing p)

Actually: 6.4.71 lupto vibhaktau — NOT applicable
          Use: 6.1.77: BU + Sap → Bava after guṇa
        → Result: Bavati

Step 8: Final sandhi (if any)
        → "Bavati" — no further sandhi needed

Final: Bavati → IAST: bhavati
```

---

## Files

### `prakriya/tinanta/lat_bhvadi.c`

```c
/**
 * Derive all 18 laṭ forms for a bhvādi (gaṇa 1) root.
 * Returns true if all 18 forms derived successfully.
 */
bool lat_bhvadi_derive_all(const SutraDB *db, const char *dhatu_slp1,
                            ASH_Form forms[18]);

/**
 * Derive a single laṭ form for a bhvādi root.
 */
bool lat_bhvadi_derive_one(const SutraDB *db, const char *dhatu_slp1,
                            ASH_Purusha p, ASH_Vacana v, ASH_Pada pd,
                            PrakriyaCtx *ctx_out);
```

Implement each step as a separate function call with logging:
```c
static bool step_assign_lakara(PrakriyaCtx *ctx, const SutraDB *db);
static bool step_assign_ting(PrakriyaCtx *ctx, const SutraDB *db);
static bool step_insert_sap(PrakriyaCtx *ctx, const SutraDB *db);
static bool step_apply_guna(PrakriyaCtx *ctx, const SutraDB *db);
static bool step_strip_anubandhas(PrakriyaCtx *ctx, const SutraDB *db);
static bool step_apply_sandhi(PrakriyaCtx *ctx, const SutraDB *db);
```

---

## Expected Output for bhū (BU)

| | Ekavacana | Dvivacana | Bahuvacana |
|-|-----------|-----------|------------|
| Prathama P | bhavati | bhavataḥ | bhavanti |
| Madhyama P | bhavasi | bhavathaḥ | bhavatha |
| Uttama P | bhavāmi | bhavāvaḥ | bhavāmaḥ |
| Prathama Ā | bhavate | bhavete | bhavante |
| Madhyama Ā | bhavase | bhavete | bhavádhvam |
| Uttama Ā | bhave | bhavāvahe | bhavāmahe |

(SLP1 equivalents for testing)

## VALIDATION

```bash
python3 tools/run_vyakarana_oracle.py --root BU --gana 1 --lakara LAT
```
Compare output form-by-form. Must match ≥ 16/18 forms exactly.

---

## Unit Test: `tests/unit/test_lat_bhvadi.c`

```c
void test_bhu_prathama_eka(void) {
  SutraDB db; sutra_db_load(&db, "data/sutras.tsv");
  PrakriyaCtx ctx;
  bool ok = lat_bhvadi_derive_one(&db, "BU", ASH_PRATHAMA,
                                   ASH_EKAVACANA, ASH_PARASMAI, &ctx);
  TEST_ASSERT_TRUE(ok);
  char form[64];
  prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("Bavati", form);  /* bhavati in SLP1 */
  sutra_db_free(&db);
}

void test_bhu_all_18(void) {
  /* Load expected from tests/data/lat_bhvadi_expected.tsv */
  ...
}
```

Create `tests/data/lat_bhvadi_expected.tsv` with all 18 expected SLP1 forms for bhū, gam, pat.

---

## Acceptance Criteria

- [ ] `bhū → bhavati` (prathama, ekavacana, parasmai) correct
- [ ] All 18 forms of bhū correct per expected TSV
- [ ] All 18 forms of `gam` (root: gam, gaṇa 1) correct
- [ ] Each step in prakriyā trace has a valid sūtra global_id
- [ ] `run_vyakarana_oracle.py` comparison ≥ 16/18 match

---
---

# Story 3.6 — Vikaraṇa Classes 4, 6, and 10

**Phase:** 3 — Pratyaya System  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 3.5

---

## Objective

Extend the tiṅanta engine to Classes 4 (divādi), 6 (tudādi), and 10 (curādi) — the three most common non-bhvādi classes — completing full laṭ coverage for ~70% of all Sanskrit verbs.

---

## Vikaraṇa Rules

| Gaṇa | Class Name | Vikaraṇa | Sūtra | Notes |
|------|-----------|---------|-------|-------|
| 1 | Bhvādi | śap (a) | 3.1.68 | guṇa applies (done in 3.5) |
| 4 | Divādi | śyan (ya) | 3.1.69 | y inserted; NO guṇa |
| 6 | Tudādi | śa (a) | 3.1.77 | same suffix as bhvādi BUT 7.3.84 is blocked (no guṇa) by 1.1.5? |
| 10 | Curādi | ṇic (ay) | 3.1.25 | causative-like aya, WITH guṇa + vṛddhi |

Wait — Class 6 actually uses the same `śa` vikaraṇa as 1 but guṇa does NOT apply because the root vowel is not eligible (laghu/guru distinction or the suffix is not triggering guṇa under different rule reading). Implement per traditional understanding.

Class 10 (curādi) additionally applies vṛddhi before ṇic.

---

## Files

### `prakriya/tinanta/vikaranas.h`

```c
typedef enum {
  VIKARANA_SAP = 1,   /* śap — gaṇa 1 */
  VIKARANA_SYAN,      /* śyan — gaṇa 4 */
  VIKARANA_SA,        /* śa — gaṇa 6 */
  VIKARANA_NIC,       /* ṇic — gaṇa 10 (causative) */
  VIKARANA_SNU,       /* śnu — gaṇa 5 */
  VIKARANA_U,         /* u — gaṇa 8 */
  VIKARANA_NA,        /* nā — gaṇa 9 */
} VikaranaType;

/** Insert the appropriate vikaraṇa for a given gaṇa */
bool vikarana_insert(PrakriyaCtx *ctx, const SutraDB *db, int gana);

/** Derive all 18 laṭ forms for any supported gaṇa */
bool lat_derive_all(const SutraDB *db, const char *dhatu_slp1, int gana,
                    ASH_Form forms[18]);
```

---

## Expected Forms

| Root | Gaṇa | Prathama-Eka-P | SLP1 |
|------|------|----------------|------|
| div | 4 | dīvyati | dIvyati |
| nṛt | 4 | nṛtyati | nftyati |
| tud | 6 | tudati | tudati |
| liś | 6 | liśati | liSati |
| cur | 10 | corayati | corayati |
| cor | 10 | corayati | (same) |

VALIDATION:
```bash
python3 tools/run_vyakarana_oracle.py --root dIv --gana 4 --lakara LAT
python3 tools/run_vyakarana_oracle.py --root tud --gana 6 --lakara LAT
python3 tools/run_vyakarana_oracle.py --root cur --gana 10 --lakara LAT
```

---

## Acceptance Criteria

- [ ] Class 4: `div → dīvyati` correct (all 18 forms)
- [ ] Class 6: `tud → tudati` correct (all 18 forms) — note: no guṇa
- [ ] Class 10: `cur → corayati` correct (all 18 forms)
- [ ] `make validate-phase3` passes
- [ ] vyakarana oracle ≥ 14/18 match for each tested root
