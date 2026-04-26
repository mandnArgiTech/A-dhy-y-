/* vikaranas.c — basic gaNa-based vikarana helpers; Story 3.6 */
#include "vikaranas.h"
#include <string.h>

bool vikarana_for_gana(int gana, char *out, size_t out_len) {
  const char *v;
  if (!out || out_len == 0) return false;
  switch (gana) {
    case 1:  v = "a";   break; /* Sap */
    case 4:  v = "ya";  break; /* yaN */
    case 6:  v = "a";   break; /* Sap-like thematic a */
    case 10: v = "aya"; break; /* cay/ay */
    default: v = "a";   break;
  }
  if (strlen(v) + 1 > out_len) return false;
  strncpy(out, v, out_len - 1);
  out[out_len - 1] = '\0';
  return true;
}

bool gana_uses_guna(int gana) {
  return gana == 1 || gana == 10;
}

bool gana_uses_vrddhi(int gana) {
  return gana == 10;
}
