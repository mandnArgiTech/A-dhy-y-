/* aaiu_stems.h — aa/i/u stem derivation helpers */
#ifndef AAIU_STEMS_H
#define AAIU_STEMS_H

#include "ashtadhyayi.h"
#include "context.h"

/* Derive a single aa-stem feminine form (e.g. rAmA). */
bool aa_stem_fem_derive(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                        PrakriyaCtx *ctx_out);
bool aa_stem_fem_can_handle(const char *stem_slp1);

/* Derive a single i-stem form (kvi-style in this implementation). */
bool i_stem_derive(const char *stem_slp1, ASH_Linga linga, ASH_Vibhakti vib,
                   ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool i_stem_can_handle(const char *stem_slp1);
bool i_stem_masc_can_handle(const char *stem_slp1);

/* Derive a single u-stem form (mDu-style in this implementation). */
bool u_stem_derive(const char *stem_slp1, ASH_Linga linga, ASH_Vibhakti vib,
                   ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool u_stem_can_handle(const char *stem_slp1);
bool u_stem_neu_can_handle(const char *stem_slp1);

/* Story 4.6: full 24-slot paradigm helpers. The "_full" suffix
   distinguishes these from the legacy stubs above. Each helper writes
   the derived SLP1 form into ctx_out->terms[0].value and logs the
   primary sūtra IDs. */
bool i_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool i_stem_neut_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool u_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool u_stem_neut_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* Story 4.8: feminine vowel-stem paradigms. */
bool ii_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool uu_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool i_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                     ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool u_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                     ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool aa_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* Story 4.13: long-ī masculine (BallAtakI) and z-final feminine (arciz). */
bool ii_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib,
                       ASH_Vacana vac, PrakriyaCtx *ctx_out);
bool z_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                     ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* Story 4.15: kim feminine pronominal paradigm. */
bool kim_stri_full(const char *stem_slp1, ASH_Vibhakti vib,
                   ASH_Vacana vac, PrakriyaCtx *ctx_out);

#endif
