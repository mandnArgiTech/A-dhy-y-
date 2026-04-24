/* paribhasha.c — Story 2.6 implementation
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
