# Story 1.1 — Varṇa (Phoneme) Type System

**Phase:** 1 — Phonology Engine  
**Difficulty:** Medium  
**Estimated time:** 3 hours  
**Depends on:** Story 0.1 (build system)

---

## Objective

Implement the fundamental phoneme (`varṇa`) data type and classification system. Every subsequent phonological operation (pratyāhāra expansion, sandhi, it-stripping) is built on this.

---

## Background

Sanskrit phonology is fully specified in Pāṇini's grammar via:
- The 14 Māheśvara-sūtras (phoneme inventory and groupings)
- Sūtras 1.1.9–1.1.70 (phonological properties: savarṇa, etc.)

The complete phoneme inventory (52 phonemes + special markers):

**Vowels (ac):** a ā i ī u ū ṛ ṝ ḷ ḹ e ai o au  
**Consonants (hal):** k kh g gh ṅ / c ch j jh ñ / ṭ ṭh ḍ ḍh ṇ / t th d dh n / p ph b bh m / y r l v / ś ṣ s h  
**Special:** ṃ (anusvāra), ḥ (visarga), avagraha

---

## Files to Create

### `core/phonology/varna.h`

```c
#ifndef VARNA_H
#define VARNA_H

#include <stdbool.h>
#include <stdint.h>

/* ── Phoneme flags (bitmask) ─────────────────────────────────────────────── */
#define VF_VOWEL        (1u << 0)   /* ac */
#define VF_CONSONANT    (1u << 1)   /* hal */
#define VF_SHORT        (1u << 2)   /* hrasva */
#define VF_LONG         (1u << 3)   /* dīrgha */
#define VF_PLUTA        (1u << 4)   /* pluta (3x length) */
#define VF_UDATTA       (1u << 5)   /* udātta accent */
#define VF_ANUDATTA     (1u << 6)   /* anudātta accent */
#define VF_SVARITA      (1u << 7)   /* svarita accent */
#define VF_NASAL        (1u << 8)   /* anunaasika / nasal */
#define VF_ASPIRATE     (1u << 9)   /* mahāprāṇa */
#define VF_VOICED       (1u << 10)  /* ghoṣavat */
#define VF_VOICELESS    (1u << 11)  /* aghoṣa */
#define VF_ANUSVARA     (1u << 12)  /* anusvāra ṃ */
#define VF_VISARGA      (1u << 13)  /* visarga ḥ */
#define VF_SEMIVOWEL    (1u << 14)  /* antastha: y r l v */
#define VF_SIBILANT     (1u << 15)  /* ūṣman: ś ṣ s h */

/* Place of articulation */
#define VF_KANTYA       (1u << 16)  /* velar: k-row + a, ā, e, ai, o, au */
#define VF_TALAVYA      (1u << 17)  /* palatal: c-row + i, ī, e, ai */
#define VF_MURDHANYA    (1u << 18)  /* retroflex: ṭ-row + ṛ, ṝ, ṣ */
#define VF_DANTYA       (1u << 19)  /* dental: t-row + s */
#define VF_OSHTHYA      (1u << 20)  /* labial: p-row + u, ū */
#define VF_NASIKA       (1u << 21)  /* nasal stop */

typedef struct {
  char     slp1;     /* SLP1 ASCII representation (single char) */
  uint32_t flags;    /* Bitmask of VF_* constants */
} Varna;

/* ── API ─────────────────────────────────────────────────────────────────── */

/** Get Varṇa from SLP1 character. Returns zeroed struct for unknown char. */
Varna varna_from_slp1(char c);

/** Get flags for SLP1 character (convenience). */
uint32_t varna_flags(char slp1);

/** 1.1.9: tulyāsyaprayatnaṃ savarṇam — are two phonemes savarṇa? */
bool varna_is_savarna(char a, char b);

/* Classification predicates */
bool varna_is_vowel(char slp1);       /* ac pratyāhāra */
bool varna_is_consonant(char slp1);   /* hal pratyāhāra */
bool varna_is_short(char slp1);       /* hrasva */
bool varna_is_long(char slp1);        /* dīrgha */
bool varna_is_nasal(char slp1);       /* nasal stop (ṅ ñ ṇ n m) */
bool varna_is_semivowel(char slp1);   /* antastha: y r l v */
bool varna_is_sibilant(char slp1);    /* ūṣman: ś ṣ s h */
bool varna_is_voiced(char slp1);
bool varna_is_voiceless(char slp1);
bool varna_is_aspirate(char slp1);

/** Get the savarṇa class representative (= short vowel or first class member) */
char varna_savarna_rep(char slp1);

/** Get the corresponding long vowel for a short vowel (for dīrgha sandhi) */
char varna_lengthen(char short_vowel);

/** Get the corresponding short vowel for a long vowel */
char varna_shorten(char long_vowel);

/** Get the guna of a vowel (1.1.2: a e o = guṇa of a i u) */
char varna_guna(char vowel);

/** Get the vṛddhi of a vowel */
char varna_vrddhi(char vowel);

/** Is this char a valid SLP1 phoneme at all? */
bool varna_is_valid_slp1(char c);

#endif /* VARNA_H */
```

