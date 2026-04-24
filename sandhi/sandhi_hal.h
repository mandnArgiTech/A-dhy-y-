#ifndef SANDHI_HAL_H
#define SANDHI_HAL_H
#include "sandhi_vowel.h"
SandhiResult sandhi_hal_apply(char a_final, char b_initial);
bool sandhi_hal_join(const char *a, const char *b, char *out, size_t len);
#endif
