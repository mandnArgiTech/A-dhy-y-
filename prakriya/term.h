/* term.h — derivation term; Story 3.1 */
#ifndef TERM_H
#define TERM_H
#include "samjna.h"
#include "ashtadhyayi.h"
#include <stdio.h>
#define TERM_VALUE_LEN    128
#define TERM_UPADESA_LEN  128
#define MAX_RULE_HISTORY   64
typedef struct {
  char     value[TERM_VALUE_LEN];
  char     upadesa[TERM_UPADESA_LEN];
  Samjna   samjna;
  uint64_t it_flags;
  uint32_t rule_history[MAX_RULE_HISTORY];
  uint8_t  rule_count;
  uint8_t  index;
  bool     is_final;
} Term;
void term_init(Term *t, const char *upadesa_slp1, Samjna context);
void term_substitute(Term *t, const char *new_value, uint32_t rule_id);
void term_record_rule(Term *t, uint32_t rule_id);
static inline bool term_has(const Term *t, Samjna f) { return samjna_has(t->samjna, f); }
static inline char term_initial(const Term *t) { return t->value[0]; }
char term_final(const Term *t);
char term_penultimate(const Term *t);
bool term_ends_vowel(const Term *t);
bool term_begins_vowel(const Term *t);
void term_print(const Term *t, FILE *stream);
#endif
