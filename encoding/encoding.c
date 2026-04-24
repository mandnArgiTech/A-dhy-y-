/* encoding.c — Story 1.3 implementation
 *
 * SLP1 ↔ IAST ↔ Devanāgarī ↔ Harvard-Kyoto codec.
 *
 * Primary encoding for all internal processing: SLP1 (ASCII-only, 1 byte per phoneme).
 * Conversion happens only at input and output boundaries.
 */

#include "encoding.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

const char *ash_version(void) { return "0.1.0"; }

/* ── Phoneme table: SLP1 char ↔ IAST bytes ↔ Devanāgarī standalone ↔ HK ───
 *
 * Each Devanāgarī has TWO forms: standalone (अ) and mātrā (vowel mark after consonant).
 * For consonants, the encoded form is the consonant-letter-with-virama pair (e.g. क्).
 * The main conversion loop removes virāmas when a vowel (→ mātrā) follows.
 */

typedef struct {
  char slp1;            /* single ASCII char */
  const char *iast;     /* UTF-8 IAST */
  const char *hk;       /* Harvard-Kyoto (ASCII) */
  const char *deva_std; /* Devanāgarī standalone letter (UTF-8) */
  const char *deva_matra; /* Devanāgarī vowel mark (UTF-8), NULL for consonants/special */
} EncEntry;

static const EncEntry ENC[] = {
  /* Vowels — standalone form + mātrā */
  { 'a', "a",        "a",   "अ",  ""       },   /* inherent — no matra */
  { 'A', "\xc4\x81", "A",   "आ",  "\xe0\xa4\xbe" },
  { 'i', "i",        "i",   "इ",  "\xe0\xa4\xbf" },
  { 'I', "\xc4\xab", "I",   "ई",  "\xe0\xa5\x80" },
  { 'u', "u",        "u",   "उ",  "\xe0\xa5\x81" },
  { 'U', "\xc5\xab", "U",   "ऊ",  "\xe0\xa5\x82" },
  { 'f', "\xe1\xb9\x9b", "R",  "ऋ",  "\xe0\xa5\x83" },   /* ṛ */
  { 'F', "\xe1\xb9\x9d", "RR", "ॠ",  "\xe0\xa5\x84" },   /* ṝ */
  { 'x', "\xe1\xb8\xb7", "lR", "ऌ",  "\xe0\xa5\x8c" /* approximation */ },
  { 'e', "e",        "e",   "ए",  "\xe0\xa5\x87" },
  { 'E', "ai",       "ai",  "ऐ",  "\xe0\xa5\x88" },
  { 'o', "o",        "o",   "ओ",  "\xe0\xa5\x8b" },
  { 'O', "au",       "au",  "औ",  "\xe0\xa5\x8c" },

  /* Special */
  { 'M', "\xe1\xb9\x83", "M", "\xe0\xa4\x82", NULL },    /* ṃ → anusvāra sign */
  { 'H', "\xe1\xb8\xa5", "H", "\xe0\xa4\x83", NULL },    /* ḥ → visarga sign */

  /* Consonants — standalone = letter + virāma (गेमा form).
     Tests here check IAST/SLP1 roundtrip; Devanāgarī output inserts virāma
     unless a vowel mātrā follows (handled in the conversion loop). */
  { 'k', "k",  "k",  "\xe0\xa4\x95", NULL },
  { 'K', "kh", "kh", "\xe0\xa4\x96", NULL },
  { 'g', "g",  "g",  "\xe0\xa4\x97", NULL },
  { 'G', "gh", "gh", "\xe0\xa4\x98", NULL },
  { 'N', "\xe1\xb9\x85", "G",  "\xe0\xa4\x99", NULL },  /* ṅ */
  { 'c', "c",  "c",  "\xe0\xa4\x9a", NULL },
  { 'C', "ch", "ch", "\xe0\xa4\x9b", NULL },
  { 'j', "j",  "j",  "\xe0\xa4\x9c", NULL },
  { 'J', "jh", "jh", "\xe0\xa4\x9d", NULL },
  { 'Y', "\xc3\xb1", "J", "\xe0\xa4\x9e", NULL },       /* ñ */
  { 'w', "\xe1\xb9\xad", "T",  "\xe0\xa4\x9f", NULL },  /* ṭ */
  { 'W', "\xe1\xb9\xad\x68", "Th", "\xe0\xa4\xa0", NULL },/* ṭh */
  { 'q', "\xe1\xb8\x8d", "D",  "\xe0\xa4\xa1", NULL },  /* ḍ */
  { 'Q', "\xe1\xb8\x8d\x68", "Dh", "\xe0\xa4\xa2", NULL },/* ḍh */
  { 'R', "\xe1\xb9\x87", "N",  "\xe0\xa4\xa3", NULL },  /* ṇ */
  { 't', "t",  "t",  "\xe0\xa4\xa4", NULL },
  { 'T', "th", "th", "\xe0\xa4\xa5", NULL },
  { 'd', "d",  "d",  "\xe0\xa4\xa6", NULL },
  { 'D', "dh", "dh", "\xe0\xa4\xa7", NULL },
  { 'n', "n",  "n",  "\xe0\xa4\xa8", NULL },
  { 'p', "p",  "p",  "\xe0\xa4\xaa", NULL },
  { 'P', "ph", "ph", "\xe0\xa4\xab", NULL },
  { 'b', "b",  "b",  "\xe0\xa4\xac", NULL },
  { 'B', "bh", "bh", "\xe0\xa4\xad", NULL },
  { 'm', "m",  "m",  "\xe0\xa4\xae", NULL },
  { 'y', "y",  "y",  "\xe0\xa4\xaf", NULL },
  { 'r', "r",  "r",  "\xe0\xa4\xb0", NULL },
  { 'l', "l",  "l",  "\xe0\xa4\xb2", NULL },
  { 'v', "v",  "v",  "\xe0\xa4\xb5", NULL },
  { 'S', "\xc5\x9b", "z", "\xe0\xa4\xb6", NULL },       /* ś */
  { 'z', "\xe1\xb9\xa3", "S", "\xe0\xa4\xb7", NULL },   /* ṣ */
  { 's', "s",  "s",  "\xe0\xa4\xb8", NULL },
  { 'h', "h",  "h",  "\xe0\xa4\xb9", NULL },

  { 0, NULL, NULL, NULL, NULL }  /* sentinel */
};

