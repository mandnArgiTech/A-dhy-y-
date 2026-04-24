# Story 1.3 — SLP1 ↔ IAST ↔ Devanāgarī ↔ HK Codec

**Phase:** 1 — Phonology Engine  
**Difficulty:** Medium  
**Estimated time:** 3 hours  
**Depends on:** Story 1.1

---

## Objective

Implement a lossless bidirectional encoding codec. **All internal processing uses SLP1 exclusively.** This module converts at input and output boundaries only.

---

## Background

Four encoding systems are in use in Sanskrit computational work:

| Encoding | Domain | Example (bhavati) |
|---------|--------|-----------------|
| SLP1 | Internal processing (ASCII) | `Bavati` |
| IAST | International scholarly standard (Unicode) | `bhavati` |
| Devanāgarī | Native script (Unicode) | `भवति` |
| Harvard-Kyoto (HK) | Legacy digital texts | `bhavati` |

Note: IAST and HK look similar but differ in capitals and some digraphs.

---

## Files to Create

### `encoding/encoding.h`

```c
#ifndef ENCODING_H
#define ENCODING_H

#include "ashtadhyayi.h"
#include <stddef.h>

/**
 * Convert a string between encodings.
 * @param input   Source string.
 * @param from    Source encoding.
 * @param to      Target encoding.
 * @return Heap-allocated result string. Caller must free(). NULL on error.
 */
char *encoding_convert(const char *input, ASH_Encoding from, ASH_Encoding to);

/* Convenience wrappers */
char *enc_slp1_to_iast(const char *slp1);
char *enc_slp1_to_devanagari(const char *slp1);
char *enc_slp1_to_hk(const char *slp1);
char *enc_iast_to_slp1(const char *iast);
char *enc_hk_to_slp1(const char *hk);
char *enc_devanagari_to_slp1(const char *deva);

/** In-place conversion (writes to caller-provided buffer).
 *  Returns number of bytes written, or -1 if buffer too small. */
int encoding_convert_buf(const char *input, ASH_Encoding from, ASH_Encoding to,
                         char *out, size_t out_len);

#endif /* ENCODING_H */
```

### `encoding/encoding.c`

#### SLP1 → IAST mapping table

Each SLP1 character maps to a UTF-8 IAST string (1–4 bytes):

