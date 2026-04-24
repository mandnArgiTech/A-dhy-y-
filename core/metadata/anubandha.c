/* anubandha.c — stub; Story 2.3 */
#include "anubandha.h"
#include <string.h>
void anubandha_strip(const char *u, Samjna ctx, AnubandhaResult *r) {
  (void)ctx;
  if (!u||!r) return;
  memset(r,0,sizeof(*r));
  strncpy(r->clean_slp1, u, sizeof(r->clean_slp1)-1); /* stub: no stripping */
}
bool anubandha_has_it(const char *u, char c, Samjna ctx) {
  (void)ctx;
  if (!u) return false;
  while (*u) if (*u++ == c) return true;
  return false;
}
Samjna it_to_samjna(char c) {
  switch(c) {
    case 'k': return SJ_KIT;
    case 'N': return SJ_NGIT;
    case 'R': return SJ_NNIT;
    case 'p': return SJ_PIT;
    case 'S': return SJ_SARVADHATUKA;
    default:  return SJ_NONE;
  }
}
