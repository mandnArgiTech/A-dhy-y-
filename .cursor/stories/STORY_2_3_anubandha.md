# Story 2.3 — Anubandha (It-Letter) Stripping

**Phase:** 2 — Sūtra Engine & Saṃjñā System  
**Difficulty:** High  
**Estimated time:** 3 hours  
**Depends on:** Story 2.2, Story 1.1

---

## Objective

Implement anubandha (it-letter) identification and stripping per sūtras 1.3.2–1.3.9. Every dhātu in the Dhātupāṭha and every pratyaya appears in "upadesa" form — the canonical form with marker letters that are stripped before phonological processing.

---

## Background

Pāṇini uses special marker letters (anubandha / it) to encode grammatical information compactly. Rules 1.3.2–1.3.9 define which letters in an upadesa are it-letters.

| Sūtra | Rule | Example |
|-------|------|---------|
| 1.3.2 | nasalized vowels are it | bhU (ū̃ = U-nasal) |
| 1.3.3 | final consonant is it (general) | BU → BU, 'U' is it |
| 1.3.4 | not final t/th/d/dh/n/s/h in vibhakti suffixes | exception |
| 1.3.5 | initial ñ/i is it in certain contexts | |
| 1.3.6 | ṣ is it | |
| 1.3.7 | c/ñ are it in niṣṭhā | |
| 1.3.8 | l is it | |
| 1.3.9 | (final result: strip all it-letters) | |

Key examples:
- `BU` (bhū) → clean = `BU`, it = {none} (ū is not stripped — it's the root)
- `qukf` (ḍukṛ) → clean = `kf`, it = {qu} (ḍu is it-prefix)
- `Sa` (śa, certain suffixes) → it = {Ś,a}
- `Sap` (śap, class 1 vikaraṇa) → it = {Ś,p}, clean = `a`
- `tip` → it = {p}, clean = `ti`
- `zvu` → it = {zv}, clean = `u` → applied as 'uv'

---

## Files

### `core/metadata/anubandha.h`

```c
#ifndef ANUBANDHA_H
#define ANUBANDHA_H

#include "samjna.h"
#include <stdint.h>
#include <stdbool.h>

/* Which positions can have it-letters */
#define IT_POS_INITIAL  0x01  /* ādya-it */
#define IT_POS_FINAL    0x02  /* anta-it */
#define IT_POS_MEDIAL   0x04  /* (rare) */

typedef struct {
  char     it_chars[16];   /* SLP1 chars identified as it, null-terminated */
  int      it_count;
  char     clean_slp1[64]; /* Form with all it-letters stripped */
  Samjna   it_samjna;      /* Saṃjñās implied by the it-letters */
} AnubandhaResult;

/**
 * Strip anubandhas from an upadesa form.
 * @param upadesa_slp1  The upadesa (with it-markers), SLP1
 * @param context       SJ_PRATYAYA | SJ_DHATU | SJ_VIBHAKTI etc. (affects rules)
 * @param result        Output: filled with stripped form and it info
 */
void anubandha_strip(const char *upadesa_slp1, Samjna context,
                     AnubandhaResult *result);

/** Check if a specific it-letter is present in an upadesa. */
bool anubandha_has_it(const char *upadesa_slp1, char it_char, Samjna context);

/**
 * Determine the saṃjñā implied by having a given it-letter:
 *   k → SJ_KIT, ṅ → SJ_NGIT, ṇ → SJ_NNIT, p → SJ_PIT, etc.
 */
Samjna it_to_samjna(char it_char);

#endif /* ANUBANDHA_H */
```

### `core/metadata/anubandha.c`

Implement the 8-rule decision process. Apply rules in order 1.3.2 → 1.3.9.

Key it-letter implications:
- `k` as it → SJ_KIT (blocks guṇa by 1.1.5)
- `N` (ṅ) as it → SJ_NGIT (ātmanepada)
- `R` (ṇ) as it → SJ_NNIT (vṛddhi by 7.2.115)
- `p` as it → SJ_PIT (takes guṇa)
- `S` (ś) as it → śit (sārvadhatuka marker)
- `n` as it → anit (no iṭ augment)
- `s` as it → set (takes iṭ augment)

---

## Unit Test: `tests/unit/test_anubandha.c`

```c
void test_dhatu_bhu(void) {
  AnubandhaResult r;
  anubandha_strip("BU", SJ_DHATU, &r);
  TEST_ASSERT_EQUAL_STRING("BU", r.clean_slp1);
  TEST_ASSERT_EQUAL_INT(0, r.it_count);
}

void test_dhatu_dukr(void) {
  /* qukf → clean=kf, it={q,u} */
  AnubandhaResult r;
  anubandha_strip("qukf", SJ_DHATU, &r);
  TEST_ASSERT_EQUAL_STRING("kf", r.clean_slp1);
  TEST_ASSERT_EQUAL_INT(2, r.it_count);
}

void test_pratyaya_sap(void) {
  /* Sap → clean=a, it={S,p} */
  AnubandhaResult r;
  anubandha_strip("Sap", SJ_PRATYAYA, &r);
  TEST_ASSERT_EQUAL_STRING("a", r.clean_slp1);
  TEST_ASSERT_TRUE(samjna_has(r.it_samjna, SJ_SARVADHATUKA));
}

void test_pratyaya_tip(void) {
  /* tip → clean=ti, it={p} */
  AnubandhaResult r;
  anubandha_strip("tip", SJ_PRATYAYA | SJ_TING, &r);
  TEST_ASSERT_EQUAL_STRING("ti", r.clean_slp1);
}
```

---

## Acceptance Criteria

- [ ] All 8 anubandha rules implemented (1.3.2–1.3.9)
- [ ] `BU` → no it-letters, clean=`BU`
- [ ] `qukf` → clean=`kf`, it=[q,u]
- [ ] `Sap` → clean=`a`, saṃjñā includes SARVADHATUKA
- [ ] `tip` → clean=`ti`, it=[p]
- [ ] Unit test passes with 0 failures
