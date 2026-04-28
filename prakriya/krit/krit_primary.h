/* krit_primary.h — primary krt derivation helpers (Story 5.3) */
#ifndef KRIT_PRIMARY_H
#define KRIT_PRIMARY_H

#include "ashtadhyayi.h"
#include "context.h"

/* Derive one primary krt form for a root. */
ASH_Form krit_derive(const char *root_slp1, int gana, ASH_KritType krit);

/* Derive one primary krt form with an upasarga prefix. */
ASH_Form krit_derive_with_prefix(const char *root_slp1, int gana,
                                 ASH_KritType krit, const char *prefix_slp1);

/* Return whether the suffix context needs iT augment handling. */
bool krit_needs_it_augment(const char *root_slp1, ASH_KritType krit);

#endif