static const char VIRAMA_UTF8[]   = "\xe0\xa5\x8d";   /* ् */
static const size_t VIRAMA_LEN    = 3;

/* ── Helpers ─────────────────────────────────────────────────────────────── */

static const EncEntry *find_by_slp1(char c) {
  for (int i = 0; ENC[i].slp1; i++)
    if (ENC[i].slp1 == c) return &ENC[i];
  return NULL;
}

static bool is_vowel_slp1(char c) {
  return c == 'a' || c == 'A' || c == 'i' || c == 'I' ||
         c == 'u' || c == 'U' || c == 'f' || c == 'F' ||
         c == 'x' || c == 'X' || c == 'e' || c == 'E' ||
         c == 'o' || c == 'O';
}

static bool is_consonant_slp1(char c) {
  const EncEntry *e = find_by_slp1(c);
  return e != NULL && !is_vowel_slp1(c) && c != 'M' && c != 'H';
}

/* ── SLP1 → IAST ─────────────────────────────────────────────────────────── */

char *enc_slp1_to_iast(const char *slp1) {
  if (!slp1) return NULL;
  size_t out_cap = strlen(slp1) * 4 + 1;  /* IAST max = 3 bytes per char */
  char *out = malloc(out_cap);
  if (!out) return NULL;
  size_t pos = 0;
  for (const char *p = slp1; *p && pos + 4 < out_cap; p++) {
    const EncEntry *e = find_by_slp1(*p);
    if (e) {
      size_t n = strlen(e->iast);
      memcpy(out + pos, e->iast, n);
      pos += n;
    } else {
      out[pos++] = *p;  /* pass-through unknown (e.g. space, punctuation) */
    }
  }
  out[pos] = '\0';
  return out;
}

/* ── SLP1 → Devanāgarī ───────────────────────────────────────────────────
 *
 * Rules:
 *  1. Vowel at word-start or after another vowel/non-consonant → standalone letter
 *  2. Vowel after consonant → mātrā (and we replaced the preceding consonant's
 *     implicit virāma — so we track whether the last output was a consonant)
 *  3. Consonant not followed by vowel → letter + virāma
 *  4. Consonant followed by consonant → letter + virāma (conjunct)
 *  5. Consonant followed by vowel → letter + mātrā (no virāma)
 *  6. Anusvāra/visarga attach directly (no virāma logic)
 */
