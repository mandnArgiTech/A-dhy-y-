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

#ifdef __cplusplus
}
#endif

#endif
