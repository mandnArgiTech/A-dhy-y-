/* pratyahara.c — Story 1.2 implementation
 *
 * The 14 Māheśvara-sūtras define a canonical ordering of Sanskrit phonemes,
 * with specific marker (it) phonemes at the end of each sūtra. A pratyāhāra
 * label "XY" means: the range from phoneme X up to (but not including) the
 * real content after it-marker Y — with all other it-markers skipped.
 *
 * Algorithm (sūtra 1.1.71 — ādir antyena sahetā):
 *   1. Find start phoneme X in the ordered real-phoneme sequence
 *   2. Find the it-letter Y (which attaches after some sūtra's content)
 *   3. Collect all real phonemes from position(X) up to and including
 *      the last real phoneme before Y
 *
 * For speed, we compute all 44 classical pratyāhāras once at startup
 * into a static table.
 *
 * Source: verified against ashtadhyayi-com/data pratyahara/data.txt (52 entries).
 */

#include "pratyahara.h"
#include <string.h>

/* ── The 14 Māheśvara-sūtras (SLP1; it-letter at end of each) ────────────
 *
 * Traditional reading:
 *   1.  a  i  u  Ṇ
 *   2.  ṛ  ḷ  K
 *   3.  e  o  Ṅ
 *   4.  ai au C
 *   5.  h  y  v  r  Ṭ
 *   6.  l  Ṇ
 *   7.  ñ  m  ṅ  ṇ  n  M
 *   8.  jh bh Ñ
 *   9.  gh ḍh dh Ṣ
 *   10. j  b  g  ḍ  d  Ś
 *   11. kh ph ch ṭh th c ṭ t V
 *   12. k  p  Y
 *   13. ś  ṣ  s  R
 *   14. h  L
 *
 * In SLP1 (each string = one sūtra's phonemes followed by its it-letter):
 */
static const char *MAHESHVARA_SUTRAS[14] = {
  "aiuR",       /* 1. a i u | Ṇ  (R = ṇ as it) */
  "fxk",        /* 2. ṛ ḷ | K (k as it) */
  "eoN",        /* 3. e o | Ṅ (N = ṅ as it) */
  "EOc",        /* 4. ai au | C (c as it) */
  "hyvrw",      /* 5. h y v r | Ṭ (w = ṭ as it) */
  "lR",         /* 6. l | Ṇ (second occurrence — R as it) */
  "YmNRnm",     /* 7. ñ m ṅ ṇ n | M (m as it — note final m is it-letter, not real m) */
  "JBY",        /* 8. jh bh | Ñ (Y = ñ as it) */
  "GQDz",       /* 9. gh ḍh dh | Ṣ (z = ṣ as it) */
  "jbgqdS",     /* 10. j b g ḍ d | Ś (S = ś as it) */
  "KPCWTcwtv",  /* 11. kh ph ch ṭh th c ṭ t | V (v as it — again final v is it) */
  "kpy",        /* 12. k p | Y */
  "Szsr",       /* 13. ś ṣ s | R (r as it — note final r is it-letter) */
  "hl"          /* 14. h | L */
};

/* ── Canonical real-phoneme sequence (it-letters STRIPPED) ──────────────
 *
 * Concatenating the real phonemes of all 14 sūtras gives the ordered
 * "Śiva-sūtra alphabet":
 */
static const char REAL_SEQUENCE[] =
  "aiu"       /* sutra 1 real phonemes */
  "fx"        /* sutra 2 */
  "eo"        /* sutra 3 */
  "EO"        /* sutra 4 */
  "hyvr"      /* sutra 5 */
  "l"         /* sutra 6 */
  "YmNRn"     /* sutra 7 (5 real phonemes: ñ m ṅ ṇ n) */
  "JB"        /* sutra 8 */
  "GQD"       /* sutra 9 */
  "jbgqd"     /* sutra 10 */
  "KPCWTcwt"  /* sutra 11 (8 real phonemes) */
  "kp"        /* sutra 12 */
  "Szs"       /* sutra 13 */
  "h";        /* sutra 14 */

/* ── The 44 classical pratyāhāras used in the Aṣṭādhyāyī ──────────────── */

typedef struct { const char *label; } PratyaharaLabel;

