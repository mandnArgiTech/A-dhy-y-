/* a_stem.c — a-stem masculine declension helpers */
#include "a_stem.h"
#include "vibhakti.h"
#include <string.h>

/* Returns true when the supplied stem can be handled by this module. */
bool a_stem_masc_can_handle(const char *stem_slp1) {
  if (!stem_slp1 || stem_slp1[0] == '\0') return false;
  return stem_slp1[strlen(stem_slp1) - 1] == 'a' ||
         stem_slp1[strlen(stem_slp1) - 1] == 'm';
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

  /* Older tests passed rāma as rAm; normalize that spelling to current rAma. */
  if (n > 0 && stem_base[n - 1] == 'm') {
    if (n + 1 >= sizeof(stem_base)) return false;
    stem_base[n] = 'a';
    stem_base[n + 1] = '\0';
  }

  strncpy(ctx_out->terms[0].value, stem_base, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  if (sup_assign(ctx_out, vib, vac, ASH_PUMS) != 0) return false;
  if (ctx_out->term_count < 2) return false;
  {
    char before[TERM_VALUE_LEN] = {0};
    char form[TERM_VALUE_LEN] = {0};
    const char *suffix = ctx_out->terms[1].value;
    prakriya_current_form(ctx_out, before, sizeof(before));
    strncat(form, ctx_out->terms[0].value, sizeof(form) - 1);
    if (form[0] != '\0' && form[strlen(form) - 1] == 'a') {
      if (strcmp(suffix, "H") == 0 || strcmp(suffix, "m") == 0 ||
          strcmp(suffix, "sya") == 0 || strcmp(suffix, "yoH") == 0 ||
          strcmp(suffix, "e") == 0 || suffix[0] == '\0') {
        /* These suffixes attach directly to the a-stem base. */
      } else if (suffix[0] == 'A' || suffix[0] == 'e' || suffix[0] == 'O') {
        form[strlen(form) - 1] = '\0';
      }
    }
    strncat(form, suffix, sizeof(form) - strlen(form) - 1);
    strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
    ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
    ctx_out->term_count = 1;
    if (vib == ASH_PRATHAMA_VIB &&
        (vac == ASH_EKAVACANA || vac == ASH_BAHUVACANA)) {
      prakriya_log_transition(ctx_out, 803015, "KaravasAnayor visarjanIyaH", before, form);
    } else if (vib == ASH_DVITIYA_VIB && vac == ASH_BAHUVACANA) {
      prakriya_log_transition(ctx_out, 701012, "wA-Nasi-NasAm inAdyAH", before, form);
    } else if (vib == ASH_TRITIYA_VIB && vac == ASH_EKAVACANA) {
      prakriya_log_transition(ctx_out, 703102, "supi ca", before, form);
    } else {
      prakriya_log_transition(ctx_out, 401002, "svOjasamOw", before, form);
    }
  }
  return true;
}