### `core/phonology/varna.c`

Implement the full phoneme table as a static const array of 52+ `Varna` entries, one per SLP1 character. Every `varna_*` function looks up this table.

#### Complete phoneme table (must implement all):

| SLP1 | Phoneme | Key flags |
|------|---------|-----------|
| `a` | a | VOWEL, SHORT, KANTYA |
| `A` | ā | VOWEL, LONG, KANTYA |
| `i` | i | VOWEL, SHORT, TALAVYA |
| `I` | ī | VOWEL, LONG, TALAVYA |
| `u` | u | VOWEL, SHORT, OSHTHYA |
| `U` | ū | VOWEL, LONG, OSHTHYA |
| `f` | ṛ | VOWEL, SHORT, MURDHANYA |
| `F` | ṝ | VOWEL, LONG, MURDHANYA |
| `x` | ḷ | VOWEL, SHORT, DANTYA |
| `X` | ḹ | VOWEL, LONG, DANTYA |
| `e` | e | VOWEL, LONG, TALAVYA+KANTYA |
| `E` | ai | VOWEL, LONG, TALAVYA+KANTYA |
| `o` | o | VOWEL, LONG, OSHTHYA+KANTYA |
| `O` | au | VOWEL, LONG, OSHTHYA+KANTYA |
| `k` | k | CONSONANT, VOICELESS, KANTYA |
| `K` | kh | CONSONANT, VOICELESS, ASPIRATE, KANTYA |
| `g` | g | CONSONANT, VOICED, KANTYA |
| `G` | gh | CONSONANT, VOICED, ASPIRATE, KANTYA |
| `N` | ṅ | CONSONANT, NASAL, NASIKA, KANTYA |
| `c` | c | CONSONANT, VOICELESS, TALAVYA |
| `C` | ch | CONSONANT, VOICELESS, ASPIRATE, TALAVYA |
| `j` | j | CONSONANT, VOICED, TALAVYA |
| `J` | jh | CONSONANT, VOICED, ASPIRATE, TALAVYA |
| `Y` | ñ | CONSONANT, NASAL, NASIKA, TALAVYA |
| `w` | ṭ | CONSONANT, VOICELESS, MURDHANYA |
| `W` | ṭh | CONSONANT, VOICELESS, ASPIRATE, MURDHANYA |
| `q` | ḍ | CONSONANT, VOICED, MURDHANYA |
| `Q` | ḍh | CONSONANT, VOICED, ASPIRATE, MURDHANYA |
| `R` | ṇ | CONSONANT, NASAL, NASIKA, MURDHANYA |
| `t` | t | CONSONANT, VOICELESS, DANTYA |
| `T` | th | CONSONANT, VOICELESS, ASPIRATE, DANTYA |
| `d` | d | CONSONANT, VOICED, DANTYA |
| `D` | dh | CONSONANT, VOICED, ASPIRATE, DANTYA |
| `n` | n | CONSONANT, NASAL, NASIKA, DANTYA |
| `p` | p | CONSONANT, VOICELESS, OSHTHYA |
| `P` | ph | CONSONANT, VOICELESS, ASPIRATE, OSHTHYA |
| `b` | b | CONSONANT, VOICED, OSHTHYA |
| `B` | bh | CONSONANT, VOICED, ASPIRATE, OSHTHYA |
| `m` | m | CONSONANT, NASAL, NASIKA, OSHTHYA |
| `y` | y | CONSONANT, SEMIVOWEL, VOICED, TALAVYA |
| `r` | r | CONSONANT, SEMIVOWEL, VOICED, MURDHANYA |
| `l` | l | CONSONANT, SEMIVOWEL, VOICED, DANTYA |
| `v` | v | CONSONANT, SEMIVOWEL, VOICED, OSHTHYA |
| `S` | ś | CONSONANT, SIBILANT, VOICELESS, TALAVYA |
| `z` | ṣ | CONSONANT, SIBILANT, VOICELESS, MURDHANYA |
| `s` | s | CONSONANT, SIBILANT, VOICELESS, DANTYA |
| `h` | h | CONSONANT, SIBILANT, VOICED, KANTYA |
| `M` | ṃ | ANUSVARA |
| `H` | ḥ | VISARGA |