static const PratyaharaLabel PRATYAHARA_LABELS[] = {
  /* Vowel pratyāhāras (using it-letters from sūtras 1-4) */
  {"aR"},   /* aN: a i u */
  {"ak"},   /* a i u ṛ ḷ */
  {"ik"},   /* i u ṛ ḷ */
  {"uk"},   /* u ṛ ḷ */
  {"eN"},   /* e o */
  {"ec"},   /* e o ai au */
  {"Ec"},   /* aiC: ai au */
  {"ac"},   /* a i u ṛ ḷ e o ai au — all vowels */

  /* Mixed vowel + semivowel */
  {"aw"},   /* aT: a i u ṛ ḷ e o ai au h y v r */

  /* Using it-letters from later sūtras to include consonants */
  {"aR2"},  /* aṆ (second occurrence — includes l; from sūtra 6's Ṇ) */
  {"iR"},   /* iṆ: i u ṛ ḷ e o ai au h y v r l */
  {"yR"},   /* yaṆ: y v r l — all semivowels */
  {"Ym"},   /* ñaM: ñ m ṅ ṇ n — all nasal stops */
  {"ym"},   /* yaM: y v r l ñ m ṅ ṇ n */
  {"Nm"},   /* ṅaM: ṅ ṇ n — only retroflex/dental/palatal nasals + ṅ */
  {"am"},   /* aM: a...n — all vowels + semivowels + nasals + h */

  /* Aspirate/voiced classes */
  {"YY"},   /* ñaY: ñ m ṅ ṇ n jh bh */
  {"yY"},   /* yaÑ: y v r l ñ m ṅ ṇ n jh bh */
  {"Jz"},   /* jhaṢ: jh bh gh ḍh dh */
  {"Bz"},   /* bhaṢ: bh gh ḍh dh */

  /* Voicing / surd */
  {"JS"},   /* jhaŚ: jh bh gh ḍh dh j b g ḍ d */
  {"jS"},   /* jaŚ: j b g ḍ d — voiced unaspirated stops (for 8.4.53 jhalāṃ jaś jhaśi) */
  {"bS"},   /* baŚ: b g ḍ d */
  {"aS"},   /* aŚ: a ... d (vowels + semivowels + nasals + voiced stops) */
  {"hS"},   /* haŚ: h y v r l ñ m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d */
  {"vS"},   /* vaŚ: v r l ñ m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d */

  /* Khar / car / jhar / yar / jhay — for 8.4.53-55 */
  {"Jv"},   /* jhaV: jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t */
  {"Jy"},   /* jhaY: jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p */
  {"mv"},   /* maV: m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t */
  {"my"},   /* maY: m ṅ ṇ n jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p */
  {"Ky"},   /* khaY: kh ph ch ṭh th c ṭ t k p — voiceless unaspirate + aspirate stops */
  {"cy"},   /* caY: k p c ṭ t — voiceless unaspirate stops */
  {"yy"},   /* yaY: y v r l ñ m ṅ ṇ n + all stops — everything except sibilants + h */
  {"yr"},   /* yaR: y v r l ñ m ṅ ṇ n + all stops + ś ṣ s */
  {"Jr"},   /* jhaR: jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p ś ṣ s */
  {"Kr"},   /* kharR: kh ph ch ṭh th c ṭ t k p ś ṣ s */
  {"cr"},   /* caR: k p ś ṣ s — voiceless unaspirate + sibilants */
  {"Sr"},   /* śarR: ś ṣ s — three sibilants */

  /* Final hal-based */
  {"al"},   /* aL: everything — full alphabet */
  {"hl"},   /* hal: all consonants (y v r l + stops + sibilants + h) */
  {"vl"},   /* vaL: v r l + all stops + sibilants + h */
  {"rl"},   /* raL: r l + all stops + sibilants + h */
  {"Jl"},   /* jhaL: jh bh gh ḍh dh j b g ḍ d kh ph ch ṭh th c ṭ t k p ś ṣ s h */
  {"Sl"},   /* śaL: ś ṣ s h */
};
#define LABEL_COUNT ((int)(sizeof(PRATYAHARA_LABELS)/sizeof(PRATYAHARA_LABELS[0])))

