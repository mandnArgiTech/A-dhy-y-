/* paribhasha.h — paribhAShA metadata and predicates; Story 2.6 + Phase ε */
#ifndef PARIBHASHA_H
#define PARIBHASHA_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  PB_STHANIVAD=1, PB_ASIDDHA_BAHIRANG, PB_PURVA_PATA, PB_NITYA_OVER_ANITYA,
  PB_APAVADA_OVER_UTSARGA, PB_ANTARA_OVER_BAHIRA, PB_SARVADHATUKE_ARDHA,
  PB_ANGA_ASIDDHA, PB_COUNT
} ParibhashaId;

typedef struct {
  const char *label_slp1;
  const char *description_en;
  bool (*applies)(ParibhashaId pb, const void *ctx);
} Paribhasha;

bool paribhasha_applies(ParibhashaId id, const void *ctx);
const Paribhasha *paribhasha_get(ParibhashaId id);
bool paribhasha_sthanivad_applies(const char *orig, const char *sub, uint32_t rule);

/* Phase ε rule-conflict resolution. */

/* Given two candidate sūtra IDs that both could fire, returns the one
 * that wins per paribhāṣā precedence (apavāda > nitya > antaraṅga >
 * paratva). */
uint32_t paribhasha_resolve_pair(uint32_t a, uint32_t b);

/* Pick the winner from N candidate sūtra IDs by left-folding pair
 * resolution. Returns 0 if `candidates` is null or `count` is 0. */
uint32_t paribhasha_resolve(const uint32_t *candidates, size_t count);

/* Check if `exception` is registered as an apavāda of `general`. */
bool paribhasha_is_apavada_of(uint32_t exception, uint32_t general);

#endif