#### savarṇa implementation (1.1.9)

Two phonemes are savarṇa if they share the same:
1. Place of articulation (āsya / same place flag)
2. Effort (prayatna: same voicing, aspiration, nasality)

Special: all vowels a/ā are savarṇa; i/ī savarṇa; u/ū savarṇa; ṛ/ṝ savarṇa; ḷ/ḹ savarṇa.

#### guṇa mapping (1.1.2: adeṅ guṇaḥ)
```
a/ā → a (already guṇa)
i/ī → e
u/ū → o
ṛ/ṝ → ar (represented as "ar" — caller handles the two-char case)
ḷ   → al
```

#### vṛddhi mapping (1.1.1: vṛddhirādaic)
```
a/ā → ā
i/ī → ai (E)
u/ū → au (O)
ṛ/ṝ → ār
ḷ   → āl
```

---

## Unit Test: `tests/unit/test_varna.c`

```c
#include "unity.h"
#include "varna.h"

void test_vowel_classification(void) {
  TEST_ASSERT_TRUE(varna_is_vowel('a'));
  TEST_ASSERT_TRUE(varna_is_vowel('A'));
  TEST_ASSERT_TRUE(varna_is_vowel('i'));
  TEST_ASSERT_TRUE(varna_is_vowel('e'));
  TEST_ASSERT_TRUE(varna_is_vowel('O'));  /* au */
  TEST_ASSERT_FALSE(varna_is_vowel('k'));
  TEST_ASSERT_FALSE(varna_is_vowel('M')); /* anusvara — not ac */
}

void test_consonant_classification(void) {
  TEST_ASSERT_TRUE(varna_is_consonant('k'));
  TEST_ASSERT_TRUE(varna_is_consonant('y'));  /* semivowel is hal */
  TEST_ASSERT_FALSE(varna_is_consonant('a'));
}

void test_savarna(void) {
  /* 1.1.9 */
  TEST_ASSERT_TRUE(varna_is_savarna('a', 'A'));  /* a and ā */
  TEST_ASSERT_TRUE(varna_is_savarna('i', 'I'));  /* i and ī */
  TEST_ASSERT_TRUE(varna_is_savarna('k', 'K'));  /* k and kh — same place */
  TEST_ASSERT_FALSE(varna_is_savarna('a', 'i')); /* different place */
  TEST_ASSERT_FALSE(varna_is_savarna('k', 'g')); /* k unvoiced, g voiced */
}

void test_guna(void) {
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('a'));
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('A'));
  TEST_ASSERT_EQUAL_CHAR('e', varna_guna('i'));
  TEST_ASSERT_EQUAL_CHAR('e', varna_guna('I'));
  TEST_ASSERT_EQUAL_CHAR('o', varna_guna('u'));
  TEST_ASSERT_EQUAL_CHAR('o', varna_guna('U'));
}

void test_vrddhi(void) {
  TEST_ASSERT_EQUAL_CHAR('A', varna_vrddhi('a'));
  TEST_ASSERT_EQUAL_CHAR('E', varna_vrddhi('i'));  /* ai */
  TEST_ASSERT_EQUAL_CHAR('O', varna_vrddhi('u'));  /* au */
}

void test_lengthen_shorten(void) {
  TEST_ASSERT_EQUAL_CHAR('A', varna_lengthen('a'));
  TEST_ASSERT_EQUAL_CHAR('I', varna_lengthen('i'));
  TEST_ASSERT_EQUAL_CHAR('a', varna_shorten('A'));
}

/* ... 40+ more tests covering all 52 phonemes */
```

Must cover:
- All 14 vowels: classification, guṇa, vṛddhi
- All 25 stops: voicing, aspiration, place
- All 4 semivowels
- All 4 sibilants + h
- Anusvāra and visarga: not vowel, not consonant (special)
- 10 savarṇa pairs, 10 non-savarṇa pairs

---

## Acceptance Criteria

- [ ] All 52 phonemes defined in the table
- [ ] `varna_is_vowel` / `varna_is_consonant` correct for all entries
- [ ] `varna_is_savarna` correct for 20 test pairs
- [ ] `varna_guna` and `varna_vrddhi` correct for all 14 vowels
- [ ] Unit test `test_varna` passes with 0 failures
- [ ] No compiler warnings
