/* encoding.c — stub; implemented in Story 1.3 */
#include "encoding.h"
#include <stdlib.h>
#include <string.h>

const char *ash_version(void) { return "0.1.0"; }

char *ash_encode(const char *input, ASH_Encoding from, ASH_Encoding to) {
  (void)from; (void)to;
  if (!input) return NULL;
  return strdup(input); /* stub — identity transform */
}

char *encoding_convert(const char *input, ASH_Encoding from, ASH_Encoding to) {
  return ash_encode(input, from, to);
}
char *enc_slp1_to_iast(const char *s)      { return s ? strdup(s) : NULL; }
char *enc_slp1_to_devanagari(const char *s){ return s ? strdup(s) : NULL; }
char *enc_slp1_to_hk(const char *s)        { return s ? strdup(s) : NULL; }
char *enc_iast_to_slp1(const char *s)      { return s ? strdup(s) : NULL; }
char *enc_hk_to_slp1(const char *s)        { return s ? strdup(s) : NULL; }
char *enc_devanagari_to_slp1(const char *s){ return s ? strdup(s) : NULL; }
int encoding_convert_buf(const char *in, ASH_Encoding from, ASH_Encoding to,
                          char *out, size_t out_len) {
  (void)from; (void)to;
  if (!in || !out || out_len == 0) return -1;
  strncpy(out, in, out_len - 1); out[out_len-1] = '\0';
  return (int)strlen(out);
}
