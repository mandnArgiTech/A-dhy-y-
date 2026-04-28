/* a_stem.h — a-stem subanta helpers (Story 4.2) */
#ifndef A_STEM_H
#define A_STEM_H

#include "ashtadhyayi.h"
#include "context.h"

/* Returns true when the stem belongs to the supported masculine a-stem class. */
bool a_stem_masc_can_handle(const char *stem_slp1);

/* Derive one masculine a-stem form into a full derivation context. */
bool a_stem_masc_derive(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* Returns true when the stem belongs to the supported neuter a-stem class. */
bool a_stem_neuter_can_handle(const char *stem_slp1);

/* Derive one neuter a-stem form into a full derivation context. */
bool a_stem_neuter_derive(const char *stem_slp1, ASH_Vibhakti vib,
                          ASH_Vacana vac, PrakriyaCtx *ctx_out);

#endif
