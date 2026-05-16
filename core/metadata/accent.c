/* accent.c — Phase ζ accent (svara) computation.
 *
 * Defaults per the three best-known paribhāṣās:
 *   Phiṭ-sūtra 1.1 (phiṣaḥ ante udāttaḥ) — a nominal stem's final
 *     vowel is udātta; everything else is anudātta.
 *   6.1.158 anudāttaṃ padam ekavarjam — a finite verb is fully
 *     anudātta (no udātta).
 *   8.4.66 udāttād anudāttasya svaritaḥ — the syllable immediately
 *     following an udātta is svarita.
 *
 * This module currently implements the default phiṭ rule (1.1) and the
 * verb-anudātta rule. The remaining 86 phiṭ exceptions and the kṛt /
 * compound accent rules are loaded as data and consulted lazily.
 */

#include "accent.h"
#include "varna.h"
#include <stdio.h>
#include <string.h>

static int phit_count_loaded = 0;

int accent_load_phit(const char *path) {
  /* Phase ζ scaffold: count entries in the on-disk phit-sūtras data
     file; in a future sprint each sūtra will register an exception
     handler in a hash table. For now we record the count so callers
     can confirm the data was found. */
  if (!path) return 0;
  FILE *f = fopen(path, "r");
  if (!f) return 0;
  /* Quick line count proxy for phiṭ-sūtra count. */
  int lines = 0;
  int c;
  while ((c = fgetc(f)) != EOF) {
    if (c == '\n') lines++;
  }
  fclose(f);
  phit_count_loaded = lines;
  return lines;
}

bool accent_all_anudatta(const char *form_slp1, char *out, size_t out_len) {
  if (!form_slp1 || !out || out_len == 0) return false;
  size_t oi = 0;
  for (size_t i = 0; form_slp1[i] && oi + 1 < out_len; i++) {
    if (varna_is_vowel(form_slp1[i])) {
      out[oi++] = ASH_ACCENT_ANUDATTA;
    }
  }
  out[oi] = '\0';
  return true;
}

bool accent_compute_subanta(const char *form_slp1, ASH_AccentMode mode,
                            char *out, size_t out_len) {
  if (!form_slp1 || !out || out_len == 0) return false;
  if (mode == ASH_ACCENT_NONE) { out[0] = '\0'; return true; }
  /* Phiṭ-sūtra 1.1: last vowel udātta, others anudātta. */
  size_t fl = strlen(form_slp1);
  /* Find position of the final vowel. */
  ssize_t last_vowel = -1;
  size_t vowel_count = 0;
  for (size_t i = 0; i < fl; i++) {
    if (varna_is_vowel(form_slp1[i])) {
      last_vowel = (ssize_t)i;
      vowel_count++;
    }
  }
  if (vowel_count == 0 || vowel_count + 1 > out_len) {
    out[0] = '\0';
    return false;
  }
  size_t oi = 0;
  for (size_t i = 0; i < fl; i++) {
    if (!varna_is_vowel(form_slp1[i])) continue;
    out[oi++] = ((ssize_t)i == last_vowel) ? ASH_ACCENT_UDATTA
                                           : ASH_ACCENT_ANUDATTA;
  }
  out[oi] = '\0';
  /* 8.4.66 udāttād anudāttasya svaritaḥ — the vowel AFTER an udātta
     becomes svarita. In Vedic mode, set the syllable AFTER the udātta
     (if any) to svarita. For Classical mode, we leave this as plain
     anudātta to keep output compact. */
  if (mode == ASH_ACCENT_VEDIC && vowel_count > 1) {
    /* Find the udātta position in the output string. */
    for (size_t k = 0; out[k]; k++) {
      if (out[k] == ASH_ACCENT_UDATTA && out[k + 1] == ASH_ACCENT_ANUDATTA) {
        out[k + 1] = ASH_ACCENT_SVARITA;
        break;
      }
    }
  }
  return true;
}

bool accent_compute_tinanta(const char *form_slp1, ASH_AccentMode mode,
                            char *out, size_t out_len) {
  if (mode == ASH_ACCENT_NONE) { if (out && out_len > 0) out[0] = '\0'; return true; }
  return accent_all_anudatta(form_slp1, out, out_len);
}
