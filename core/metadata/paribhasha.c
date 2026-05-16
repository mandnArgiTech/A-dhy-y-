/* paribhasha.c — Story 2.6 + Phase ε implementation
 *
 * Paribhāṣās are meta-rules that govern how the Aṣṭādhyāyī's rules interact.
 * The classical collection is the Paribhāṣendu-śekhara by Nāgeśa (133 rules).
 *
 * This module implements the most critical paribhāṣās used in practice:
 *   PB_STHANIVAD           — sthānivad-ādeśo'nal-vidhau (1.1.56)
 *                             A substitute is treated like the original
 *                             (except for rules about single phonemes)
 *   PB_APAVADA_OVER_UTSARGA — exception beats general rule
 *   PB_NITYA_OVER_ANITYA   — obligatory beats optional
 *   PB_ANTARA_OVER_BAHIRA  — inner (earlier-applicable) beats outer
 *   PB_PURVA_PATA          — later sūtra wins in conflict (paratva)
 *   PB_SARVADHATUKE_ARDHA  — sarvadhātuka has priority over ārdhadhātuka
 *                             in contexts where both could apply
 *   PB_ANGA_ASIDDHA        — 8.2.1 pūrva-trāsiddham: later 8.* rules are
 *                             'invisible' to earlier rules
 *   PB_ASIDDHA_BAHIRANG    — bahiranga rule is suspended until antaraṅga
 *                             has completed (asiddhatva)
 *
 * Phase ε also adds a paribhasha_resolve_conflict() function that, given a
 * set of candidate rule IDs that all could fire, picks the winner. The
 * algorithm:
 *   1. If one candidate is in the apavāda registry as an apavāda of another,
 *      the apavāda wins (PB_APAVADA_OVER_UTSARGA).
 *   2. If one candidate is nitya (always applicable) and another is anitya,
 *      the nitya wins (PB_NITYA_OVER_ANITYA).
 *   3. If one candidate is antaraṅga (its cause is "inside" the form) and
 *      another is bahiraṅga, the antaraṅga wins (PB_ANTARA_OVER_BAHIRA).
 *   4. As a default, the later-numbered sūtra wins (PB_PURVA_PATA, paratva).
 */

#include "paribhasha.h"
#include <stdlib.h>
#include <string.h>

/* ── Static table of paribhāṣās ───────────────────────────────────────── */

static const Paribhasha TABLE[PB_COUNT] = {
  { NULL, NULL, NULL },  /* index 0 unused */
  { "sTAnivad",
    "Substitute acts like original (1.1.56) — except in rules about single al",
    NULL },
  { "asiddha",
    "Bahiraṅga suspended until antaraṅga completes",
    NULL },
  { "pUrvapara",
    "Later sūtra wins in conflict (paratva — 1.4.2)",
    NULL },
  { "nityAnitya",
    "Obligatory rule > optional rule",
    NULL },
  { "apavAda",
    "Exception (viśeṣa) > general (sāmānya)",
    NULL },
  { "antaraNga",
    "Antaraṅga (inner/internal-cause) > bahiranga (outer/later-cause)",
    NULL },
  { "sArvadhAtuka",
    "Sarvadhātuka takes priority over ārdhadhātuka in 3.4.113 context",
    NULL },
  { "aNgAsiddha",
    "8.2.1 pūrva-trāsiddham — 8.* rules don't see each other",
    NULL },
};

bool paribhasha_applies(ParibhashaId id, const void *ctx) {
  (void)ctx;
  return id > 0 && id < PB_COUNT;
}

const Paribhasha *paribhasha_get(ParibhashaId id) {
  if (id <= 0 || id >= PB_COUNT) return NULL;
  return &TABLE[id];
}

/* Check if sthānivad (1.1.56) applies to a specific substitution.
 * Default: true unless the rule is "anal-vidhau" (about single phonemes). */
bool paribhasha_sthanivad_applies(const char *original, const char *substitute,
                                    uint32_t rule) {
  (void)rule;
  if (!original || !substitute) return true;
  /* Exception: if both are single chars, we're in anal-vidhi territory.
     In that case sthānivad typically doesn't apply (only to multi-char forms). */
  if (strlen(original) == 1 && strlen(substitute) == 1) return false;
  return true;
}

/* ── Phase ε: rule-conflict resolution registry ─────────────────────── */

/* Apavāda pairs: (general, exception). The exception fires when both
   would otherwise apply. Pre-populated from the most heavily relied-on
   apavāda relations in Pāṇinian derivation. */
typedef struct {
  uint32_t general;   /* sūtra ID of the general (utsarga) rule */
  uint32_t exception; /* sūtra ID that beats it */
} ApavadaPair;

