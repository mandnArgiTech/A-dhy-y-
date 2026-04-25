/* consonant_stems.h — selected consonant-final stem derivation helpers */
#ifndef CONSONANT_STEMS_H
#define CONSONANT_STEMS_H
#include "ashtadhyayi.h"
#include "context.h"

/**
 * Derive selected consonant-final subanta forms used by this implementation.
 *
 * Supported stems currently include:
 * - `rAjn` (rAjan-type), masculine
 * - `mns`  (manas-type), neuter
 *
 * @param stem_slp1 Stem in normalized SLP1.
 * @param lin Gender.
 * @param vib Case.
 * @param vac Number.
 * @param out_slp1 Caller-provided output buffer.
 * @param out_len Size of output buffer.
 * @return true when a form was derived for this stem.
 */
bool consonant_stem_derive(const char *stem_slp1, ASH_Linga lin,
                           ASH_Vibhakti vib, ASH_Vacana vac,
                           char *out_slp1, size_t out_len);

/* Stem-class probes for pipeline dispatch. */
bool n_stem_can_handle(const char *stem_slp1);
bool as_stem_can_handle(const char *stem_slp1);

/* Concrete stem derivation functions used by tests and pipeline dispatch. */
bool n_stem_derive(const char *stem_slp1, ASH_Linga lin,
                   ASH_Vibhakti vib, ASH_Vacana vac,
                   PrakriyaCtx *ctx_out);
bool as_stem_derive(const char *stem_slp1, ASH_Linga lin,
                    ASH_Vibhakti vib, ASH_Vacana vac,
                    PrakriyaCtx *ctx_out);
bool r_stem_can_handle(const char *stem_slp1);
bool r_stem_derive(const char *stem_slp1, ASH_Vibhakti vib,
                   ASH_Vacana vac, PrakriyaCtx *ctx_out);

#endif
