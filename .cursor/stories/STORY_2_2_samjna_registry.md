# Story 2.2 — Saṃjñā (Technical Term) Registry

**Phase:** 2 — Sūtra Engine & Saṃjñā System  
**Difficulty:** Medium  
**Estimated time:** 2.5 hours  
**Depends on:** Story 2.1

---

## Objective

Build a bitmask registry of all Pāṇinian technical terms (saṃjñās). A Term in the derivation engine carries a `Samjna` bitmask marking every technical label that applies to it. This replaces ad-hoc string tagging with O(1) lookups.

---

## Background

Pāṇini defines ~50 technical terms (saṃjñā) throughout the Aṣṭādhyāyī. Key ones:

| Term | Sūtra | Meaning |
|------|-------|---------|
| dhātu | 1.3.1 | verbal root |
| pratyaya | 3.1.1 | suffix |
| aṅga | 6.4.1 | stem (before suffix) |
| pada | 1.4.14 | word-form |
| subanta | 2.4.82 | nominal word |
| tiṅanta | 2.4.82 | verbal word |
| sarvanāman | 1.1.27 | pronoun |
| kāraka | 1.4.23 | semantic role |
| vibhakti | — | case suffix |
| guna | 1.1.2 | medium-strength vowel |
| vṛddhi | 1.1.1 | high-strength vowel |
| it | 1.3.2 | marker/tag letter |
| kit | — | suffix with k as it |
| ṅit | — | suffix with ṅ as it |
| ṇit | — | suffix with ṇ as it |
| pit | — | suffix with p as it |
| sārvadhatuka | 3.4.113 | certain verb suffixes |
| ārdhadhātuka | 3.4.114 | other verb suffixes |
| tiṅ | 3.4.77 | personal ending |
| sup | 4.1.2 | nominal case suffix |
| kṛt | 3.1.93 | primary nominal suffix |
| taddhita | 4.1.76 | secondary suffix |
| samāsa | 2.1.3 | compound |
| nipāta | 1.4.56 | particle |
| upasarga | 1.4.59 | verbal prefix |
| avyaya | 1.1.37 | indeclinable |
| sat | 3.2.127 | present participle base |
| niṣṭhā | 1.1.26 | kta/ktavat participle class |

---

## Files to Create

### `core/metadata/samjna.h`