```c
typedef struct { char slp1; const char *iast; const char *hk; const char *deva_utf8; } EncEntry;

static const EncEntry ENC_TABLE[] = {
  /* Vowels */
  { 'a', "a",    "a",    "\xe0\xa4\x85" },  /* अ */
  { 'A', "\xc4\x81", "A", "\xe0\xa4\x86" }, /* ā → आ */
  { 'i', "i",    "i",    "\xe0\xa4\x87" },  /* इ */
  { 'I', "\xc4\xab", "I", "\xe0\xa4\x88" }, /* ī → ई */
  { 'u', "u",    "u",    "\xe0\xa4\x89" },  /* उ */
  { 'U', "\xc5\xab", "U", "\xe0\xa4\x8a" }, /* ū → ऊ */
  { 'f', "\xe1\xb9\x9b", "R", "\xe0\xa4\x8b" }, /* ṛ → ऋ */
  { 'F', "\xe1\xb9\x9d", "RR", "\xe0\xa4\x8c" }, /* ṝ → ॠ */
  { 'x', "\xe1\xb8\xb7", "lR", "\xe0\xa4\x8c" }, /* ḷ → ऌ */
  { 'e', "e",    "e",    "\xe0\xa4\x8f" },  /* ए */
  { 'E', "ai",   "ai",   "\xe0\xa4\x90" },  /* ऐ */
  { 'o', "o",    "o",    "\xe0\xa4\x93" },  /* ओ */
  { 'O', "au",   "au",   "\xe0\xa4\x94" },  /* औ */
  /* Special */
  { 'M', "\xe1\xb9\x83", "M", "\xe0\xa4\x82" }, /* ṃ → ं */
  { 'H', "\xe1\xb8\xa5", "H", "\xe0\xa4\x83" }, /* ḥ → ः */
  /* Consonants — all 36 stops + 8 others */
  { 'k', "k",    "k",    "\xe0\xa4\x95\xe0\xa5\x8d" }, /* क् */
  { 'K', "kh",   "kh",   "\xe0\xa4\x96\xe0\xa5\x8d" }, /* ख् */
  { 'g', "g",    "g",    "\xe0\xa4\x97\xe0\xa5\x8d" }, /* ग् */
  { 'G', "gh",   "gh",   "\xe0\xa4\x98\xe0\xa5\x8d" }, /* घ् */
  { 'N', "\xe1\xb9\x85", "G", "\xe0\xa4\x99\xe0\xa5\x8d" }, /* ṅ → ङ् */
  { 'c', "c",    "c",    "\xe0\xa4\x9a\xe0\xa5\x8d" }, /* च् */
  { 'C', "ch",   "ch",   "\xe0\xa4\x9b\xe0\xa5\x8d" }, /* छ् */
  { 'j', "j",    "j",    "\xe0\xa4\x9c\xe0\xa5\x8d" }, /* ज् */
  { 'J', "jh",   "jh",   "\xe0\xa4\x9d\xe0\xa5\x8d" }, /* झ् */
  { 'Y', "\xc3\xb1", "J", "\xe0\xa4\x9e\xe0\xa5\x8d" }, /* ñ → ञ् */
  { 'w', "\xe1\xb9\xad", "T", "\xe0\xa4\x9f\xe0\xa5\x8d" }, /* ṭ → ट् */
  { 'W', "\xe1\xb9\xaf", "Th", "\xe0\xa4\xa0\xe0\xa5\x8d" }, /* ṭh → ठ् */
  { 'q', "\xe1\xb8\x8d", "D", "\xe0\xa4\xa1\xe0\xa5\x8d" }, /* ḍ → ड् */
  { 'Q', "\xe1\xb8\x8f", "Dh", "\xe0\xa4\xa2\xe0\xa5\x8d" }, /* ḍh → ढ् */
  { 'R', "\xe1\xb9\x87", "N", "\xe0\xa4\xa3\xe0\xa5\x8d" }, /* ṇ → ण् */
  { 't', "t",    "t",    "\xe0\xa4\xa4\xe0\xa5\x8d" }, /* त् */
  { 'T', "th",   "th",   "\xe0\xa4\xa5\xe0\xa5\x8d" }, /* थ् */
  { 'd', "d",    "d",    "\xe0\xa4\xa6\xe0\xa5\x8d" }, /* द् */
  { 'D', "dh",   "dh",   "\xe0\xa4\xa7\xe0\xa5\x8d" }, /* ध् */
  { 'n', "n",    "n",    "\xe0\xa4\xa8\xe0\xa5\x8d" }, /* न् */
  { 'p', "p",    "p",    "\xe0\xa4\xaa\xe0\xa5\x8d" }, /* प् */
  { 'P', "ph",   "ph",   "\xe0\xa4\xab\xe0\xa5\x8d" }, /* फ् */
  { 'b', "b",    "b",    "\xe0\xa4\xac\xe0\xa5\x8d" }, /* ब् */
  { 'B', "bh",   "bh",   "\xe0\xa4\xad\xe0\xa5\x8d" }, /* भ् */
  { 'm', "m",    "m",    "\xe0\xa4\xae\xe0\xa5\x8d" }, /* म् */
  { 'y', "y",    "y",    "\xe0\xa4\xaf\xe0\xa5\x8d" }, /* य् */
  { 'r', "r",    "r",    "\xe0\xa4\xb0\xe0\xa5\x8d" }, /* र् */
  { 'l', "l",    "l",    "\xe0\xa4\xb2\xe0\xa5\x8d" }, /* ल् */
  { 'v', "v",    "v",    "\xe0\xa4\xb5\xe0\xa5\x8d" }, /* व् */
  { 'S', "\xc5\x9b",   "z", "\xe0\xa4\xb6\xe0\xa5\x8d" }, /* ś → श् */
  { 'z', "\xe1\xb9\xa3", "S", "\xe0\xa4\xb7\xe0\xa5\x8d" }, /* ṣ → ष् */
  { 's', "s",    "s",    "\xe0\xa4\xb8\xe0\xa5\x8d" }, /* स् */
  { 'h', "h",    "h",    "\xe0\xa4\xb9\xe0\xa5\x8d" }, /* ह् */
  { 0, NULL, NULL, NULL }  /* sentinel */
};
```

