/* vikaranas.h — gaNa-specific vikaraNa helpers */
#ifndef VIKARANAS_H
#define VIKARANAS_H
#include "ashtadhyayi.h"
#include <stdbool.h>

bool vikarana_for_gana(int gana, char *out, size_t out_len);
bool gana_uses_guna(int gana);
bool gana_uses_vrddhi(int gana);
#endif