```c
#ifndef SAMJNA_H
#define SAMJNA_H

#include <stdint.h>
#include <stdbool.h>

/* 64-bit bitmask — supports up to 64 simultaneous saṃjñās */
typedef uint64_t Samjna;

/* ── Saṃjñā constants ────────────────────────────────────────────────────── */
#define SJ_NONE          UINT64_C(0)
#define SJ_DHATU         (UINT64_C(1) <<  0)  /* 1.3.1 */
#define SJ_PRATYAYA      (UINT64_C(1) <<  1)  /* 3.1.1 */
#define SJ_ANGA          (UINT64_C(1) <<  2)  /* 6.4.1 */
#define SJ_PADA          (UINT64_C(1) <<  3)  /* 1.4.14 */
#define SJ_SUBANTA       (UINT64_C(1) <<  4)  /* 2.4.82 */
#define SJ_TINANTA       (UINT64_C(1) <<  5)  /* 2.4.82 */
#define SJ_SARVANAMAN    (UINT64_C(1) <<  6)  /* 1.1.27 */
#define SJ_KARAKA        (UINT64_C(1) <<  7)  /* 1.4.23 */
#define SJ_VIBHAKTI      (UINT64_C(1) <<  8)
#define SJ_GUNA          (UINT64_C(1) <<  9)  /* 1.1.2 */
#define SJ_VRDDHI        (UINT64_C(1) << 10)  /* 1.1.1 */
#define SJ_IT            (UINT64_C(1) << 11)  /* 1.3.2 */
#define SJ_KIT           (UINT64_C(1) << 12)  /* k-it suffix */
#define SJ_NGIT          (UINT64_C(1) << 13)  /* ṅ-it suffix */
#define SJ_NNIT          (UINT64_C(1) << 14)  /* ṇ-it suffix */
#define SJ_PIT           (UINT64_C(1) << 15)  /* p-it suffix */
#define SJ_SARVADHATUKA  (UINT64_C(1) << 16)  /* 3.4.113 */
#define SJ_ARDHADHATUKA  (UINT64_C(1) << 17)  /* 3.4.114 */
#define SJ_TING          (UINT64_C(1) << 18)  /* 3.4.77 personal ending */
#define SJ_SUP           (UINT64_C(1) << 19)  /* 4.1.2 nominal suffix */
#define SJ_KRT           (UINT64_C(1) << 20)  /* 3.1.93 kṛt suffix */
#define SJ_TADDHITA      (UINT64_C(1) << 21)  /* 4.1.76 taddhita suffix */
#define SJ_SAMASA        (UINT64_C(1) << 22)  /* compound member */
#define SJ_NIPATA        (UINT64_C(1) << 23)  /* 1.4.56 particle */
#define SJ_UPASARGA      (UINT64_C(1) << 24)  /* 1.4.59 prefix */
#define SJ_AVYAYA        (UINT64_C(1) << 25)  /* 1.1.37 indeclinable */
#define SJ_SAT           (UINT64_C(1) << 26)  /* 3.2.127 śatṛ/śānac base */
#define SJ_NISTHA        (UINT64_C(1) << 27)  /* 1.1.26 kta/ktavat */
#define SJ_KRDANTA       (UINT64_C(1) << 28)  /* kṛt + stem */
#define SJ_TADDHITANTA   (UINT64_C(1) << 29)  /* taddhita + stem */
#define SJ_PARASMAIPADA  (UINT64_C(1) << 30)
#define SJ_ATMANEPADA    (UINT64_C(1) << 31)
#define SJ_PRATIPADIKA   (UINT64_C(1) << 32)  /* 1.2.45 prātipadika */
#define SJ_LAGHU         (UINT64_C(1) << 33)  /* light syllable */
#define SJ_GURU          (UINT64_C(1) << 34)  /* heavy syllable */
#define SJ_ANUDATTA      (UINT64_C(1) << 35)
#define SJ_UDATTA        (UINT64_C(1) << 36)
#define SJ_SVARITA       (UINT64_C(1) << 37)

/* ── Operations ──────────────────────────────────────────────────────────── */
#define samjna_has(s, flag)     (((s) & (flag)) != 0)
#define samjna_add(s, flag)     ((s) | (flag))
#define samjna_remove(s, flag)  ((s) & ~(flag))

/** Return human-readable name(s) for a saṃjñā bitmask (static buffer). */
const char *samjna_describe(Samjna s);

/** Get the saṃjñā defined by a specific sūtra (e.g. 1.3.1 → SJ_DHATU). */
Samjna samjna_of_sutra(uint32_t sutra_global_id);

#endif /* SAMJNA_H */
```

### `core/metadata/samjna.c`

Implement `samjna_describe` using a static string buffer (show up to 8 names).

Implement `samjna_of_sutra` with a static lookup table mapping key sūtra global_ids to their saṃjñā constants.

---

## Unit Test: `tests/unit/test_samjna.c`

```c
void test_has(void) {
  Samjna s = SJ_DHATU | SJ_PARASMAIPADA;
  TEST_ASSERT_TRUE(samjna_has(s, SJ_DHATU));
  TEST_ASSERT_TRUE(samjna_has(s, SJ_PARASMAIPADA));
  TEST_ASSERT_FALSE(samjna_has(s, SJ_PRATYAYA));
}

void test_add_remove(void) {
  Samjna s = SJ_NONE;
  s = samjna_add(s, SJ_TING);
  TEST_ASSERT_TRUE(samjna_has(s, SJ_TING));
  s = samjna_remove(s, SJ_TING);
  TEST_ASSERT_FALSE(samjna_has(s, SJ_TING));
}

void test_describe(void) {
  Samjna s = SJ_DHATU | SJ_PARASMAIPADA;
  const char *d = samjna_describe(s);
  TEST_ASSERT_NOT_NULL(d);
  TEST_ASSERT_TRUE(strstr(d, "DHATU") != NULL);
}
```

---

## Acceptance Criteria

- [ ] All 38 saṃjñā constants defined
- [ ] `samjna_has` / `samjna_add` / `samjna_remove` work correctly
- [ ] `samjna_describe` returns non-NULL for any combination
- [ ] Unit test passes
