/* pratyahara.c — stub; implemented in Story 1.2 */
#include "pratyahara.h"
#include <string.h>
/* Minimal table sufficient for sandhi stubs to compile */
static const Pratyahara TABLE[] = {
  {"ac",  "aiufeEoOxX", 10},
  {"hal", "kKgGNcCjJYwWqQRtTdDnpPbBmyrlvSzsh", 34},
  {"ik",  "iufx", 4},
  {"ec",  "eEoO", 4},
  {"jaS", "jbgqd", 5},
  {"kar", "kKgGcCjJwWqQtTdDpPbB", 20},
  {"", "", 0}
};
static int TABLE_COUNT = 6;
const Pratyahara *pratyahara_get(const char *label) {
  if (!label) return NULL;
  for (int i = 0; TABLE[i].count > 0; i++)
    if (strcmp(TABLE[i].label, label) == 0) return &TABLE[i];
  return NULL;
}
bool pratyahara_contains(const char *label, char c) {
  const Pratyahara *p = pratyahara_get(label);
  if (!p) return false;
  for (int i = 0; i < p->count; i++) if (p->members[i] == c) return true;
  return false;
}
void pratyahara_print(const Pratyahara *p) { (void)p; }
const Pratyahara *pratyahara_all(int *n) { if(n) *n=TABLE_COUNT; return TABLE; }
