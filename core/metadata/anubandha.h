/* anubandha.h — it-letter stripping; Story 2.3 */
#ifndef ANUBANDHA_H
#define ANUBANDHA_H
#include "samjna.h"
typedef struct {
  char   it_chars[16];
  int    it_count;
  char   clean_slp1[64];
  Samjna it_samjna;
} AnubandhaResult;
void anubandha_strip(const char *upadesa_slp1, Samjna context, AnubandhaResult *result);
bool anubandha_has_it(const char *upadesa_slp1, char it_char, Samjna context);
Samjna it_to_samjna(char it_char);
#endif
