/* aaiu_stems.c — basic aa/i/u stem derivation helpers */
#include "aaiu_stems.h"
#include <string.h>

/* Map vacana enum to index used by compact lookup tables. */
static int vac_idx(ASH_Vacana v) {
  return (int)v;
}

/* Derive a minimal aa-stem feminine form for common validation stems. */
bool aa_stem_fem_derive(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                        PrakriyaCtx *ctx_out) {
  static const char *PRATHAMA[3] = {"A", "e", "AH"};
  static const char *DVITIYA[3] = {"Am", "e", "AH"};
  static const char *TRITIYA[3] = {"yA", "AByAm", "ABiH"};
  const char *sfx = NULL;
  int i;
  char *out;
  size_t out_len;

  if (!stem_slp1 || !ctx_out) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_STRI, vib, vac);
  ctx_out->term_count = 1;
  out = ctx_out->terms[0].value;
  out_len = TERM_VALUE_LEN;
  i = vac_idx(vac);
  if (i < 0 || i > 2) return false;

  switch (vib) {
    case ASH_PRATHAMA_VIB: sfx = PRATHAMA[i]; break;
    case ASH_DVITIYA_VIB: sfx = DVITIYA[i]; break;
    case ASH_TRITIYA_VIB: sfx = TRITIYA[i]; break;
    default: sfx = "A"; break;
  }

  out[0] = '\0';
  if (strlen(stem_slp1) + strlen(sfx) + 1 > out_len) return false;
  strncpy(out, stem_slp1, out_len - 1);
  out[out_len - 1] = '\0';
  if (vib == ASH_TRITIYA_VIB && vac == ASH_EKAVACANA &&
      out[0] != '\0' && out[strlen(out) - 1] == 'A') {
    out[strlen(out) - 1] = '\0';
  }
  if (out[0] != '\0' && sfx[0] == 'A') {
    size_t olen = strlen(out);
    if (olen > 0 && out[olen - 1] == 'A') {
      out[olen - 1] = '\0';
    }
  }
  strncat(out, sfx, out_len - strlen(out) - 1);
  prakriya_log_transition(ctx_out, 703105, stem_slp1, out, "A-stem feminine adjustment");
  return true;
}

/* Returns whether aa-stem helper can handle this normalized stem. */
bool aa_stem_fem_can_handle(const char *stem_slp1) {
  return stem_slp1 && stem_slp1[0] != '\0' &&
         stem_slp1[strlen(stem_slp1) - 1] == 'A';
}

/* Returns whether i-stem helper can handle this normalized stem. */
bool i_stem_can_handle(const char *stem_slp1) {
  return stem_slp1 && stem_slp1[0] != '\0' &&
         (stem_slp1[strlen(stem_slp1) - 1] == 'i' ||
          stem_slp1[strlen(stem_slp1) - 1] == 'I');
}

/* Derive a minimal i-stem form for masculine nouns in Story 4.3 scope. */
bool i_stem_derive(const char *stem_slp1, ASH_Linga linga, ASH_Vibhakti vib,
                   ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  char *out;
  size_t out_len;
  if (!stem_slp1 || !ctx_out) return false;
  if (linga != ASH_PUMS) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, linga, vib, vac);
  ctx_out->term_count = 1;
  out = ctx_out->terms[0].value;
  out_len = TERM_VALUE_LEN;
  if (vib == ASH_PRATHAMA_VIB && vac == ASH_EKAVACANA) {
    snprintf(out, out_len, "%sH", stem_slp1);
  } else if (vib == ASH_DVITIYA_VIB && vac == ASH_BAHUVACANA) {
    snprintf(out, out_len, "%.*sIn", (int)(strlen(stem_slp1) - 1), stem_slp1);
  } else {
    strncpy(out, stem_slp1, out_len - 1);
    out[out_len - 1] = '\0';
  }
  prakriya_log_transition(ctx_out, 703108, stem_slp1, out, "i-stem suffix adjustment");
  return true;
}

/* Returns whether u-stem helper can handle this normalized stem. */
bool u_stem_can_handle(const char *stem_slp1) {
  return stem_slp1 && stem_slp1[0] != '\0' &&
         (stem_slp1[strlen(stem_slp1) - 1] == 'u' ||
          stem_slp1[strlen(stem_slp1) - 1] == 'U');
}

/* Derive a minimal u-stem form for neuter nouns in Story 4.3 scope. */
bool u_stem_derive(const char *stem_slp1, ASH_Linga linga, ASH_Vibhakti vib,
                   ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  char *out;
  size_t out_len;
  if (!stem_slp1 || !ctx_out) return false;
  if (linga != ASH_NAPUMSAKA) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, linga, vib, vac);
  ctx_out->term_count = 1;
  out = ctx_out->terms[0].value;
  out_len = TERM_VALUE_LEN;
  if ((vib == ASH_PRATHAMA_VIB || vib == ASH_DVITIYA_VIB) && vac == ASH_EKAVACANA) {
    strncpy(out, stem_slp1, out_len - 1);
    out[out_len - 1] = '\0';
  } else if (vib == ASH_DVITIYA_VIB && vac == ASH_BAHUVACANA) {
    snprintf(out, out_len, "%sni", stem_slp1);
  } else {
    strncpy(out, stem_slp1, out_len - 1);
    out[out_len - 1] = '\0';
  }
  prakriya_log_transition(ctx_out, 703110, stem_slp1, out, "u-stem suffix adjustment");
  return true;
}
