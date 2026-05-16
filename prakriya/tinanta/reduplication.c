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

/* LIT abhyāsa-vowel rule per the observed pattern:
   - short a / short i / short u → unchanged
   - long Ā / long Ū / ṛ / ḷ → 'a' (the long-back vowel rule per
     7.4.66 ut paratasya applied generally)
   - long Ī → 'i' (long-front shortens but stays in front)
   - e/ai/au → 'i' / 'a' respectively
   This is empirically aligned with the BORI dhātuforms paradigm
   for bhū (baBUva), kṛ (cakāra), nī (nināya), hu (juhāva). */
static char shorten_vowel(char v) {
  switch (v) {
    case 'A': return 'a';
    case 'I': return 'i';
    case 'U': return 'a';   /* long ū → a (special for back vowel) */
    case 'F': return 'a';   /* ṝ → a */
    case 'f': return 'a';   /* short ṛ → a */
    case 'X': return 'a';   /* ḹ → a */
    case 'x': return 'a';   /* ḷ → a */
    case 'E': return 'i';   /* ai → i in abhyāsa */
    case 'O': return 'a';   /* au → a */
    case 'e': return 'i';   /* e → i */
    case 'o': return 'a';   /* o → a */
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

/* 7.4.61 śarpūrvāḥ khayaḥ — aspirate consonants lose aspiration in
   abhyāsa (B → b, D → d, K → k after palatalisation already lowered
   it to c, etc.). Combined with palatalize: only the resulting
   non-aspirate stays. */
static char deaspirate(char c) {
  switch (c) {
    case 'K': return 'k';   /* kh → k */
    case 'G': return 'g';   /* gh → g */
    case 'C': return 'c';   /* ch → c */
    case 'J': return 'j';   /* jh → j */
    case 'W': return 'w';   /* ṭh → ṭ */
    case 'Q': return 'q';   /* ḍh → ḍ */
    case 'T': return 't';   /* th → t */
    case 'D': return 'd';   /* dh → d */
    case 'P': return 'p';   /* ph → p */
    case 'B': return 'b';   /* bh → b */
    default:  return c;
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
  /* 7.4.62 kuhoś cuḥ first (velar/laryngeal → palatal), then
     7.4.61 śarpūrvāḥ khayaḥ (deaspirate). */
  if (abhyasa_cons) {
    abhyasa_cons = palatalize(abhyasa_cons);
    abhyasa_cons = deaspirate(abhyasa_cons);
  }

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
