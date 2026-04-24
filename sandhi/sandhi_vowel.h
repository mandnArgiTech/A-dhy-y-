#ifndef SANDHI_VOWEL_H
#define SANDHI_VOWEL_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
typedef struct { char result_slp1[32]; uint32_t rule_applied; bool changed; } SandhiResult;
SandhiResult sandhi_vowel_apply(char a_final, char b_initial);
bool sandhi_vowel_join(const char *a, const char *b, char *out, size_t len);
typedef struct { char a_end; char b_start; uint32_t rule; } SandhiSplit;
int sandhi_vowel_split(char junction, SandhiSplit *splits, int max_splits);
#endif
