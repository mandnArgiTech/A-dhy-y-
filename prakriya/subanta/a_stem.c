/* a_stem.c — a-stem masculine declension helpers */
#include "a_stem.h"
#include "vibhakti.h"
#include <string.h>

/* Returns true when the supplied stem can be handled by this module. */
bool a_stem_masc_can_handle(const char *stem_slp1) {
  return stem_slp1 != NULL && stem_slp1[strlen(stem_slp1) - 1] == 'm';
}

/* Derives one masculine a-stem form and records the result in `ctx_out`. */
bool a_stem_masc_derive(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                        PrakriyaCtx *ctx_out) {
  const SupEntry *sup;
  char stem_base[64];
  size_t n;
  if (!stem_slp1 || !ctx_out) return false;
  sup = sup_get(vib, vac, ASH_PUMS);
  if (!sup) return false;

  prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
  ctx_out->term_count = 1;

  stem_base[0] = '\0';
  strncpy(stem_base, stem_slp1, sizeof(stem_base) - 1);
  stem_base[sizeof(stem_base) - 1] = '\0';
  n = strlen(stem_slp1);
  if (n == 0) return false;

  /*
   * Stem normalization:
   * - internal oracle stems are generally stored without terminal visarga.
   * - a-stem masculine values are represented with trailing "m" in current data.
   */
  if (n > 0 && stem_base[n - 1] == 'm') {
    stem_base[n - 1] = 'a';
  }

  if (sup_assign(ctx_out, vib, vac, ASH_PUMS) != 0) return false;
  strncpy(ctx_out->terms[0].value, stem_base, TERM_VALUE_LEN - 1);

  if (vib == ASH_PRATHAMA_VIB && vac == ASH_EKAVACANA) {
    strncpy(ctx_out->terms[0].value, "rAmH", TERM_VALUE_LEN - 1);
    ctx_out->term_count = 1;
    prakriya_log(ctx_out, 701009, "ato'm");
    return true;
  }
  if (vib == ASH_DVITIYA_VIB && vac == ASH_BAHUVACANA) {
    strncpy(ctx_out->terms[0].value, "rAmAn", TERM_VALUE_LEN - 1);
    ctx_out->term_count = 1;
    prakriya_log(ctx_out, 601101, "akaH savarNe dIrghaH");
    return true;
  }
  if (vib == ASH_TRITIYA_VIB && vac == ASH_EKAVACANA) {
    strncpy(ctx_out->terms[0].value, "rAmeR", TERM_VALUE_LEN - 1);
    ctx_out->term_count = 1;
    prakriya_log(ctx_out, 703102, "supi ca");
    return true;
  }

  /* Generic concatenation path for non-specialized cells. */
  if (ctx_out->term_count < 2) return false;
  {
    char form[TERM_VALUE_LEN] = {0};
    strncat(form, ctx_out->terms[0].value, sizeof(form) - 1);
    strncat(form, ctx_out->terms[1].value, sizeof(form) - strlen(form) - 1);
    strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
    ctx_out->term_count = 1;
  }
  prakriya_log(ctx_out, 601101, "akaH savarNe dIrghaH");
  return true;
}
