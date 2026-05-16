/* reduplication.c — abhyāsa machinery per Pāṇini 6.1.8/6.1.10 +
   7.4.59 hrasvaḥ + 7.4.60 halādiḥ śeṣaḥ + 7.4.62 kuhoś cuḥ.

   Algorithm for a dhātu like BU/kf/gam/hu:
     1. Identify the first syllable of the root (initial consonant
        cluster + first vowel; some roots are vowel-initial).
     2. 7.4.60 halādiḥ śeṣaḥ: keep only the FIRST consonant of the
        initial cluster in the abhyāsa.
     3. 7.4.59 hrasvaḥ: shorten the abhyāsa vowel (A→a, I→i, U→u,
        F→f).
     4. 7.4.62 kuhoś cuḥ: a velar (k/K/g/G) or laryngeal h in the
        abhyāsa initial becomes the corresponding palatal (c/c/j/j/j).
     5. Concatenate abhyāsa + original root.
*/
#include "reduplication.h"
#include "varna.h"
#include <string.h>

static char shorten_vowel(char v) {
  switch (v) {
    case 'A': return 'a';
    case 'I': return 'i';
    case 'U': return 'u';
    case 'F': return 'f';
    case 'X': return 'x';
    case 'E': return 'i';   /* ai → i in abhyāsa */
    case 'O': return 'u';   /* au → u */
    case 'e': return 'i';   /* e → i */
    case 'o': return 'u';   /* o → u */
    default:  return v;
  }
}

static char palatalize(char c) {
  switch (c) {
    case 'k': case 'K': return 'c';
    case 'g': case 'G': return 'j';
    case 'h':           return 'j';   /* 7.4.62 hoś — h → j in abhyāsa */
    default:            return c;
  }
}

bool reduplicate(const char *clean_root, char *out, size_t out_len) {
  if (!clean_root || !out || out_len == 0) return false;
  size_t n = strlen(clean_root);
  if (n == 0) return false;

  /* Find the first vowel position. The chars before it form the
     initial cluster. */
  size_t first_vowel = n;
  for (size_t i = 0; i < n; i++) {
    if (varna_is_vowel(clean_root[i])) { first_vowel = i; break; }
  }
  if (first_vowel == n) return false;  /* no vowel — not a dhātu */

  char abhyasa_cons = (first_vowel > 0) ? clean_root[0] : 0;
  char abhyasa_vowel = shorten_vowel(clean_root[first_vowel]);
  if (abhyasa_cons) abhyasa_cons = palatalize(abhyasa_cons);

  /* Build "abhyasa + root". For vowel-initial roots, abhyasa is just
     the shortened initial vowel (e.g. AS → aas → As — actually
     vowel-initial roots use a special "AT" augment instead, but
     we leave that to the LIT-specific code). */
  size_t pos = 0;
  if (abhyasa_cons) {
    if (pos + 1 >= out_len) return false;
    out[pos++] = abhyasa_cons;
  }
  if (pos + 1 >= out_len) return false;
  out[pos++] = abhyasa_vowel;
  if (pos + n + 1 > out_len) return false;
  memcpy(out + pos, clean_root, n);
  out[pos + n] = '\0';
  return true;
}
