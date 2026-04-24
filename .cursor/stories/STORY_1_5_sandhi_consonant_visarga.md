# Story 1.5 — Sandhi Engine: Consonant & Visarga Sandhi

**Phase:** 1 — Phonology Engine  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 1.4

---

## Objective

Implement hal-sandhi (consonant sandhi) and visarga-sandhi — the transformations at consonant + consonant and visarga + vowel/consonant boundaries. Rules from Aṣṭādhyāyī 8.2–8.4.

---

## Background

### Consonant Sandhi (hal-sandhi, 8.4.40–8.4.68)

Final stops assimilate in voicing to the following phoneme:
- Before voiced: final stop becomes voiced (jaś-sandhi: 8.4.53)
- Before voiceless: final stop becomes voiceless (car-sandhi: 8.4.55)
- Before nasal: final stop becomes the nasal of its own class (8.4.45)
- Anusvāra before consonants (8.3.23)

### Visarga Sandhi (8.3.15–8.3.37)

- ḥ + voiced consonant → r (8.3.15: haraḥ)
- ḥ + voiceless consonant → ḥ (stays)
- aḥ + vowel → o (8.3.15: special)
- āḥ + any → ā (long ā, visarga dropped: 8.3.17)
- iḥ/uḥ + voiced → ir/ur (8.3.15)

---

## Files to Create

### `sandhi/sandhi_hal.h`

```c
#ifndef SANDHI_HAL_H
#define SANDHI_HAL_H

#include "sandhi_vowel.h"  /* for SandhiResult */

/** Apply consonant sandhi at junction of a_final + b_initial.
 *  Returns the transformed final consonant(s) in result_slp1. */
SandhiResult sandhi_hal_apply(char a_final, char b_initial);

/** Full-word consonant sandhi join. */
bool sandhi_hal_join(const char *a, const char *b, char *out, size_t len);

#endif
```

### `sandhi/sandhi_hal.c`

Implement these rules with their sūtra IDs:

```
8.2.30  — coḥ kuḥ: c/j + pause → k (palatals become velars at pause)
8.2.39  — jhalopau tau: (special cases)
8.3.15  — kharavasānayor visarjanīyaḥ: visarga before voiceless
8.3.23  — moʾnusvāraḥ: final m before consonant → anusvāra (ṃ)
8.4.41  — yaro 'nunāsikān nunāsiko 'nādeśe: final stop + nasal → nasal of same class
8.4.45  — yaro 'nunāsikān (fuller statement)
8.4.53  — jhalopajhaś jashi: jhal before jaŚ → jaŚ (voice assimilation)
           g/k → g; d/t → d; b/p → b; ḍ/ṭ → ḍ; etc.
8.4.54  — abhyāse car ca: reduplication uses car (voiceless)
8.4.55  — khari ca: jhal before khar → car (devoicing)
           g→k, d→t, b→p, ḍ→ṭ, etc.
8.4.62  — (śar → ṣar: retroflexion)
```

#### jaŚ class (voiced non-aspirate stops: j b g ḍ d) — SLP1: j b g q d
#### car class (voiceless non-aspirate stops: k c ṭ t p + sibilants)

Voicing assimilation table:
```c
/* jhal → jaŚ (before voiced) */
static const char JAL_TO_JASH[] = {
  /* k→g, K→G, g→g, G→G, c→j, C→J, j→j, J→J,
     w→q, W→Q, q→q, Q→Q, t→d, T→D, d→d, D→D,
     p→b, P→B, b→b, B→B */
};

/* jhal → car (before voiceless) */
static const char JAL_TO_CAR[] = {
  /* k→k, K→k, g→k, G→k, c→c, C→c, j→c, J→c,
     w→w, W→w, q→w, Q→w, t→t, T→t, d→t, D→t,
     p→p, P→p, b→p, B→p */
};
```

### `sandhi/sandhi_visarga.h` + `sandhi/sandhi_visarga.c`

```c
/** Apply visarga sandhi.
 *  @param a_final    Must be 'H' (visarga) — called only when a ends in ḥ
 *  @param a_prefinal Character before visarga (to determine a+ā class)
 *  @param b_initial  First char of following word
 */
SandhiResult sandhi_visarga_apply(char a_prefinal, char b_initial);
```

Implement:
```
aH + voiced consonant/vowel → o  (8.3.15: aH + voiced → o)
aH + a → o'  (8.3.15 + 6.1.109)
AH + any → A  (8.3.17: long ā absorbs visarga)
iH/uH + voiced → ir/ur  (8.3.15)
iH/uH + voiceless → iH/uH  (stays)
H  + voiceless khar → H  (8.3.15)
H  + voiced → r  (8.3.15 general)
```

---

## Test Data: `tests/data/sandhi_hal_cases.tsv`

```
# a_final  b_initial  expected  rule    comment
k          a          ga        8.4.53  g+vowel
k          k          kk        8.4.55  stays voiceless
g          t          kt        8.4.55  g→k before voiceless t
t          d          dd        8.4.53  t→d before voiced d
p          m          bm        8.4.53  p→b before nasal m
m          k          Mk        8.3.23  m→anusvāra before consonant
```

`tests/data/sandhi_visarga_cases.tsv`:
```
# a_prefinal  b_initial  expected  rule    comment
a             b          ob        8.3.15  aḥ + voiced → o
a             a          oa        8.3.15  aḥ + a → o + a (special)
A             k          A         8.3.17  āḥ → ā
i             g          irg       8.3.15  iḥ → ir
i             k          iHk       8.3.15  iḥ stays before voiceless
```

## Unit Test: `tests/unit/test_sandhi_hal.c`

```c
void test_jhal_before_voiced(void) {
  /* 8.4.53: k before voiced → g */
  SandhiResult r = sandhi_hal_apply('k', 'a');
  TEST_ASSERT_EQUAL_STRING("g", r.result_slp1);
}

void test_jhal_before_voiceless(void) {
  /* 8.4.55: g before voiceless → k */
  SandhiResult r = sandhi_hal_apply('g', 't');
  TEST_ASSERT_EQUAL_STRING("k", r.result_slp1);
}

void test_m_anusvara(void) {
  /* 8.3.23: m + consonant → anusvāra */
  char out[32];
  sandhi_hal_join("tam", "ca", out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("taMca", out);
}

void test_visarga_before_voiced(void) {
  SandhiResult r = sandhi_visarga_apply('a', 'b');
  TEST_ASSERT_EQUAL_STRING("ob", r.result_slp1);
}
```

---

## Acceptance Criteria

- [ ] jaŚ assimilation (8.4.53) correct for all 5 voiced stop classes
- [ ] car devoicing (8.4.55) correct for all 5 voiceless classes
- [ ] Anusvāra insertion (8.3.23) for m + consonant
- [ ] Visarga rules (8.3.15, 8.3.17) correct for a/ā/i/u finals
- [ ] All TSV test cases pass
- [ ] Unit test 0 failures
- [ ] `make validate-phase1` green
