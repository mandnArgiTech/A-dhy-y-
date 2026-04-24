/* sandhi_vowel.c — stub; Story 1.4 */
#include "sandhi_vowel.h"
#include <string.h>
SandhiResult sandhi_vowel_apply(char a, char b) {
  SandhiResult r = {{0}, 0, false};
  /* Minimal: savarnadīrgha a+a→A */
  if (a == 'a' && b == 'a') { r.result_slp1[0]='A'; r.result_slp1[1]='\0'; r.changed=true; r.rule_applied=1; }
  else { r.result_slp1[0]=a; r.result_slp1[1]=b; r.result_slp1[2]='\0'; }
  return r;
}
bool sandhi_vowel_join(const char *a, const char *b, char *out, size_t len) {
  if (!a||!b||!out||len<2) return false;
  size_t la=strlen(a), lb=strlen(b);
  SandhiResult r=sandhi_vowel_apply(la?a[la-1]:0, lb?b[0]:0);
  /* Simple concatenation stub */
  if (la+lb+1 < len) { memcpy(out,a,la); memcpy(out+la,b,lb); out[la+lb]='\0'; }
  return r.changed;
}
int sandhi_vowel_split(char j, SandhiSplit *s, int m) { (void)j;(void)s;(void)m; return 0; }
