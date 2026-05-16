/* sandhi_natva.c — implementation of 8.4.1 ṇatva and 8.3.59 ṣatva.
   Single authoritative copies; previously duplicated across four
   files in prakriya/. */
#include "sandhi_natva.h"
#include "varna.h"
#include <stdbool.h>
#include <stddef.h>

static bool natva_is_blocker(char c) {
  switch (c) {
    case 't': case 'T': case 'd': case 'D':           /* dental */
    case 'c': case 'C': case 'j': case 'J': case 'Y': /* palatal */
    case 'w': case 'W': case 'q': case 'Q':           /* retroflex non-ṇ */
    case 'R':                                          /* ṇ itself — propagation halt */
    case 'S': case 's':                                /* ś / s (z=ṣ is a trigger) */
    case 'l':
      return true;
  }
  return false;
}

void sandhi_apply_natva(char *form) {
  if (!form) return;
  bool seen = false;
  for (size_t i = 0; form[i]; i++) {
    char c = form[i];
    if (c == 'r' || c == 'f' || c == 'z' || c == 'F') {
      seen = true;
    } else if (natva_is_blocker(c)) {
      seen = false;
    } else if (seen && c == 'n' && form[i + 1]) {
      /* 8.4.1 raṣābhyāṃ no ṇaḥ — fire when 'n' is followed by a
         vowel or by a semivowel/nasal (v/y/m/n/r) in the same word.
         Conservative-skip for plain consonant clusters; 8.4.37
         padāntasya covers pada-end so we don't need to special-case
         end-of-form here. */
      char next = form[i + 1];
      bool ok = varna_is_vowel(next) ||
                next == 'v' || next == 'y' || next == 'm' ||
                next == 'n' || next == 'r';
      if (ok) {
        form[i] = 'R';
        seen = false;
      }
    }
  }
}

void sandhi_apply_satva(char *form, size_t stem_split) {
  if (!form) return;
  for (size_t i = stem_split; form[i]; i++) {
    if (form[i] != 's' || i == 0) continue;
    char prev = form[i - 1];
    bool in_iN = (prev == 'i' || prev == 'I' || prev == 'u' || prev == 'U' ||
                  prev == 'f' || prev == 'F' || prev == 'x' || prev == 'X' ||
                  prev == 'e' || prev == 'o' || prev == 'E' || prev == 'O' ||
                  prev == 'k');
    if (in_iN) form[i] = 'z';
  }
}
