/* term.c — term helpers for derivation context; Story 3.1 */
#include "term.h"
#include "anubandha.h"
#include "varna.h"
#include <string.h>
#include <stdio.h>
void term_init(Term *t, const char *u, Samjna ctx) {
  if(!t||!u) return;
  memset(t,0,sizeof(*t));
  t->samjna = ctx;
  strncpy(t->upadesa, u, TERM_UPADESA_LEN-1);
  AnubandhaResult ar;
  anubandha_strip(u, ctx, &ar);
  strncpy(t->value, ar.clean_slp1, TERM_VALUE_LEN-1);
  t->samjna = samjna_add(t->samjna, ar.it_samjna);
}
void term_substitute(Term *t, const char *v, uint32_t rule) {
  if(!t||!v) return;
  strncpy(t->value, v, TERM_VALUE_LEN-1);
  term_record_rule(t, rule);
}
void term_record_rule(Term *t, uint32_t rule) {
  if(!t||t->rule_count>=MAX_RULE_HISTORY) return;
  t->rule_history[t->rule_count++] = rule;
}
char term_final(const Term *t) {
  if(!t) return 0;
  size_t n = strlen(t->value);
  return n ? t->value[n-1] : 0;
}
char term_penultimate(const Term *t) {
  if(!t) return 0;
  size_t n = strlen(t->value);
  return n > 1 ? t->value[n-2] : 0;
}
bool term_ends_vowel(const Term *t)   { return t ? varna_is_vowel(term_final(t)) : false; }
bool term_begins_vowel(const Term *t) { return t ? varna_is_vowel(t->value[0]) : false; }
void term_print(const Term *t, FILE *f) {
  if(!t||!f) return;
  fprintf(f, "[%s/%s samjna=%llu]\n", t->value, t->upadesa,
          (unsigned long long)t->samjna);
}
