/* varna.c — Story 1.1 implementation
 *
 * Complete 52-phoneme table for Sanskrit with classification flags.
 * Supports sūtra 1.1.9 (savarṇa), 1.1.1 (vṛddhi), 1.1.2 (guṇa).
 */

#include "varna.h"
#include <string.h>

/* ── Full phoneme table (52 phonemes + anusvāra + visarga) ──────────────── */

static const Varna PHONEME_TABLE[] = {
  /* Vowels: VOWEL + length + place */
  { 'a', VF_VOWEL | VF_SHORT | VF_KANTYA | VF_VOICED },
  { 'A', VF_VOWEL | VF_LONG  | VF_KANTYA | VF_VOICED },
  { 'i', VF_VOWEL | VF_SHORT | VF_TALAVYA | VF_VOICED },
  { 'I', VF_VOWEL | VF_LONG  | VF_TALAVYA | VF_VOICED },
  { 'u', VF_VOWEL | VF_SHORT | VF_OSHTHYA | VF_VOICED },
  { 'U', VF_VOWEL | VF_LONG  | VF_OSHTHYA | VF_VOICED },
  { 'f', VF_VOWEL | VF_SHORT | VF_MURDHANYA | VF_VOICED },  /* ṛ */
  { 'F', VF_VOWEL | VF_LONG  | VF_MURDHANYA | VF_VOICED },  /* ṝ */
  { 'x', VF_VOWEL | VF_SHORT | VF_DANTYA    | VF_VOICED },  /* ḷ */
  { 'X', VF_VOWEL | VF_LONG  | VF_DANTYA    | VF_VOICED },  /* ḹ */
  /* Sandhya vowels (always long; dual place) */
  { 'e', VF_VOWEL | VF_LONG  | VF_TALAVYA | VF_KANTYA | VF_VOICED },
  { 'E', VF_VOWEL | VF_LONG  | VF_TALAVYA | VF_KANTYA | VF_VOICED },  /* ai */
  { 'o', VF_VOWEL | VF_LONG  | VF_OSHTHYA | VF_KANTYA | VF_VOICED },
  { 'O', VF_VOWEL | VF_LONG  | VF_OSHTHYA | VF_KANTYA | VF_VOICED },  /* au */

  /* Velar stops (kavarga): KANTYA */
  { 'k', VF_CONSONANT | VF_VOICELESS | VF_KANTYA },
  { 'K', VF_CONSONANT | VF_VOICELESS | VF_ASPIRATE | VF_KANTYA },
  { 'g', VF_CONSONANT | VF_VOICED    | VF_KANTYA },
  { 'G', VF_CONSONANT | VF_VOICED    | VF_ASPIRATE | VF_KANTYA },
  { 'N', VF_CONSONANT | VF_VOICED    | VF_NASAL | VF_NASIKA | VF_KANTYA }, /* ṅ */

  /* Palatal stops (cavarga): TALAVYA */
  { 'c', VF_CONSONANT | VF_VOICELESS | VF_TALAVYA },
  { 'C', VF_CONSONANT | VF_VOICELESS | VF_ASPIRATE | VF_TALAVYA },
  { 'j', VF_CONSONANT | VF_VOICED    | VF_TALAVYA },
  { 'J', VF_CONSONANT | VF_VOICED    | VF_ASPIRATE | VF_TALAVYA },
  { 'Y', VF_CONSONANT | VF_VOICED    | VF_NASAL | VF_NASIKA | VF_TALAVYA }, /* ñ */

  /* Retroflex stops (ṭavarga): MURDHANYA */
  { 'w', VF_CONSONANT | VF_VOICELESS | VF_MURDHANYA },                    /* ṭ */
  { 'W', VF_CONSONANT | VF_VOICELESS | VF_ASPIRATE | VF_MURDHANYA },      /* ṭh */
  { 'q', VF_CONSONANT | VF_VOICED    | VF_MURDHANYA },                    /* ḍ */
  { 'Q', VF_CONSONANT | VF_VOICED    | VF_ASPIRATE | VF_MURDHANYA },      /* ḍh */
  { 'R', VF_CONSONANT | VF_VOICED    | VF_NASAL | VF_NASIKA | VF_MURDHANYA }, /* ṇ */

  /* Dental stops (tavarga): DANTYA */
  { 't', VF_CONSONANT | VF_VOICELESS | VF_DANTYA },
  { 'T', VF_CONSONANT | VF_VOICELESS | VF_ASPIRATE | VF_DANTYA },
  { 'd', VF_CONSONANT | VF_VOICED    | VF_DANTYA },
  { 'D', VF_CONSONANT | VF_VOICED    | VF_ASPIRATE | VF_DANTYA },
  { 'n', VF_CONSONANT | VF_VOICED    | VF_NASAL | VF_NASIKA | VF_DANTYA },

  /* Labial stops (pavarga): OSHTHYA */
  { 'p', VF_CONSONANT | VF_VOICELESS | VF_OSHTHYA },
  { 'P', VF_CONSONANT | VF_VOICELESS | VF_ASPIRATE | VF_OSHTHYA },
  { 'b', VF_CONSONANT | VF_VOICED    | VF_OSHTHYA },
  { 'B', VF_CONSONANT | VF_VOICED    | VF_ASPIRATE | VF_OSHTHYA },
  { 'm', VF_CONSONANT | VF_VOICED    | VF_NASAL | VF_NASIKA | VF_OSHTHYA },

  /* Semivowels (antastha) */
  { 'y', VF_CONSONANT | VF_SEMIVOWEL | VF_VOICED | VF_TALAVYA },
  { 'r', VF_CONSONANT | VF_SEMIVOWEL | VF_VOICED | VF_MURDHANYA },
  { 'l', VF_CONSONANT | VF_SEMIVOWEL | VF_VOICED | VF_DANTYA },
  { 'v', VF_CONSONANT | VF_SEMIVOWEL | VF_VOICED | VF_OSHTHYA },

  /* Sibilants (ūṣman) */
  { 'S', VF_CONSONANT | VF_SIBILANT | VF_VOICELESS | VF_TALAVYA },   /* ś */
  { 'z', VF_CONSONANT | VF_SIBILANT | VF_VOICELESS | VF_MURDHANYA }, /* ṣ */
  { 's', VF_CONSONANT | VF_SIBILANT | VF_VOICELESS | VF_DANTYA },
  { 'h', VF_CONSONANT | VF_SIBILANT | VF_VOICED    | VF_KANTYA },

  /* Special markers */
  { 'M', VF_ANUSVARA | VF_NASAL | VF_VOICED },  /* ṃ */
  { 'H', VF_VISARGA  | VF_VOICELESS },          /* ḥ */
};

