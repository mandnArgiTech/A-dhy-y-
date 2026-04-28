/* guna_vrddhi.c — guNa/vfddhi helpers for derivational steps */
#include "guna_vrddhi.h"
#include "varna.h"
#include <string.h>

/* Returns true when the pratyaya blocks guNa/vfddhi substitution. */
bool guna_is_blocked(const Term *pratyaya) {
  if (!pratyaya) return false;
  return term_has(pratyaya, SJ_KIT) || term_has(pratyaya, SJ_NGIT);
}

/* Applies guNa to the final vowel of anga when permitted by context. */
bool guna_apply_to_final(Term *anga, const Term *pratyaya, uint32_t *rule_fired) {
  size_t n;
  char v;
  if (!anga) return false;
  if (guna_is_blocked(pratyaya)) {
    if (rule_fired) *rule_fired = 100105; /* 1.1.5 kGiti ca */
    return false;
  }
  n = strlen(anga->value);
  if (n == 0) return false;
  v = anga->value[n - 1];
  if (!varna_is_vowel(v)) return false;
  anga->value[n - 1] = varna_guna(v);
  if (rule_fired) *rule_fired = 703084; /* sArvadhAtukArdhadhAtukayoH */
  return anga->value[n - 1] != v;
}

/* Applies guNa to the upadha vowel when present and unblocked. */
bool guna_apply_to_upadha(Term *anga, const Term *pratyaya, uint32_t *rule_fired) {
  size_t n;
  char v;
  if (!anga) return false;
  if (guna_is_blocked(pratyaya)) {
    if (rule_fired) *rule_fired = 100105; /* 1.1.5 kGiti ca */
    return false;
  }
  n = strlen(anga->value);
  if (n < 2) return false;
  v = anga->value[n - 2];
  if (!varna_is_vowel(v)) return false;
  anga->value[n - 2] = varna_guna(v);
  if (rule_fired) *rule_fired = 703086; /* pugantalaghu-upadha guNa */
  return anga->value[n - 2] != v;
}

/* Applies vfddhi to the final vowel when not blocked by pratyaya markers. */
bool vrddhi_apply_to_final(Term *anga, const Term *pratyaya, uint32_t *rule_fired) {
  size_t n;
  char v;
  if (!anga) return false;
  if (guna_is_blocked(pratyaya)) {
    if (rule_fired) *rule_fired = 100105; /* 1.1.5 kGiti ca */
    return false;
  }
  n = strlen(anga->value);
  if (n == 0) return false;
  v = anga->value[n - 1];
  if (!varna_is_vowel(v)) return false;
  anga->value[n - 1] = varna_vrddhi(v);
  if (rule_fired) *rule_fired = 702115; /* aco YRiti */
  return anga->value[n - 1] != v;
}
