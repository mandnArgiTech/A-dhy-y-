# Story 1.4 — Sandhi Engine: Vowel Sandhi (ac-sandhi)

**Phase:** 1 — Phonology Engine  
**Difficulty:** High  
**Estimated time:** 4 hours  
**Depends on:** Story 1.1, 1.2

---

## Objective

Implement vowel sandhi — the phonological transformations that occur at morpheme boundaries when a vowel-final form meets a vowel-initial form. These rules are from Aṣṭādhyāyī 6.1.66–6.1.111.

---

## Background

Vowel sandhi is pervasive in Sanskrit. Six main rules cover ~95% of cases:

| Rule | Sūtra | Name | Input | Output | Example |
|------|-------|------|-------|--------|---------|
| Savarṇadīrgha | 6.1.101 | like + like → long | a + a | ā | rāma + astu → rāmāstu |
| Guṇa | 6.1.87 | front vowel after a/ā | a + i/ī | e | ca + iti → ceti |
| Vṛddhi | 6.1.88 | | a + e/ai | ai | ca + eva → caiva |
| Vṛddhi | 6.1.89 | | a + o/au | au | ca + ojas → caujas |
| YaṆ | 6.1.77 | ik vowel before vowel | i/ī + V | y + V | iti + api → ityapi |
| AyāVadi | 6.1.78 | eC before vowel | e + V | ay + V | ne + ati → nayati |
| Prakṛtibhāva | 6.1.125 | No sandhi (exceptions) | | | (Vedic) |

---

## Files to Create

### `sandhi/sandhi_vowel.h`

```c
#ifndef SANDHI_VOWEL_H
#define SANDHI_VOWEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Result of a sandhi operation */
typedef struct {
  char   result_slp1[32];  /* Resulting string in SLP1 */
  uint32_t rule_applied;   /* global_id of sūtra applied, 0 if none */
  bool   changed;          /* Was any change made? */
} SandhiResult;

/**
 * Apply vowel sandhi at the junction of string a (ending) and b (beginning).
 * Only looks at the LAST character of a and FIRST character of b.
 * Fills result with the junction string.
 *
 * Example: a="rAma", b="asti"
 *   → last(a)='a', first(b)='a'
 *   → 6.1.101 fires → result="A" (the junction)
 *   → caller reconstructs "rAmAsti"
 */
SandhiResult sandhi_vowel_apply(char a_final, char b_initial);

/**
 * Apply vowel sandhi across a full word boundary.
 * @param a    First word (SLP1)
 * @param b    Second word (SLP1)
 * @param out  Output buffer
 * @param len  Output buffer length
 * @return true if any change was made
 */
bool sandhi_vowel_join(const char *a, const char *b, char *out, size_t len);

/**
 * Split a vowel-sandhied junction back into components.
 * Returns the number of splits found (can be 0 if no sandhi detected).
 */
typedef struct { char a_end; char b_start; uint32_t rule; } SandhiSplit;
int sandhi_vowel_split(char junction, SandhiSplit *splits, int max_splits);

#endif /* SANDHI_VOWEL_H */
```

### `sandhi/sandhi_vowel.c`

Implement the full decision table for vowel sandhi. The key sūtras to implement (with their global_id from `data/sutras.tsv`):

#### Rule table (implement ALL of these)

```
6.1.66  — lopaḥ śākalyasya (Vedic elision, optional)
6.1.77  — iko yaṇ aci: ik + vowel → corresponding semivowel
           i/ī → y, u/ū → v, ṛ/ṝ → r, ḷ → l
6.1.78  — eco 'yavāyāvaḥ: eC + vowel → ay/āy/av/āv
           e → ay, ai → āy, o → av, au → āv
6.1.87  — ādguṇaḥ: a/ā + ik vowel → guṇa vowel
           a+i → e, a+u → o, a+ṛ → ar, a+ḷ → al
6.1.88  — vṛddhireci: a/ā + eC → vṛddhi
           a+e → ai, a+ai → ai, a+o → au, a+au → au
6.1.101 — akaḥ savarṇe dīrghaḥ: like vowel + like vowel → long
           a+a → ā, i+i → ī, u+u → ū, ṛ+ṛ → ṝ
6.1.102 — prathama-dvitīyayoḥ pūrvasya (special cases)
6.1.109 — eṅaḥ padāntādati (e/o final before a: e stays, a is elided — Vedic)
6.1.125 — prakṛtibhāvaḥ (no sandhi in certain Vedic contexts)
```

