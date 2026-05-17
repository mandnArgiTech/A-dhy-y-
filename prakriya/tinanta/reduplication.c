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
    case 'U': return 'u';   /* hrasva: ū → u */
    case 'F': return 'a';   /* ṝ → a (7.4.66/7.4.59) */
    case 'f': return 'a';   /* short ṛ → a */
    case 'X': return 'a';   /* ḹ → a */
    case 'x': return 'a';   /* ḷ → a */
    case 'E': return 'a';   /* ai → a in abhyāsa for ai-final dhātus
                               (glē, mlē etc., per 7.4.66 ut paratasya
                               applied for ec-final). */
    case 'O': return 'u';   /* au → u in abhyāsa (u is the latter part) */
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

  /* 7.4.61 śarpūrvāḥ khayaḥ — in an initial consonant cluster where
     the FIRST consonant is a śar (S/z/s) and the second is a khay
     (k/K/g/G/c/C/j/J/w/W/q/Q/t/T/d/D/p/P), the khay survives in the
     abhyāsa instead of the śar. */
  char abhyasa_cons = 0;
  if (first_vowel > 0) {
    abhyasa_cons = clean_root[0];
    if (first_vowel >= 2 &&
        (clean_root[0] == 'S' || clean_root[0] == 'z' || clean_root[0] == 's')) {
      char c2 = clean_root[1];
      bool c2_khay = (c2 == 'k' || c2 == 'K' || c2 == 'g' || c2 == 'G' ||
                      c2 == 'c' || c2 == 'C' || c2 == 'j' || c2 == 'J' ||
                      c2 == 'w' || c2 == 'W' || c2 == 'q' || c2 == 'Q' ||
                      c2 == 't' || c2 == 'T' || c2 == 'd' || c2 == 'D' ||
                      c2 == 'p' || c2 == 'P');
      if (c2_khay) abhyasa_cons = c2;
    }
  }
  char abhyasa_vowel = shorten_vowel(clean_root[first_vowel]);
  /* For consonant + 'y' clusters, the abhyāsa-vowel is 'i' rather
     than the root vowel (dyut → didyot not dudyot, syand → sisyand).
     Cv/Cr clusters keep the root vowel as the abhyāsa-vowel
     (tvanc → tatvanc, Svaca → SaSvac). */
  if (first_vowel >= 2 && clean_root[1] == 'y') {
    abhyasa_vowel = 'i';
  }
  /* 7.4.62 kuhoś cuḥ first (velar/laryngeal → palatal), then
     7.4.61 śarpūrvāḥ khayaḥ (deaspirate). */
  if (abhyasa_cons) {
    abhyasa_cons = palatalize(abhyasa_cons);
    abhyasa_cons = deaspirate(abhyasa_cons);
  }

  /* Build "abhyasa + root". For vowel-initial roots, the abhyāsa
     vowel and the root-initial vowel are savarṇa, so per 6.1.101
     akaḥ savarṇe dīrghaḥ they merge into the corresponding long
     vowel (a + a → ā, i + i → ī, u + u → ū). */
  size_t pos = 0;
  if (abhyasa_cons) {
    if (pos + 1 >= out_len) return false;
    out[pos++] = abhyasa_cons;
  }
  if (first_vowel == 0) {
    /* Vowel-initial root. Count consonants after the initial vowel up
       to the next vowel (or end).
       Per 6.1.101 + 6.4.77: i/u-initial single-cons roots take the
       iyaṅ/uvaṅ-ādeśa path (no merge — abhyāsa stays short, root
       vowel gets guṇa, the i/u + V boundary later becomes iy/uv +
       V). For a-initial we still merge to long Ā.
       Multi-cons clusters (≥2 consonants after V) use "An"-abhyāsa
       (arda → Anarda). */
    size_t cons_after = 0;
    for (size_t i = 1; i < n; i++) {
      if (varna_is_vowel(clean_root[i])) break;
      cons_after++;
    }
    char root_v = clean_root[0];
    char merged_a = 0;
    if ((abhyasa_vowel == 'a' && (root_v == 'a' || root_v == 'A')) ||
        (abhyasa_vowel == 'A' && (root_v == 'a' || root_v == 'A'))) {
      merged_a = 'A';
    }
    char merged_iu = 0;
    if ((abhyasa_vowel == 'i' && (root_v == 'i' || root_v == 'I')) ||
        (abhyasa_vowel == 'I' && (root_v == 'i' || root_v == 'I'))) {
      merged_iu = 'I';
    } else if ((abhyasa_vowel == 'u' && (root_v == 'u' || root_v == 'U')) ||
               (abhyasa_vowel == 'U' && (root_v == 'u' || root_v == 'U'))) {
      merged_iu = 'U';
    }
    /* Multi-consonant cluster: use "An"-abhyāsa pattern for any
       vowel-initial root (covers ard, arc, idi, uK after num).
       Short ṛ-initial roots like fj, fc also use An-abhyāsa even
       with a single consonant following the ṛ (oracle: fj → Anfje). */
    bool rinit_single = (root_v == 'f' && cons_after >= 1);
    if (cons_after >= 2 || rinit_single) {
      char vowel_for_an = merged_a ? merged_a :
                          (merged_iu == 'I' ? 'I' :
                           merged_iu == 'U' ? 'U' :
                           (root_v == 'f') ? 'A' : 0);
      if (vowel_for_an) {
        if (pos + 2 + n + 1 > out_len) return false;
        out[pos++] = vowel_for_an;
        out[pos++] = 'n';
        memcpy(out + pos, clean_root, n);
        out[pos + n] = '\0';
        return true;
      }
    }
    /* Single-cons clusters: a-initial merges (ata → Āta), but
       i/u-initial does NOT merge — the i/u stays as the abhyāsa,
       and the root vowel keeps its full identity for guṇa to
       operate. uKa → u + uKa → guṇa + uvaṅ → uvoKa. */
    if (merged_a) {
      if (pos + 1 >= out_len) return false;
      out[pos++] = merged_a;
      if (pos + (n - 1) + 1 > out_len) return false;
      memcpy(out + pos, clean_root + 1, n - 1);
      out[pos + n - 1] = '\0';
      return true;
    }
    /* For i/u-initial single-cons roots: fall through to the default
       "abhyāsa-vowel + root" path. This produces "i" + "iK" = "iiK"
       and the LIT branch will apply guṇa to the second i (root vowel),
       turning it into "ieK", then post-process iyaṅ-ādeśa converts
       "ieK" → "iyeK". */
  }
  if (pos + 1 >= out_len) return false;
  out[pos++] = abhyasa_vowel;
  if (pos + n + 1 > out_len) return false;
  memcpy(out + pos, clean_root, n);
  out[pos + n] = '\0';
  return true;
}
