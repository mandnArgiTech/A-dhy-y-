/* conflict.c — stub; Story 3.2 */
#include "conflict.h"
int conflict_resolve(const SutraDB *db, const ConflictCandidate *c, int n, const void *ctx) {
  (void)db;(void)ctx;
  if(n<=0) return -1;
  /* Stub: apavada wins first, else last (paratva) */
  for(int i=0;i<n;i++) if(c[i].is_apavada) return i;
  return n-1;
}
ConflictCandidate conflict_candidate_build(const SutraDB *db, uint32_t id) {
  (void)db;
  ConflictCandidate c = {id, false, false, false, false};
  return c;
}
bool conflict_is_apavada_of(const SutraDB *db, uint32_t a, uint32_t b) {
  (void)db; return a > b; /* stub: later rule = apavada */
}