Priority when multiple rules could apply:
1. 6.1.101 (savarṇadīrgha) is most specific — fire first if applicable
2. 6.1.77 (yaṆ) for ik vowels
3. 6.1.78 (ayāvadi) for eC vowels
4. 6.1.87 (guṇa) for a + ik
5. 6.1.88 (vṛddhi) for a + eC

---

## Test Data

Create `tests/data/sandhi_vowel_cases.tsv`:
```
# a_final  b_initial  expected_junction  rule_id  comment
a          a          A                  6.1.101  savarṇadīrgha
A          a          A                  6.1.101
a          i          e                  6.1.87   guṇa
a          I          e                  6.1.87
a          u          o                  6.1.87
a          U          o                  6.1.87
a          f          ar                 6.1.87   ṛ → ar
A          i          e                  6.1.87
a          e          E                  6.1.88   vṛddhi
a          E          E                  6.1.88
a          o          O                  6.1.88
a          O          O                  6.1.88
i          a          ya                 6.1.77   yaṆ
I          a          ya                 6.1.77
u          a          va                 6.1.77
U          a          va                 6.1.77
f          a          ra                 6.1.77
e          a          aya                6.1.78   ayāvadi
E          a          Aya                6.1.78
o          a          ava                6.1.78
O          a          Ava                6.1.78
i          i          I                  6.1.101  savarṇa
u          u          U                  6.1.101
```

Load this file in the test and assert each case.

## Unit Test: `tests/unit/test_sandhi_vowel.c`

```c
void test_savarna_dirgha(void) {
  SandhiResult r = sandhi_vowel_apply('a', 'a');
  TEST_ASSERT_EQUAL_STRING("A", r.result_slp1);
  TEST_ASSERT_TRUE(r.changed);
}

void test_guna(void) {
  SandhiResult r = sandhi_vowel_apply('a', 'i');
  TEST_ASSERT_EQUAL_STRING("e", r.result_slp1);
}

void test_yan(void) {
  SandhiResult r = sandhi_vowel_apply('i', 'a');
  TEST_ASSERT_EQUAL_STRING("ya", r.result_slp1);

  r = sandhi_vowel_apply('u', 'e');
  TEST_ASSERT_EQUAL_STRING("ve", r.result_slp1);
}

void test_join_full_words(void) {
  char out[64];
  sandhi_vowel_join("rAma", "asti", out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("rAmAsti", out);

  sandhi_vowel_join("ca", "iti", out, sizeof(out));
  TEST_ASSERT_EQUAL_STRING("ceti", out);
}

/* Load and run all cases from tests/data/sandhi_vowel_cases.tsv */
void test_full_case_table(void) { ... }
```

---

## Validation

Run oracle comparison:
```bash
python3 tools/run_sandhi_oracle.py  # calls samsaadhanii/scl sandhi tool
```
Expected: ≥ 95% match rate on 200 test cases.

---

## Acceptance Criteria

- [ ] All 8 major vowel sandhi rules implemented and traced to sūtra number
- [ ] TSV test table (all 23 rows) passes
- [ ] `sandhi_vowel_join("rAma", "asti", ...)` = "rAmAsti"
- [ ] `sandhi_vowel_join("ca", "iti", ...)` = "ceti"
- [ ] Unit test 0 failures
- [ ] Each SandhiResult contains the sūtra `global_id` of the rule applied
