# Story 1.2 — Māheśvara-Sūtras & Pratyāhāra Engine

**Phase:** 1 — Phonology Engine  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 1.1

---

## Objective

Implement the pratyāhāra (phoneme-class shorthand) system — the most ingenious technical device in the Aṣṭādhyāyī. Pāṇini compresses phoneme classes like "all vowels" or "all consonants" into 2-letter abbreviations using the 14 Māheśvara-sūtras.

---

## Background: How Pratyāhāras Work

The 14 Māheśvara-sūtras list all Sanskrit phonemes in a specific order:
```
1.  a  i  u  Ṇ
2.  ṛ  ḷ  K
3.  e  o  Ṅ
4.  ai  au  C
5.  h  y  v  r  Ṭ
6.  l  Ṇ
7.  ñ  m  ṅ  ṇ  n  M
8.  jh  bh  Ñ
9.  gh  ḍh  dh  Ṣ
10. j  b  g  ḍ  d  Ś
11. kh  ph  ch  ṭh  th  c  ṭ  t  V
12. k  p  Y
13. ś  ṣ  s  R
14. h  L
```
(Capital letters are **it** — marker phonemes, not part of the language; they are stripped.)

A **pratyāhāra** is formed by: first phoneme of a range + the it-marker of the sūtra where the range ends.

Example: `ac` = from `a` (sūtra 1) to `c` (it of sūtra 4) = a i u ṛ ḷ e o ai au = all vowels.
Example: `hal` = from `h` (sūtra 5) to `l` (it of sūtra 14) = all consonants.
Example: `ik` = from `i` (sūtra 1) to `k` (it of sūtra 2) = i u ṛ ḷ.

The 44 classical pratyāhāras used in the Aṣṭādhyāyī:

| Pratyāhāra | SLP1 | Expansion |
|-----------|------|-----------|
| aṆ | aN | a i u ṛ ḷ e o ai au (all vowels except pluta) |
| ac | ac | a i u ṛ ḷ e o ai au (same as aN) |
| aK | ak | a i u ṛ ḷ |
| iK | ik | i u ṛ ḷ |
| uK | uk | u ṛ ḷ |
| eṄ | eN | e o |
| eC | ec | e o ai au |
| aiC | aic | ai au |
| hal | hal | all consonants |
| yaR | yar | y r l v ñ m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d |
| yaY | yay | y r l v ñ m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p |
| maY | may | m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p |
| jaŚ | jaS | j b g ḍ d |
| baŚ | baS | b g ḍ d |
| jaR | jar | j b g ḍ d kh ph ch ṭh th c ṭ t k p ś ṣ s h |
| caR | car | kh ph ch ṭh th c ṭ t k p ś ṣ s h |
| jhaL | jhal | jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p ś ṣ s h |
| jhaŚ | jhaS | jh bh gh ḍh dh j b g ḍ d |
| jhaY | jhay | jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p |
| śaR | Sar | ś ṣ s r |
| śaL | Sal | ś ṣ s h |
| aŚ | aS | a i u ṛ ḷ e o ai au y r l v ñ m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d |
| aT | at | a i u ṛ ḷ e o ai au h y v r |
| iN | iR | i u ṛ ḷ e o ai au h y v r l ñ m ṅ ṇ n |
| ñaM | YaM | ñ m ṅ ṇ n |
| ñaY | Yay | ñ m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p |

(The remaining pratyāhāras follow the same pattern — implement all 44.)

---

## Files to Create

### `core/phonology/pratyahara.h`

