/* pratyahara.h — pratyāhāra engine; implemented in Story 1.2 */
#ifndef PRATYAHARA_H
#define PRATYAHARA_H
#include "varna.h"
#define PRATYAHARA_MAX_MEMBERS 52
typedef struct {
  char label[8];
  char members[PRATYAHARA_MAX_MEMBERS];
  int  count;
} Pratyahara;
const Pratyahara *pratyahara_get(const char *label_slp1);
bool              pratyahara_contains(const char *label_slp1, char c);
void              pratyahara_print(const Pratyahara *p);
const Pratyahara *pratyahara_all(int *count_out);
#endif
