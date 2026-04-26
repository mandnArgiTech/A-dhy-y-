/* samasa.h — samAsa derivation helpers (Story 5.2) */
#ifndef SAMASA_H
#define SAMASA_H

#include "ashtadhyayi.h"
#include "sutra.h"

/* Derives a compound stem from two members and samasa type. */
ASH_Form samasa_derive(const SutraDB *db,
                       const char *purva_slp1, const char *uttara_slp1,
                       ASH_SamasaType type, ASH_Linga linga,
                       void *ctx_out_unused);

/* Returns the canonical SLP1 label for a samasa type. */
const char *samasa_type_name(ASH_SamasaType type);

#endif