char *enc_slp1_to_devanagari(const char *slp1) {
  if (!slp1) return NULL;
  size_t out_cap = strlen(slp1) * 10 + 16;  /* Devanāgarī can be 3-6 bytes per phoneme */
  char *out = malloc(out_cap);
  if (!out) return NULL;
  size_t pos = 0;
  bool prev_was_consonant = false;

  for (const char *p = slp1; *p && pos + 16 < out_cap; p++) {
    char c = *p;
    const EncEntry *e = find_by_slp1(c);

    if (!e) {
      /* Pass through non-SLP1 chars */
      if (prev_was_consonant) {
        memcpy(out + pos, VIRAMA_UTF8, VIRAMA_LEN);
        pos += VIRAMA_LEN;
        prev_was_consonant = false;
      }
      out[pos++] = c;
      continue;
    }

    if (is_vowel_slp1(c)) {
      if (prev_was_consonant && e->deva_matra) {
        /* Vowel mātrā after consonant — do NOT emit virāma */
        size_t n = strlen(e->deva_matra);
        memcpy(out + pos, e->deva_matra, n);
        pos += n;
      } else {
        /* Standalone vowel */
        size_t n = strlen(e->deva_std);
        memcpy(out + pos, e->deva_std, n);
        pos += n;
      }
      prev_was_consonant = false;
    } else if (is_consonant_slp1(c)) {
      /* If previous was a consonant, emit virāma for conjunct */
      if (prev_was_consonant) {
        memcpy(out + pos, VIRAMA_UTF8, VIRAMA_LEN);
        pos += VIRAMA_LEN;
      }
      size_t n = strlen(e->deva_std);
      memcpy(out + pos, e->deva_std, n);
      pos += n;
      prev_was_consonant = true;
    } else {
      /* Anusvāra or visarga */
      if (prev_was_consonant) {
        memcpy(out + pos, VIRAMA_UTF8, VIRAMA_LEN);
        pos += VIRAMA_LEN;
        prev_was_consonant = false;
      }
      size_t n = strlen(e->deva_std);
      memcpy(out + pos, e->deva_std, n);
      pos += n;
    }
  }
  /* Trailing consonant — emit virāma */
  if (prev_was_consonant && pos + VIRAMA_LEN + 1 < out_cap) {
    memcpy(out + pos, VIRAMA_UTF8, VIRAMA_LEN);
    pos += VIRAMA_LEN;
  }
  out[pos] = '\0';
  return out;
}

/* ── SLP1 → Harvard-Kyoto ───────────────────────────────────────────────── */

char *enc_slp1_to_hk(const char *slp1) {
  if (!slp1) return NULL;
  size_t out_cap = strlen(slp1) * 3 + 1;
  char *out = malloc(out_cap);
  if (!out) return NULL;
  size_t pos = 0;
  for (const char *p = slp1; *p && pos + 4 < out_cap; p++) {
    const EncEntry *e = find_by_slp1(*p);
    if (e) {
      size_t n = strlen(e->hk);
      memcpy(out + pos, e->hk, n);
      pos += n;
    } else {
      out[pos++] = *p;
    }
  }
  out[pos] = '\0';
  return out;
}

/* ── IAST → SLP1 (reverse; longest-match first) ────────────────────────── */

/* Sorted longest-first for correct disambiguation (ai before a, kh before k) */
static int iast_to_slp1_match(const char *iast, char *out_slp1) {
  /* Try 3-byte matches first (ā, ī, ū, ṛ, ṝ, ṃ, ḥ, etc.) */
  for (int i = 0; ENC[i].slp1; i++) {
    size_t n = strlen(ENC[i].iast);
    if (n == 0) continue;
    if (strncmp(iast, ENC[i].iast, n) == 0) {
      *out_slp1 = ENC[i].slp1;
      return (int)n;
    }
  }
  return 0;
}

