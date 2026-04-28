/* sandhi_vowel.c — Story 1.4 implementation
 *
 * Vowel sandhi rules (ac-sandhi) from Aṣṭādhyāyī 6.1.66–6.1.111.
 *
 * Priority (most specific first):
 *   1. 6.1.101  akaḥ savarṇe dīrghaḥ   (savarṇadīrgha: like + like → long)
 *   2. 6.1.87   ādguṇaḥ                 (a/ā + ik vowel → guṇa)
 *   3. 6.1.88   vṛddhireci               (a/ā + eC vowel → vṛddhi)
 *   4. 6.1.77   iko yaṇ aci              (ik + vowel → corresponding semivowel)
 *   5. 6.1.78   eco 'yavāyāvaḥ           (eC + vowel → ay/āy/av/āv)
 */

#include "sandhi_vowel.h"
#include "varna.h"
#include <string.h>

/* Traditional addresses of the sandhi sūtras (for rule_applied field).
 * Stored in adhyAya/pAda/sUtra packed format: a*100000 + p*1000 + n. */
#define SUTRA_6_1_66  610066
#define SUTRA_6_1_77  610077
#define SUTRA_6_1_78  610078
#define SUTRA_6_1_87  610087
#define SUTRA_6_1_88  610088
#define SUTRA_6_1_101 610101
#define SUTRA_6_1_109 610109

/* ── Helpers ─────────────────────────────────────────────────────────────── */

/* Is this a vowel in the ik class (i/ī/u/ū/ṛ/ṝ/ḷ)? */
static bool is_ik(char c) {
  return c == 'i' || c == 'I' || c == 'u' || c == 'U' ||
         c == 'f' || c == 'F' || c == 'x';
}

/* Is this a vowel in the eC class (e/ai/o/au)? */
static bool is_ec(char c) {
  return c == 'e' || c == 'E' || c == 'o' || c == 'O';
}

/* Map ik vowel → corresponding semivowel (yaṆ): i→y, u→v, ṛ→r, ḷ→l */
static char yan_of(char ik_vowel) {
  switch (ik_vowel) {
    case 'i': case 'I': return 'y';
    case 'u': case 'U': return 'v';
    case 'f': case 'F': return 'r';
    case 'x':           return 'l';
    default:            return 0;
  }
}

/* Map eC vowel → ay/āy/av/āv sequence (written to out, returns length) */
static int ayavadi_of(char ec_vowel, char *out) {
  switch (ec_vowel) {
    case 'e': out[0] = 'a'; out[1] = 'y'; return 2;
    case 'E': out[0] = 'A'; out[1] = 'y'; return 2;
    case 'o': out[0] = 'a'; out[1] = 'v'; return 2;
    case 'O': out[0] = 'A'; out[1] = 'v'; return 2;
    default:  out[0] = ec_vowel; return 1;
  }
}

/* ── Core: junction sandhi ─────────────────────────────────────────────── */

SandhiResult sandhi_vowel_apply(char a_final, char b_initial) {
  SandhiResult r;
  memset(&r, 0, sizeof(r));
  r.changed = false;
  r.rule_applied = 0;

  /* Only apply if both are vowels */
  if (!varna_is_vowel(a_final) || !varna_is_vowel(b_initial)) {
    /* Just concatenate */
    r.result_slp1[0] = a_final;
    r.result_slp1[1] = b_initial;
    r.result_slp1[2] = '\0';
    return r;
  }

  /* Rule 6.1.101: savarṇadīrgha — like + like → long
   * If a and b are savarṇa (same phoneme class), output the long form. */
  if (varna_is_savarna(a_final, b_initial)) {
    char long_form = varna_lengthen(varna_savarna_rep(a_final));
    r.result_slp1[0] = long_form;
    r.result_slp1[1] = '\0';
    r.changed = true;
    r.rule_applied = SUTRA_6_1_101;
    return r;
  }

  /* Rules 6.1.87 / 6.1.88: a/ā + {ik | eC} */
  if (a_final == 'a' || a_final == 'A') {
    if (is_ik(b_initial)) {
      /* 6.1.87 ādguṇaḥ — guṇa */
      r.result_slp1[0] = varna_guna(b_initial);
      /* ṛ/ḷ produce 2-char guṇa (ar, al) — append 'r'/'l' */
      if (b_initial == 'f' || b_initial == 'F') {
        r.result_slp1[1] = 'r';
        r.result_slp1[2] = '\0';
      } else if (b_initial == 'x') {
        r.result_slp1[1] = 'l';
        r.result_slp1[2] = '\0';
      } else {
        r.result_slp1[1] = '\0';
      }
      r.changed = true;
      r.rule_applied = SUTRA_6_1_87;
      return r;
    }
    if (is_ec(b_initial)) {
      /* 6.1.88 vṛddhireci — vṛddhi */
      r.result_slp1[0] = varna_vrddhi(b_initial);
      r.result_slp1[1] = '\0';
      r.changed = true;
      r.rule_applied = SUTRA_6_1_88;
      return r;
    }
  }

  /* Rule 6.1.77: ik + vowel → yaṆ (semivowel) + vowel
   * Applies when a_final is ik vowel and b_initial is any vowel. */
  if (is_ik(a_final)) {
    char yan = yan_of(a_final);
    r.result_slp1[0] = yan;
    r.result_slp1[1] = b_initial;
    r.result_slp1[2] = '\0';
    r.changed = true;
    r.rule_applied = SUTRA_6_1_77;
    return r;
  }

  /* Rule 6.1.78: eC + vowel → ay/āy/av/āv + vowel */
  if (is_ec(a_final)) {
    char tmp[4];
    int n = ayavadi_of(a_final, tmp);
    memcpy(r.result_slp1, tmp, n);
    r.result_slp1[n] = b_initial;
    r.result_slp1[n + 1] = '\0';
    r.changed = true;
    r.rule_applied = SUTRA_6_1_78;
    return r;
  }

  /* Fall-through: no rule applied, concatenate */
  r.result_slp1[0] = a_final;
  r.result_slp1[1] = b_initial;
  r.result_slp1[2] = '\0';
  return r;
}

