/* pronouns.h — sarvanāma paradigms. Story 5.x. */
#ifndef ASHTADHYAYI_PRAKRIYA_SUBANTA_PRONOUNS_H
#define ASHTADHYAYI_PRAKRIYA_SUBANTA_PRONOUNS_H

#include "../context.h"
#include "ashtadhyayi.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns true if the given upadeśa is one of the sarvanāma stems
   currently supported (tad, yad, etad, kim, sarva). */
bool pronoun_is_sarvanama(const char *upadesa);

/* Fill ctx_out with the PUMS form of the pronoun at the given slot.
   Returns false on unsupported stem or out-of-range slot. */
bool sarvanama_masc_full(const char *stem_slp1, ASH_Vibhakti vib,
                         ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* Same as above, for NAPUMSAKA. */
bool sarvanama_neut_full(const char *stem_slp1, ASH_Vibhakti vib,
                         ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* idam (this) — irregular demonstrative, all three liṅgas. */
bool pronoun_is_idam(const char *upadesa);
bool idam_full(const char *stem_slp1, ASH_Linga li,
               ASH_Vibhakti vib, ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* adas (that, distal) — irregular demonstrative, all three liṅgas. */
bool pronoun_is_adas(const char *upadesa);
bool adas_full(const char *stem_slp1, ASH_Linga li,
               ASH_Vibhakti vib, ASH_Vacana vac, PrakriyaCtx *ctx_out);

/* asmad (1st person) and yuzmad (2nd person) — liṅga-invariant. */
bool pronoun_is_personal(const char *upadesa);
bool asmad_yuzmad_full(const char *stem_slp1, ASH_Vibhakti vib,
                       ASH_Vacana vac, PrakriyaCtx *ctx_out);

#ifdef __cplusplus
}
#endif

#endif