#define PHONEME_COUNT ((int)(sizeof(PHONEME_TABLE)/sizeof(PHONEME_TABLE[0])))

/* ── Core lookup ─────────────────────────────────────────────────────────── */

Varna varna_from_slp1(char c) {
  for (int i = 0; i < PHONEME_COUNT; i++)
    if (PHONEME_TABLE[i].slp1 == c) return PHONEME_TABLE[i];
  Varna zero = {0, 0};
  return zero;
}

uint32_t varna_flags(char slp1) {
  return varna_from_slp1(slp1).flags;
}

bool varna_is_valid_slp1(char c) {
  return varna_from_slp1(c).slp1 != 0;
}

/* ── Classification predicates ───────────────────────────────────────────── */

bool varna_is_vowel(char c)     { return (varna_flags(c) & VF_VOWEL) != 0; }
bool varna_is_consonant(char c) { return (varna_flags(c) & VF_CONSONANT) != 0; }
bool varna_is_short(char c)     { return (varna_flags(c) & VF_SHORT) != 0; }
bool varna_is_long(char c)      { return (varna_flags(c) & VF_LONG) != 0; }
bool varna_is_nasal(char c)     { return (varna_flags(c) & VF_NASAL) != 0; }
bool varna_is_semivowel(char c) { return (varna_flags(c) & VF_SEMIVOWEL) != 0; }
bool varna_is_sibilant(char c)  { return (varna_flags(c) & VF_SIBILANT) != 0; }
bool varna_is_voiced(char c)    { return (varna_flags(c) & VF_VOICED) != 0; }
bool varna_is_voiceless(char c) { return (varna_flags(c) & VF_VOICELESS) != 0; }
bool varna_is_aspirate(char c)  { return (varna_flags(c) & VF_ASPIRATE) != 0; }

