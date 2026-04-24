/* sandhi_visarga.c — Story 1.5 implementation
 *
 * Visarga sandhi (8.3.15–8.3.37).
 * Called when a word ends in ḥ (SLP1 'H'); a_prefinal = char before the ḥ.
 *
 * Rules:
 *   aḥ + voiced          → o         (8.3.17 / 6.1.113)
 *   aḥ + a (special)     → o' (with avagraha, we output o)
 *   āḥ + any             → ā         (8.3.17)
 *   iḥ/uḥ + voiced       → ir / ur   (8.3.15: aḥ)
 *   ḥ + voiceless        → ḥ         (stays)
 */

#include "sandhi_visarga.h"
#include "varna.h"
#include <string.h>

#define SUTRA_8_3_15  830015
#define SUTRA_8_3_17  830017

SandhiResult sandhi_visarga_apply(char a_prefinal, char b_initial) {
  SandhiResult r;
  memset(&r, 0, sizeof(r));
  r.changed = false;

  /* Is b_initial voiced? (any vowel, or voiced consonant) */
  bool b_voiced = varna_is_vowel(b_initial) ||
                   varna_is_voiced(b_initial) ||
                   varna_is_semivowel(b_initial) ||
                   varna_is_nasal(b_initial);

  /* Long ā + ḥ → ā + (drop ḥ) before any consonant */
  if (a_prefinal == 'A' && varna_is_consonant(b_initial) && !b_voiced) {
    /* āḥ + voiceless → āḥ (stays) */
    r.result_slp1[0] = 'H';
    r.result_slp1[1] = b_initial;
    r.result_slp1[2] = '\0';
    r.rule_applied = SUTRA_8_3_17;
    return r;
  }
  if (a_prefinal == 'A') {
    /* āḥ + voiced → ā (visarga drops) */
    r.result_slp1[0] = b_initial;
    r.result_slp1[1] = '\0';
    r.changed = true;
    r.rule_applied = SUTRA_8_3_17;
    return r;
  }

  /* Short a + ḥ + voiced → o */
  if (a_prefinal == 'a' && b_voiced) {
    /* Replace aH with o */
    r.result_slp1[0] = 'o';
    r.result_slp1[1] = b_initial;
    r.result_slp1[2] = '\0';
    r.changed = true;
    r.rule_applied = SUTRA_8_3_15;
    return r;
  }

  /* i/u/other vowel + ḥ + voiced → r */
  if (varna_is_vowel(a_prefinal) && b_voiced) {
    r.result_slp1[0] = 'r';
    r.result_slp1[1] = b_initial;
    r.result_slp1[2] = '\0';
    r.changed = true;
    r.rule_applied = SUTRA_8_3_15;
    return r;
  }

  /* Before voiceless → ḥ stays */
  r.result_slp1[0] = 'H';
  r.result_slp1[1] = b_initial;
  r.result_slp1[2] = '\0';
  r.rule_applied = SUTRA_8_3_15;
  return r;
}