char *enc_iast_to_slp1(const char *iast) {
  if (!iast) return NULL;
  size_t out_cap = strlen(iast) + 1;
  char *out = malloc(out_cap);
  if (!out) return NULL;
  size_t pos = 0;

  /* Pre-sort: try digraphs first (kh, gh, ch, jh, th, dh, ph, bh, ai, au)
   * because simple single-char lookup would match 'k' before 'kh'. */
  static const struct { const char *iast; char slp1; } DIGRAPHS[] = {
    {"kh", 'K'}, {"gh", 'G'}, {"ch", 'C'}, {"jh", 'J'},
    {"\xe1\xb9\xad\x68", 'W'},  /* ṭh */
    {"\xe1\xb8\x8d\x68", 'Q'},  /* ḍh */
    {"th", 'T'}, {"dh", 'D'}, {"ph", 'P'}, {"bh", 'B'},
    {"ai", 'E'}, {"au", 'O'},
    {NULL, 0}
  };

  const char *p = iast;
  while (*p) {
    bool matched = false;
    for (int i = 0; DIGRAPHS[i].iast; i++) {
      size_t n = strlen(DIGRAPHS[i].iast);
      if (strncmp(p, DIGRAPHS[i].iast, n) == 0) {
        if (pos + 1 < out_cap) out[pos++] = DIGRAPHS[i].slp1;
        p += n;
        matched = true;
        break;
      }
    }
    if (matched) continue;

    char slp1_char = 0;
    int consumed = iast_to_slp1_match(p, &slp1_char);
    if (consumed > 0) {
      if (pos + 1 < out_cap) out[pos++] = slp1_char;
      p += consumed;
    } else {
      /* Pass through ASCII that we don't recognize */
      if (pos + 1 < out_cap) out[pos++] = *p;
      p++;
    }
  }
  out[pos] = '\0';
  return out;
}

/* ── HK → SLP1 ───────────────────────────────────────────────────────────── */

char *enc_hk_to_slp1(const char *hk) {
  if (!hk) return NULL;
  size_t out_cap = strlen(hk) + 1;
  char *out = malloc(out_cap);
  if (!out) return NULL;
  size_t pos = 0;

  /* HK uses single-char uppercase for retroflex/nasal etc.
     Digraphs still present (kh, gh, etc.) */
  const char *p = hk;
  while (*p) {
    bool matched = false;
    /* Try 2-char digraphs */
    if (p[1]) {
      for (int i = 0; ENC[i].slp1; i++) {
        if (strlen(ENC[i].hk) == 2 && strncmp(p, ENC[i].hk, 2) == 0) {
          if (pos + 1 < out_cap) out[pos++] = ENC[i].slp1;
          p += 2;
          matched = true;
          break;
        }
      }
    }
    if (matched) continue;

    /* Single char */
    for (int i = 0; ENC[i].slp1; i++) {
      if (strlen(ENC[i].hk) == 1 && ENC[i].hk[0] == *p) {
        if (pos + 1 < out_cap) out[pos++] = ENC[i].slp1;
        p++;
        matched = true;
        break;
      }
    }
    if (!matched) {
      if (pos + 1 < out_cap) out[pos++] = *p;
      p++;
    }
  }
  out[pos] = '\0';
  return out;
}

/* Devanāgarī → SLP1 is complex; stubbed for now */
char *enc_devanagari_to_slp1(const char *deva) {
  if (!deva) return NULL;
  return strdup(deva);  /* TODO: full implementation */
}

/* ── Generic dispatcher ──────────────────────────────────────────────────── */

char *ash_encode(const char *input, ASH_Encoding from, ASH_Encoding to) {
  if (!input) return NULL;
  if (from == to) return strdup(input);

  /* Route through SLP1 as intermediate */
  char *slp1 = NULL;
  bool alloc_slp1 = false;
  switch (from) {
    case ASH_ENC_SLP1:       slp1 = (char *)input; break;
    case ASH_ENC_IAST:       slp1 = enc_iast_to_slp1(input); alloc_slp1 = true; break;
    case ASH_ENC_HK:         slp1 = enc_hk_to_slp1(input); alloc_slp1 = true; break;
    case ASH_ENC_DEVANAGARI: slp1 = enc_devanagari_to_slp1(input); alloc_slp1 = true; break;
  }
  if (!slp1) return NULL;

  char *result = NULL;
  switch (to) {
    case ASH_ENC_SLP1:       result = strdup(slp1); break;
    case ASH_ENC_IAST:       result = enc_slp1_to_iast(slp1); break;
    case ASH_ENC_HK:         result = enc_slp1_to_hk(slp1); break;
    case ASH_ENC_DEVANAGARI: result = enc_slp1_to_devanagari(slp1); break;
  }
  if (alloc_slp1) free(slp1);
  return result;
}

char *encoding_convert(const char *input, ASH_Encoding from, ASH_Encoding to) {
  return ash_encode(input, from, to);
}

int encoding_convert_buf(const char *in, ASH_Encoding from, ASH_Encoding to,
                          char *out, size_t out_len) {
  char *r = ash_encode(in, from, to);
  if (!r || !out || out_len == 0) { free(r); return -1; }
  size_t n = strlen(r);
  if (n + 1 > out_len) { free(r); return -1; }
  memcpy(out, r, n + 1);
  free(r);
  return (int)n;
}
