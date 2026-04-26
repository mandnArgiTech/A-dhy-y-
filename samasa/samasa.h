/* samasa.h — compound formation API; Story 5.2 */
#ifndef SAMASA_H
#define SAMASA_H

#include "ashtadhyayi.h"
#include "sutra.h"

ASH_Form samasa_derive(const SutraDB *db,
                       const char *purva_slp1, const char *uttara_slp1,
                       ASH_SamasaType type, ASH_Linga linga);

#endif
