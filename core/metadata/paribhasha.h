/* paribhasha.h — paribhAShA metadata and predicates; Story 2.6 */
#ifndef PARIBHASHA_H
#define PARIBHASHA_H
#include <stdbool.h>
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
#endif