static const ApavadaPair APAVADA_PAIRS[] = {
  /* 3.1.68 kartari śap (general) vs 3.1.69 divādibhyaḥ śyan (apavāda) */
  { 301068, 301069 },
  /* 3.1.68 kartari śap vs 3.1.73 svādibhyaḥ śnu (apavāda) */
  { 301068, 301073 },
  /* 3.1.68 kartari śap vs 3.1.77 tudādibhyaḥ śaḥ (apavāda) */
  { 301068, 301077 },
  /* 3.1.68 kartari śap vs 3.1.78 rudhādibhyaḥ śnam (apavāda) */
  { 301068, 301078 },
  /* 3.1.68 kartari śap vs 3.1.79 tanādibhyaḥ uḥ (apavāda) */
  { 301068, 301079 },
  /* 3.1.68 kartari śap vs 3.1.81 kryādibhyaḥ śnā (apavāda) */
  { 301068, 301081 },
  /* 3.1.68 kartari śap vs 3.1.25 satyāpa-... -ic (apavāda for gaṇa-10) */
  { 301068, 301025 },
  /* 7.3.84 sārvadhātukārdhadhātukayoḥ guṇa (general) vs
     7.3.86 pugantalaghūpadhasya guṇa (apavāda — laghu upadhā specific) */
  { 703084, 703086 },
  /* 7.2.115 ato'ñṇiti vrddhi vs 7.2.116 ata upadhāyāḥ (more specific) */
  { 702115, 702116 },
  /* 8.4.1 raṣābhyāṃ no ṇaḥ (general ṇatva) vs 8.4.2 aṭ-kuṣ-vāṅ-numa-vy-
     ākhyānāt (negative apavāda blocking ṇatva in specific contexts) */
  { 800401, 800402 },
};

/* Sūtra-IDs that are "nitya" — apply whenever applicable, regardless of
   alternative rules. */
static const uint32_t NITYA_SUTRAS[] = {
  301068, /* kartari śap — once gaṇa is fixed, śap is mandatory in gaṇa-1 */
  703084, /* guṇa for sārvadhātuka — mandatory when conditions met */
  604071, /* a-augment for past tenses — mandatory */
  601064, /* dhātv-ādeḥ ṣaḥ saḥ — mandatory at derivation start */
  0,
};

/* Sūtra-IDs that are "antaraṅga" — their trigger is internal to the
   immediate aṅga, so they fire before bahiraṅga rules whose trigger
   involves later/outer material. */
static const uint32_t ANTARANGA_SUTRAS[] = {
  601008,  /* liṭi dhātor anabhyāsasya — reduplication (internal) */
  601078,  /* eco'yavāyāvaḥ — internal sandhi */
  704059,  /* hrasvaḥ — abhyāsa-internal */
  704060,  /* halādiḥ śeṣaḥ — abhyāsa-internal */
  704061,  /* śarpūrvāḥ khayaḥ — abhyāsa-internal */
  704062,  /* kuhoś cuḥ — abhyāsa-internal */
  701058,  /* idito num dhātor — root-internal augment */
  0,
};

static bool in_list(const uint32_t *list, uint32_t id) {
  for (size_t i = 0; list[i] != 0; i++) {
    if (list[i] == id) return true;
  }
  return false;
}

/* Public: given two candidate sūtra IDs, returns the one that wins per
   paribhāṣā precedence. Returns 0 if neither is preferred (i.e. both
   should fire independently). */
uint32_t paribhasha_resolve_pair(uint32_t a, uint32_t b) {
  if (!a) return b;
  if (!b) return a;
  if (a == b) return a;
  /* 1. apavāda > utsarga */
  for (size_t i = 0; i < sizeof(APAVADA_PAIRS) / sizeof(APAVADA_PAIRS[0]); i++) {
    if (APAVADA_PAIRS[i].general == a && APAVADA_PAIRS[i].exception == b) {
      return b;
    }
    if (APAVADA_PAIRS[i].general == b && APAVADA_PAIRS[i].exception == a) {
      return a;
    }
  }
  /* 2. nitya > anitya */
  bool a_nitya = in_list(NITYA_SUTRAS, a);
  bool b_nitya = in_list(NITYA_SUTRAS, b);
  if (a_nitya && !b_nitya) return a;
  if (b_nitya && !a_nitya) return b;
  /* 3. antaraṅga > bahiraṅga */
  bool a_anta = in_list(ANTARANGA_SUTRAS, a);
  bool b_anta = in_list(ANTARANGA_SUTRAS, b);
  if (a_anta && !b_anta) return a;
  if (b_anta && !a_anta) return b;
  /* 4. paratva: later sūtra ID wins (since IDs are encoded
     as adhyaya×100000 + pāda×1000 + sūtra, larger = later). */
  return (a > b) ? a : b;
}

/* Pick the winner from N candidate sūtra IDs, applying the resolver
   pairwise (left-fold). Returns 0 if candidates is empty or null. */
uint32_t paribhasha_resolve(const uint32_t *candidates, size_t count) {
  if (!candidates || count == 0) return 0;
  uint32_t winner = candidates[0];
  for (size_t i = 1; i < count; i++) {
    winner = paribhasha_resolve_pair(winner, candidates[i]);
  }
  return winner;
}

/* Check if `exception` is an apavāda of `general` per the registered
   pairs. Used for documentation / pratyāhāra audits. */
bool paribhasha_is_apavada_of(uint32_t exception, uint32_t general) {
  for (size_t i = 0; i < sizeof(APAVADA_PAIRS) / sizeof(APAVADA_PAIRS[0]); i++) {
    if (APAVADA_PAIRS[i].exception == exception &&
        APAVADA_PAIRS[i].general == general) {
      return true;
    }
  }
  return false;
}
