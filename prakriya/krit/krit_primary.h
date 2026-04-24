/* krit_primary.h — primary krt derivation helpers; Story 5.3 */
#ifndef KRIT_PRIMARY_H
#define KRIT_PRIMARY_H

#include "ashtadhyayi.h"
#include "sutra.h"

ASH_Form krit_derive(const SutraDB *db, const char *root_slp1, int gana,
                     ASH_KritType krit);

#endif