#### Devanāgarī vowel mark table

When a vowel follows a consonant in Devanāgarī, use vowel signs (mātrā) instead of full vowel letters:

```c
typedef struct { char slp1; const char *matra_utf8; } MatraEntry;
static const MatraEntry MATRA_TABLE[] = {
  { 'a', "" },                          /* inherent a — no mark */
  { 'A', "\xe0\xa4\xbe" },             /* ा */
  { 'i', "\xe0\xa4\xbf" },             /* ि */
  { 'I', "\xe0\xa5\x80" },             /* ी */
  { 'u', "\xe0\xa5\x81" },             /* ु */
  { 'U', "\xe0\xa5\x82" },             /* ू */
  { 'f', "\xe0\xa5\x83" },             /* ृ */
  { 'F', "\xe0\xa5\x84" },             /* ॄ */
  { 'e', "\xe0\xa5\x87" },             /* े */
  { 'E', "\xe0\xa5\x88" },             /* ै */
  { 'o', "\xe0\xa5\x8b" },             /* ो */
  { 'O', "\xe0\xa5\x8c" },             /* ौ */
  { 0, NULL }
};
```

#### Implementation notes

- SLP1 → IAST: scan SLP1 char by char, look up `iast` field. Output UTF-8.
- SLP1 → Devanāgarī: more complex — must track "previous was consonant" state to decide full-vowel vs mātrā.
- IAST → SLP1: scan UTF-8 IAST, match against `iast` fields (longest match first for "ai", "au", "kh", etc.)
- All returned strings are heap-allocated UTF-8. Caller frees.

---

## Unit Test: `tests/unit/test_encoding.c`

```c
void test_slp1_to_iast_simple(void) {
  char *r = enc_slp1_to_iast("BavAmi");  /* bhavāmi */
  TEST_ASSERT_NOT_NULL(r);
  TEST_ASSERT_EQUAL_STRING("bhav\xc4\x81mi", r);  /* bhavāmi */
  free(r);
}

void test_slp1_to_iast_roundtrip(void) {
  const char *words[] = {"rAma", "kfzRa", "BArata", "pfTivI", NULL};
  for (int i = 0; words[i]; i++) {
    char *iast = enc_slp1_to_iast(words[i]);
    char *back = enc_iast_to_slp1(iast);
    TEST_ASSERT_EQUAL_STRING(words[i], back);
    free(iast); free(back);
  }
}

void test_slp1_to_devanagari(void) {
  char *d = enc_slp1_to_devanagari("rAma");
  TEST_ASSERT_NOT_NULL(d);
  /* "रामa" — verify it starts with र */
  TEST_ASSERT_EQUAL_UINT8(0xe0, (uint8_t)d[0]);
  free(d);
}

/* 500-word round-trip test loaded from tests/data/encoding_roundtrip.tsv */
void test_roundtrip_corpus(void) { ... }
```

Provide `tests/data/encoding_roundtrip.tsv` with 100 SLP1↔IAST pairs drawn from common Sanskrit words.

---

## Acceptance Criteria

- [ ] All 52 phonemes convert correctly SLP1 → IAST
- [ ] `enc_iast_to_slp1(enc_slp1_to_iast(s)) == s` for all test words
- [ ] Devanāgarī output is valid UTF-8
- [ ] 100-word round-trip test passes
- [ ] No memory leaks (valgrind clean)
