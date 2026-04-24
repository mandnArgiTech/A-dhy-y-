/* samjna.c — stub; Story 2.2 */
#include "samjna.h"
#include <stdio.h>
static char _buf[256];
const char *samjna_describe(Samjna s) {
  _buf[0]='\0';
  if(samjna_has(s,SJ_DHATU))       { strncat(_buf,"DHATU|",   sizeof(_buf)-strlen(_buf)-1); }
  if(samjna_has(s,SJ_PRATYAYA))    { strncat(_buf,"PRATYAYA|",sizeof(_buf)-strlen(_buf)-1); }
  if(samjna_has(s,SJ_ANGA))        { strncat(_buf,"ANGA|",    sizeof(_buf)-strlen(_buf)-1); }
  if(samjna_has(s,SJ_TING))        { strncat(_buf,"TING|",    sizeof(_buf)-strlen(_buf)-1); }
  if(samjna_has(s,SJ_SUP))         { strncat(_buf,"SUP|",     sizeof(_buf)-strlen(_buf)-1); }
  if(samjna_has(s,SJ_SARVADHATUKA)){ strncat(_buf,"SARV|",    sizeof(_buf)-strlen(_buf)-1); }
  if(_buf[0]=='\0') strncat(_buf,"NONE",sizeof(_buf)-1);
  return _buf;
}
Samjna samjna_of_sutra(uint32_t id) { (void)id; return SJ_NONE; }
