/* conflict.h — rule priority; Story 3.2 */
#ifndef CONFLICT_H
#define CONFLICT_H
#include "sutra.h"
#include <stdint.h>
typedef struct {
  uint32_t sutra_id;
  bool     is_apavada;
  bool     is_nitya;
  bool     is_antaranga;
  bool     is_optional;
} ConflictCandidate;
int  conflict_resolve(const SutraDB *db, const ConflictCandidate *cands, int count, const void *ctx);
ConflictCandidate conflict_candidate_build(const SutraDB *db, uint32_t sutra_id);
bool conflict_is_apavada_of(const SutraDB *db, uint32_t a_id, uint32_t b_id);
#endif
