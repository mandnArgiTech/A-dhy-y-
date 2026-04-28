/* consonant_stems.c — selected consonant-final stem derivation helpers */
#include "consonant_stems.h"
#include <string.h>

/* Returns true if the stem belongs to the implemented n-final family. */
bool n_stem_can_handle(const char *stem_slp1) {
  return stem_slp1 && strcmp(stem_slp1, "rAjn") == 0;
}

/* Returns true if the stem belongs to the implemented as-final family. */
bool as_stem_can_handle(const char *stem_slp1) {
  return stem_slp1 && strcmp(stem_slp1, "mns") == 0;
}

/* Returns true if the stem belongs to the implemented R-final family. */
bool r_stem_can_handle(const char *stem_slp1) {
  return stem_slp1 && strcmp(stem_slp1, "pitf") == 0;
}

/* Derives one n-stem form using a small representative table. */
bool n_stem_derive(const char *stem_slp1, ASH_Linga lin,
                   ASH_Vibhakti vib, ASH_Vacana vac,
                   PrakriyaCtx *ctx_out) {
  char out_slp1[TERM_VALUE_LEN] = {0};
  size_t out_len = sizeof(out_slp1);
  (void)lin;
  if (!ctx_out || !n_stem_can_handle(stem_slp1)) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
  ctx_out->term_count = 1;
  if (vib == ASH_PRATHAMA_VIB && vac == ASH_EKAVACANA) {
    snprintf(out_slp1, out_len, "%.*sA", 3, stem_slp1);
  } else if (vib == ASH_DVITIYA_VIB && vac == ASH_BAHUVACANA) {
    snprintf(out_slp1, out_len, "%.*sYH", 3, stem_slp1);
  } else if (vib == ASH_TRITIYA_VIB && vac == ASH_EKAVACANA) {
    snprintf(out_slp1, out_len, "%.*sYA", 3, stem_slp1);
  } else {
    strncpy(out_slp1, stem_slp1, out_len - 1);
  }
  out_slp1[out_len - 1] = '\0';
  strncpy(ctx_out->terms[0].value, out_slp1, TERM_VALUE_LEN - 1);
  prakriya_log_transition(ctx_out, 802007, "nalopaH prAtipadikAntasya", stem_slp1, ctx_out->terms[0].value);
  return true;
}

/* Derives one as-stem form using a small representative table. */
bool as_stem_derive(const char *stem_slp1, ASH_Linga lin,
                    ASH_Vibhakti vib, ASH_Vacana vac,
                    PrakriyaCtx *ctx_out) {
  char out_slp1[TERM_VALUE_LEN] = {0};
  size_t out_len = sizeof(out_slp1);
  (void)lin;
  if (!ctx_out || !as_stem_can_handle(stem_slp1)) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_NAPUMSAKA, vib, vac);
  ctx_out->term_count = 1;
  if ((vib == ASH_PRATHAMA_VIB || vib == ASH_DVITIYA_VIB) && vac == ASH_EKAVACANA) {
    snprintf(out_slp1, out_len, "%.*sH", 2, stem_slp1);
  } else if (vib == ASH_TRITIYA_VIB && vac == ASH_EKAVACANA) {
    snprintf(out_slp1, out_len, "%sA", stem_slp1);
  } else {
    strncpy(out_slp1, stem_slp1, out_len - 1);
  }
  out_slp1[out_len - 1] = '\0';
  strncpy(ctx_out->terms[0].value, out_slp1, TERM_VALUE_LEN - 1);
  prakriya_log_transition(ctx_out, 803016, "aH parasminn asati virAmaH", stem_slp1, ctx_out->terms[0].value);
  return true;
}

/* Derives one R-stem form using a small representative table. */
bool r_stem_derive(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                   PrakriyaCtx *ctx_out) {
  char out_slp1[TERM_VALUE_LEN] = {0};
  size_t out_len = sizeof(out_slp1);
  if (!ctx_out || !r_stem_can_handle(stem_slp1)) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
  ctx_out->term_count = 1;
  if (vib == ASH_PRATHAMA_VIB && vac == ASH_EKAVACANA) {
    snprintf(out_slp1, out_len, "%.*sA", 3, stem_slp1);
  } else if (vib == ASH_SHASTHI_VIB && vac == ASH_EKAVACANA) {
    snprintf(out_slp1, out_len, "%.*suH", 3, stem_slp1);
  } else {
    strncpy(out_slp1, stem_slp1, out_len - 1);
  }
  out_slp1[out_len - 1] = '\0';
  strncpy(ctx_out->terms[0].value, out_slp1, TERM_VALUE_LEN - 1);
  prakriya_log_transition(ctx_out, 701072, "f-final stem special handling", stem_slp1, ctx_out->terms[0].value);
  return true;
}

/* Unified entry point used by tests and fallback dispatch code. */
bool consonant_stem_derive(const char *stem_slp1, ASH_Linga lin,
                           ASH_Vibhakti vib, ASH_Vacana vac,
                           char *out_slp1, size_t out_len) {
  PrakriyaCtx ctx = {0};
  bool ok = false;
  if (!out_slp1 || out_len == 0) return false;
  if (n_stem_can_handle(stem_slp1)) {
    ok = n_stem_derive(stem_slp1, lin, vib, vac, &ctx);
  } else if (as_stem_can_handle(stem_slp1)) {
    ok = as_stem_derive(stem_slp1, lin, vib, vac, &ctx);
  } else if (r_stem_can_handle(stem_slp1)) {
    ok = r_stem_derive(stem_slp1, vib, vac, &ctx);
  }
  if (!ok) return false;
  strncpy(out_slp1, ctx.terms[0].value, out_len - 1);
  out_slp1[out_len - 1] = '\0';
  return true;
}
