/* paribhasha.c — stub; Story 2.6 */
#include "paribhasha.h"
#include <stdlib.h>
static const Paribhasha TABLE[PB_COUNT] = {
  {NULL,NULL,NULL},
  {"sTAnIvad",  "Substitute = original (except al-rules)", NULL},
  {"asiddham",  "Bahiranga inoperative before antaranga",  NULL},
  {"paratva",   "Later rule wins",                         NULL},
  {"nitya",     "Obligatory > optional",                   NULL},
  {"apavAda",   "Exception > general",                     NULL},
  {"antaraNga", "Inner > outer",                           NULL},
  {"sArvaDAtuke","Sarvadhatuka > ardhadhatuka",             NULL},
  {"aNgAsiddha","Anga rules unseen for later anga rules",  NULL},
};
bool paribhasha_applies(ParibhashaId id, const void *ctx) { (void)id;(void)ctx; return true; }
const Paribhasha *paribhasha_get(ParibhashaId id) {
  if(id<=0||id>=PB_COUNT) return NULL;
  return &TABLE[id];
}
bool paribhasha_sthanivad_applies(const char *o, const char *s, uint32_t r) {
  (void)o;(void)s;(void)r; return true;
}