/* ── Sūtra 1.1.9: tulyāsyaprayatnaṃ savarṇam ─────────────────────────────
 *
 * Two phonemes are savarṇa ("same class") if they share:
 *   (a) place of articulation (āsya)
 *   (b) internal effort (ābhyantara prayatna): voicing + aspiration + nasality
 *
 * Vowel length is NOT a distinguishing feature for savarṇa:
 *   a and ā are savarṇa; i and ī are savarṇa; ṛ and ṝ are savarṇa; etc.
 */
bool varna_is_savarna(char a, char b) {
  if (a == b) return true;
  Varna va = varna_from_slp1(a);
  Varna vb = varna_from_slp1(b);
  if (va.slp1 == 0 || vb.slp1 == 0) return false;

  /* Both must be either vowels or consonants — can't cross-class */
  bool va_vowel = (va.flags & VF_VOWEL) != 0;
  bool vb_vowel = (vb.flags & VF_VOWEL) != 0;
  if (va_vowel != vb_vowel) return false;

  /* Place must match exactly (mask out length/short/long/aspirate/etc.) */
  uint32_t place_mask = VF_KANTYA | VF_TALAVYA | VF_MURDHANYA | VF_DANTYA | VF_OSHTHYA;
  if ((va.flags & place_mask) != (vb.flags & place_mask)) return false;

  /* For vowels: place match is sufficient (length ignored by 1.1.9) */
  if (va_vowel) return true;

  /* For consonants: must also match voicing, aspiration, and nasality */
  uint32_t effort_mask = VF_VOICED | VF_VOICELESS | VF_ASPIRATE | VF_NASAL | VF_SEMIVOWEL | VF_SIBILANT;
  return (va.flags & effort_mask) == (vb.flags & effort_mask);
}

/* Get the canonical representative of a savarṇa class (short form for vowels) */
char varna_savarna_rep(char c) {
  switch (c) {
    case 'A': return 'a';
    case 'I': return 'i';
    case 'U': return 'u';
    case 'F': return 'f';
    case 'X': return 'x';
    default:  return c;
  }
}

char varna_lengthen(char c) {
  switch (c) {
    case 'a': return 'A';
    case 'i': return 'I';
    case 'u': return 'U';
    case 'f': return 'F';
    case 'x': return 'X';
    default:  return c;
  }
}

char varna_shorten(char c) {
  switch (c) {
    case 'A': return 'a';
    case 'I': return 'i';
    case 'U': return 'u';
    case 'F': return 'f';
    case 'X': return 'x';
    default:  return c;
  }
}

/* Sūtra 1.1.2 adeṅ guṇaḥ — guṇa substitution
 *   a/ā   → a (already guṇa)
 *   i/ī   → e
 *   u/ū   → o
 *   ṛ/ṝ   → ar  (two characters — caller must handle — we return 'a' and add 'r' context)
 *   ḷ/ḹ   → al  (similar)
 *
 * For single-char return: ṛ→a, ḷ→a (caller inserts 'r'/'l' via separate logic)
 */
char varna_guna(char c) {
  switch (c) {
    case 'a': case 'A': return 'a';
    case 'i': case 'I': return 'e';
    case 'u': case 'U': return 'o';
    case 'f': case 'F': return 'a';   /* ṛ → ar (caller handles 'r') */
    case 'x': case 'X': return 'a';   /* ḷ → al (caller handles 'l') */
    default:            return c;
  }
}

/* Sūtra 1.1.1 vṛddhirādaic — vṛddhi substitution
 *   a/ā   → ā
 *   i/ī   → ai (E)
 *   u/ū   → au (O)
 *   ṛ/ṝ   → ār (caller adds 'r')
 *   ḷ     → āl (caller adds 'l')
 */
char varna_vrddhi(char c) {
  switch (c) {
    case 'a': case 'A': return 'A';
    case 'i': case 'I': return 'E';
    case 'u': case 'U': return 'O';
    case 'f': case 'F': return 'A';
    case 'x': case 'X': return 'A';
    default:            return c;
  }
}
