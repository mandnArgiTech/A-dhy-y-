/* numerals.h — saṅkhyā paradigms. */
#ifndef ASHTADHYAYI_PRAKRIYA_SUBANTA_NUMERALS_H
#define ASHTADHYAYI_PRAKRIYA_SUBANTA_NUMERALS_H

#include "../context.h"
#include "ashtadhyayi.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Returns true if the upadeśa is one of the supported numerals
   (dvi, tri, catur, paYcan, zaz, saptan, azwan, navan, daSan). */
bool numeral_is_known(const char *upadesa);

/* Fill ctx_out with the numeral's form at the given slot. Returns
   false if stem unsupported, slot invalid, or (for catur STRI)
   if the caller should fall through to catur_stri_full. */
bool numeral_full(const char *stem, ASH_Linga li, ASH_Vibhakti vib,
                  ASH_Vacana vac, PrakriyaCtx *ctx_out);

#ifdef __cplusplus
}
#endif

#endif
