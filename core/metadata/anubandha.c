/* anubandha.c — Story 2.3 implementation
 *
 * Anubandha (it-letter) identification and stripping per sūtras 1.3.2–1.3.9.
 *
 * Every dhātu in the Dhātupāṭha and every suffix in the Aṣṭādhyāyī is given
 * in "upadesa" form with marker letters (anubandhas) that encode grammatical
 * information compactly. These markers are stripped before phonological
 * processing but their implications (e.g. kit, ṅit, ṇit, pit) are preserved
 * in the Term's Samjna bitmask.
 */

#include "anubandha.h"
#include "varna.h"
#include <string.h>
#include <ctype.h>

/* Return length of upadesa in bytes (SLP1 is ASCII so bytes == chars) */
static inline int slen(const char *s) { return s ? (int)strlen(s) : 0; }

/* Check if a char is an "it-able" consonant per Pāṇini's rules:
 * Any final consonant in upadesa is a candidate for it-status (1.3.3). */
static bool is_candidate_final_it(char c) {
  /* 1.3.3 hal antyam — any final consonant is it */
  /* Exception 1.3.4: in vibhakti contexts, t/th/d/dh/n/s/h are NOT it */
  return varna_is_consonant(c) || c == 'h';
}

/* 1.3.6: ṣ is it when first letter of a pratyaya
 * 1.3.7: c/ñ are it when first letter of a pratyaya
 * 1.3.8: l is it when first letter of a pratyaya */
static bool is_candidate_initial_it(char c, Samjna ctx) {
  if (!samjna_has(ctx, SJ_PRATYAYA)) return false;
  return c == 'z' ||    /* ṣ (1.3.6 ṣo'pratyayasya) */
         c == 'c' ||    /* (1.3.7 cuṭū) */
         c == 'w' ||    /* ṭ — same sūtra 1.3.7 */
         c == 'Y' ||    /* ñ — 1.3.7 */
         c == 'l' ||    /* l (1.3.8) */
         c == 'S';      /* ś (1.3.8 laśakvataddhite) */
}

/* Main entry: strip anubandhas and record it-letters + derived saṃjñās */
void anubandha_strip(const char *upadesa_slp1, Samjna context,
                     AnubandhaResult *result) {
  if (!result) return;
  memset(result, 0, sizeof(*result));
  if (!upadesa_slp1) { result->clean_slp1[0] = '\0'; return; }

  int n = slen(upadesa_slp1);
  if (n == 0) { result->clean_slp1[0] = '\0'; return; }

  /* Working buffer — copy upadesa, then remove it-letters */
  char work[128];
  if (n >= (int)sizeof(work)) n = (int)sizeof(work) - 1;
  memcpy(work, upadesa_slp1, n);
  work[n] = '\0';

  /* Track which positions are it-letters (true = strip) */
  bool is_it[128] = {false};

  /* ── 1.3.2: nasalized vowels (~ marker) are it ──────────────────────── */
  for (int i = 0; i < n; i++) {
    if (work[i] == '~') {
      is_it[i] = true;
      /* The nasalization marker itself is deleted; the preceding vowel stays */
    }
  }

  /* ── 1.3.3: final consonant is it (general rule) ──────────────────── */
  /* Find the last non-'~' char */
  int last = n - 1;
  while (last > 0 && work[last] == '~') last--;

  if (last >= 0 && is_candidate_final_it(work[last])) {
    /* Exception 1.3.4: in vibhakti/sup suffix contexts, certain finals are NOT it */
    bool exception = false;
    if (samjna_has(context, SJ_VIBHAKTI) || samjna_has(context, SJ_SUP)) {
      char c = work[last];
      if (c == 't' || c == 'T' || c == 'd' || c == 'D' ||
          c == 'n' || c == 's' || c == 'h') {
        exception = true;
      }
    }
    if (!exception) {
      is_it[last] = true;

      /* Record derived saṃjñā */
      result->it_samjna = samjna_add(result->it_samjna, it_to_samjna(work[last]));
    }
  }

  /* ── 1.3.5-8: initial it-letters in pratyaya context ───────────────── */
  /* 1.3.5 ādir ñi-ṭu-ḍavaḥ — certain initial pairs are it */
  /* Patterns: ñi-, ṭu-, ḍu- at start of dhātu/pratyaya are it */
  if (n >= 2 && samjna_has(context, SJ_PRATYAYA | SJ_DHATU)) {
    /* ñi = "Yi", ṭu = "wu", ḍu = "qu" */
    if ((work[0] == 'Y' && work[1] == 'i') ||
        (work[0] == 'w' && work[1] == 'u') ||
        (work[0] == 'q' && work[1] == 'u')) {
      is_it[0] = true;
      is_it[1] = true;
    }
  }

  /* 1.3.6/7/8: initial single-char it in pratyaya context */
  if (n >= 1 && is_candidate_initial_it(work[0], context) && !is_it[0]) {
    /* Don't strip if it's the only char */
    if (n > 1) {
      is_it[0] = true;
      result->it_samjna = samjna_add(result->it_samjna, it_to_samjna(work[0]));
    }
  }

  /* ── Build clean form + it-letter list ─────────────────────────────── */
  int clean_pos = 0;
  result->it_count = 0;
  for (int i = 0; i < n; i++) {
    if (is_it[i]) {
      if (result->it_count < 15) {
        result->it_chars[result->it_count++] = work[i];
      }
    } else {
      if (clean_pos < (int)sizeof(result->clean_slp1) - 1) {
        result->clean_slp1[clean_pos++] = work[i];
      }
    }
  }
  result->clean_slp1[clean_pos] = '\0';
  result->it_chars[result->it_count] = '\0';

  /* Mark the term as having been processed for it-letters */
  if (result->it_count > 0) {
    result->it_samjna = samjna_add(result->it_samjna, SJ_IT);
  }
}

bool anubandha_has_it(const char *upadesa_slp1, char it_char, Samjna context) {
  AnubandhaResult r;
  anubandha_strip(upadesa_slp1, context, &r);
  for (int i = 0; i < r.it_count; i++)
    if (r.it_chars[i] == it_char) return true;
  return false;
}

/* Map a single it-letter to its implied Samjna bit */
Samjna it_to_samjna(char it_char) {
  switch (it_char) {
    case 'k': return SJ_KIT;
    case 'N': return SJ_NGIT;       /* ṅ-it */
    case 'R': return SJ_NNIT;       /* ṇ-it */
    case 'p': return SJ_PIT;
    case 'S': return SJ_SARVADHATUKA;  /* ś-it marks sārvadhatuka */
    default:  return SJ_NONE;
  }
}