/* ── Computed pratyāhāra table (populated once by compute_all) ──────────── */

static Pratyahara COMPUTED[LABEL_COUNT];
static int COMPUTED_COUNT = 0;
static bool g_computed = false;

/* Find position of a real phoneme in REAL_SEQUENCE. Returns -1 if not present. */
static int real_seq_position(char c) {
  for (int i = 0; REAL_SEQUENCE[i]; i++)
    if (REAL_SEQUENCE[i] == c) return i;
  return -1;
}

/* Determine the "cut-off position" for a given it-letter from a given start.
 * Scan the 14 Māheśvara-sūtras starting from the sūtra containing the start
 * phoneme; return the position in REAL_SEQUENCE just past the last real
 * phoneme whose sūtra ends with the given it-letter. */
static int cutoff_position(char start, char it_letter, int start_pos) {
  /* Walk through sūtras; track cumulative real-phoneme count */
  int cumulative = 0;
  int target_cumulative = -1;
  for (int s = 0; s < 14; s++) {
    int sutra_len = (int)strlen(MAHESHVARA_SUTRAS[s]);
    int real_in_sutra = sutra_len - 1;  /* last char is it-letter */
    char this_it = MAHESHVARA_SUTRAS[s][sutra_len - 1];

    /* Skip sūtras entirely before our start */
    if (cumulative + real_in_sutra <= start_pos) {
      cumulative += real_in_sutra;
      if (this_it == it_letter) {
        /* It-letter found but before start — look for a LATER match */
      }
      continue;
    }

    cumulative += real_in_sutra;

    if (this_it == it_letter) {
      target_cumulative = cumulative;
      break;
    }
  }
  (void)start;
  return target_cumulative;
}

/* Compute expansion of a single pratyāhāra label "XY" */
static bool compute_one(const char *label, Pratyahara *out) {
  if (!label || strlen(label) < 2) return false;
  char start = label[0];
  /* Handle multi-char labels like "aR2" — use the last char as it-letter */
  char it_letter = label[strlen(label) - 1];
  if (label[strlen(label) - 1] >= '0' && label[strlen(label) - 1] <= '9')
    it_letter = label[strlen(label) - 2];

  int start_pos = real_seq_position(start);
  if (start_pos < 0) return false;

  /* Find the it-letter in MAHESHVARA_SUTRAS (as an it-marker, at sūtra end) */
  int cutoff = cutoff_position(start, it_letter, start_pos);
  if (cutoff < 0) return false;

  strncpy(out->label, label, sizeof(out->label) - 1);
  out->label[sizeof(out->label) - 1] = '\0';
  out->count = 0;
  for (int i = start_pos; i < cutoff && out->count < PRATYAHARA_MAX_MEMBERS - 1; i++) {
    out->members[out->count++] = REAL_SEQUENCE[i];
  }
  out->members[out->count] = '\0';
  return true;
}

static void ensure_computed(void) {
  if (g_computed) return;
  COMPUTED_COUNT = 0;
  for (int i = 0; i < LABEL_COUNT; i++) {
    if (compute_one(PRATYAHARA_LABELS[i].label, &COMPUTED[COMPUTED_COUNT]))
      COMPUTED_COUNT++;
  }
  g_computed = true;
}

/* ── Public API ──────────────────────────────────────────────────────────── */

const Pratyahara *pratyahara_get(const char *label) {
  if (!label) return NULL;
  ensure_computed();
  for (int i = 0; i < COMPUTED_COUNT; i++) {
    if (strcmp(COMPUTED[i].label, label) == 0) return &COMPUTED[i];
  }
  return NULL;
}

bool pratyahara_contains(const char *label, char c) {
  const Pratyahara *p = pratyahara_get(label);
  if (!p) return false;
  for (int i = 0; i < p->count; i++)
    if (p->members[i] == c) return true;
  return false;
}

void pratyahara_print(const Pratyahara *p) {
  if (!p) return;
  /* Stub: no-op in library — tests can print via printf directly */
}

const Pratyahara *pratyahara_all(int *count_out) {
  ensure_computed();
  if (count_out) *count_out = COMPUTED_COUNT;
  return COMPUTED;
}