```c
#ifndef PRATYAHARA_H
#define PRATYAHARA_H

#include "varna.h"
#include <stdbool.h>

/* Maximum phonemes in any pratyāhāra expansion */
#define PRATYAHARA_MAX_MEMBERS 52

/** A pratyāhāra with its expansion */
typedef struct {
  char label[8];            /* SLP1 label, e.g. "ac", "hal", "ik" */
  char members[PRATYAHARA_MAX_MEMBERS]; /* SLP1 chars, null-terminated */
  int  count;               /* Number of members */
} Pratyahara;

/**
 * Expand a pratyāhāra label to its member phonemes.
 * @return Pointer to static Pratyahara, or NULL if label unknown.
 */
const Pratyahara *pratyahara_get(const char *label_slp1);

/** Check if SLP1 phoneme c belongs to pratyāhāra label. */
bool pratyahara_contains(const char *label_slp1, char c);

/** Print expansion to stdout (debug utility). */
void pratyahara_print(const Pratyahara *p);

/** Get all known pratyāhāras (for iteration). */
const Pratyahara *pratyahara_all(int *count_out);

#endif /* PRATYAHARA_H */
```

### `core/phonology/pratyahara.c`

Implement using a static table. The Māheśvara-sūtras define the ordering; the pratyāhāra table is derived from them at compile time (hardcode the results — they are fixed forever).

Algorithm to compute expansion for label "XY":
1. Find position of phoneme X in the Māheśvara-sūtra sequence (ignoring it-letters).
2. Find position of it-marker Y in the sequence.
3. Collect all real phonemes from X's position up to and including Y's position.

**Hardcode the Māheśvara sequence** (real phonemes only, in order):
```c
static const char MAHESHVARA_SEQ[] =
  "aiufxeoEOhyvr" "l" "YmGRnJBGQDjbgqd" "KPCWTcwt" "kp" "Szs" "hl";
```

**Hardcode the 44 pratyāhāras** as a static array initialized from the algorithm. This makes lookups O(1).

---

## Unit Test: `tests/unit/test_pratyahara.c`

```c
void test_ac_expansion(void) {
  const Pratyahara *p = pratyahara_get("ac");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(14, p->count);  /* all vowels */
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'a'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'E'));  /* ai */
  TEST_ASSERT_FALSE(pratyahara_contains("ac", 'k'));
}

void test_hal_expansion(void) {
  const Pratyahara *p = pratyahara_get("hal");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(38, p->count);  /* all consonants */
  TEST_ASSERT_TRUE(pratyahara_contains("hal", 'k'));
  TEST_ASSERT_TRUE(pratyahara_contains("hal", 'h'));
  TEST_ASSERT_FALSE(pratyahara_contains("hal", 'a'));
}

void test_ik_expansion(void) {
  /* ik = i u ṛ ḷ */
  const Pratyahara *p = pratyahara_get("ik");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(4, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'i'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'u'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'f'));  /* ṛ */
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'x'));  /* ḷ */
  TEST_ASSERT_FALSE(pratyahara_contains("ik", 'a'));
}

void test_jash_expansion(void) {
  /* jaŚ = j b g ḍ d (5 voiced non-aspirate stops) */
  const Pratyahara *p = pratyahara_get("jaS");
  TEST_ASSERT_EQUAL_INT(5, p->count);
}

void test_unknown_label(void) {
  TEST_ASSERT_NULL(pratyahara_get("zz"));
}

/* Test all 44 pratyāhāras have non-zero count */
void test_all_pratyaharas_defined(void) {
  int count;
  const Pratyahara *all = pratyahara_all(&count);
  TEST_ASSERT_EQUAL_INT(44, count);
  for (int i = 0; i < count; i++) {
    TEST_ASSERT_GREATER_THAN(0, all[i].count);
  }
}
```

---

## Acceptance Criteria

- [ ] All 44 classical pratyāhāras defined and returning correct expansions
- [ ] `ac` = 14 members (all vowels)
- [ ] `hal` = 38 members (all consonants)
- [ ] `ik` = 4 members (i u ṛ ḷ)
- [ ] `jaŚ` / `"jaS"` = 5 members (j b g ḍ d)
- [ ] Cross-reference: every member returned by `pratyahara_get("ac")` returns `true` for `varna_is_vowel()`
- [ ] Unit test passes with 0 failures
