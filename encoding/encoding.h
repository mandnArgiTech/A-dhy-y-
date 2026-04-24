/* encoding.h — SLP1 ↔ IAST ↔ Devanāgarī ↔ HK codec
 * Implemented in Story 1.3 */
#ifndef ENCODING_H
#define ENCODING_H
#include "ashtadhyayi.h"
#include <stddef.h>
char *encoding_convert(const char *input, ASH_Encoding from, ASH_Encoding to);
char *enc_slp1_to_iast(const char *slp1);
char *enc_slp1_to_devanagari(const char *slp1);
char *enc_slp1_to_hk(const char *slp1);
char *enc_iast_to_slp1(const char *iast);
char *enc_hk_to_slp1(const char *hk);
char *enc_devanagari_to_slp1(const char *deva);
int   encoding_convert_buf(const char *in, ASH_Encoding from, ASH_Encoding to,
                            char *out, size_t out_len);
#endif