/* ── Full-word sandhi join ────────────────────────────────────────────── */

bool sandhi_vowel_join(const char *a, const char *b, char *out, size_t len) {
  if (!a || !b || !out || len < 2) return false;
  size_t la = strlen(a);
  size_t lb = strlen(b);
  if (la == 0 || lb == 0) {
    /* Empty input — just copy whichever is non-empty */
    size_t n = la + lb;
    if (n + 1 > len) return false;
    memcpy(out, a, la); memcpy(out + la, b, lb); out[n] = '\0';
    return false;
  }

  char a_final = a[la - 1];
  char b_initial = b[0];
  SandhiResult sr = sandhi_vowel_apply(a_final, b_initial);

  /* Compose output: a[0..la-2] + sr.result_slp1 + b[1..lb-1] */
  size_t prefix_len = la - 1;
  size_t junction_len = strlen(sr.result_slp1);
  size_t suffix_len = lb - 1;
  size_t total = prefix_len + junction_len + suffix_len;

  if (total + 1 > len) return false;

  memcpy(out, a, prefix_len);
  memcpy(out + prefix_len, sr.result_slp1, junction_len);
  memcpy(out + prefix_len + junction_len, b + 1, suffix_len);
  out[total] = '\0';

  return sr.changed;
}

int sandhi_vowel_split(char junction, SandhiSplit *splits, int max_splits) {
  int count = 0;
  if (!splits || max_splits <= 0) return 0;
#define ADD_SPLIT(A, B, R)                          \
  do {                                              \
    if (count < max_splits) {                       \
      splits[count].a_end = (A);                    \
      splits[count].b_start = (B);                  \
      splits[count].rule = (R);                     \
      count++;                                      \
    }                                               \
  } while (0)

  switch (junction) {
    case 'A':
      ADD_SPLIT('a', 'a', SUTRA_6_1_101);
      ADD_SPLIT('A', 'a', SUTRA_6_1_101);
      ADD_SPLIT('a', 'A', SUTRA_6_1_101);
      break;
    case 'I':
      ADD_SPLIT('i', 'i', SUTRA_6_1_101);
      ADD_SPLIT('I', 'i', SUTRA_6_1_101);
      ADD_SPLIT('i', 'I', SUTRA_6_1_101);
      break;
    case 'U':
      ADD_SPLIT('u', 'u', SUTRA_6_1_101);
      ADD_SPLIT('U', 'u', SUTRA_6_1_101);
      ADD_SPLIT('u', 'U', SUTRA_6_1_101);
      break;
    case 'e':
      ADD_SPLIT('a', 'i', SUTRA_6_1_87);
      ADD_SPLIT('a', 'I', SUTRA_6_1_87);
      break;
    case 'o':
      ADD_SPLIT('a', 'u', SUTRA_6_1_87);
      ADD_SPLIT('a', 'U', SUTRA_6_1_87);
      break;
    case 'E':
      ADD_SPLIT('a', 'e', SUTRA_6_1_88);
      ADD_SPLIT('a', 'E', SUTRA_6_1_88);
      break;
    case 'O':
      ADD_SPLIT('a', 'o', SUTRA_6_1_88);
      ADD_SPLIT('a', 'O', SUTRA_6_1_88);
      break;
    default:
      break;
  }
#undef ADD_SPLIT
  return count;
}
