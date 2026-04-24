/* sandhi_hal.c — stub; Story 1.5 */
#include "sandhi_hal.h"
#include <string.h>
SandhiResult sandhi_hal_apply(char a, char b) {
  SandhiResult r = {{0}, 0, false};
  r.result_slp1[0]=a; r.result_slp1[1]='\0';
  return r;
}
bool sandhi_hal_join(const char *a, const char *b, char *out, size_t len) {
  if (!a||!b||!out||len<2) return false;
  size_t la=strlen(a), lb=strlen(b);
  if (la+lb+1 < len) { memcpy(out,a,la); memcpy(out+la,b,lb); out[la+lb]='\0'; }
  return false;
}
