/* guna_vrddhi.h — guNa/vfddhi operations for derivation terms */
#ifndef GUNA_VRDDHI_H
#define GUNA_VRDDHI_H

#include "term.h"
#include <stdbool.h>
#include <stdint.h>

/* Applies guNa to a term's final vowel when not blocked by kit/Nit markers. */
bool guna_apply_to_final(Term *anga, const Term *pratyaya, uint32_t *rule_fired);

/* Applies guNa to a term's penultimate (upadhA) vowel when not blocked. */
bool guna_apply_to_upadha(Term *anga, const Term *pratyaya, uint32_t *rule_fired);

/* Applies vfddhi to a term's final vowel when not blocked by kit/Nit markers. */
bool vrddhi_apply_to_final(Term *anga, const Term *pratyaya, uint32_t *rule_fired);

/* Returns true when guNa/vfddhi should be blocked by suffix markers. */
bool guna_is_blocked(const Term *pratyaya);

#endif
