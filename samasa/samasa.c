/* samasa.c — Story 5.2 baseline implementation */
#include "samasa.h"
#include "sandhi_vowel.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ASH_Form make_error_form(const char *purva, const char *uttara) {
  ASH_Form f = {0};
  f.valid = false;
  snprintf(f.error, sizeof(f.error),
           "samasa derivation failed for %s + %s",
           purva ? purva : "(null)",
           uttara ? uttara : "(null)");
  return f;
}

static void fill_encodings(ASH_Form *f) {
  char *iast = enc_slp1_to_iast(f->slp1);
  if (iast) {
    strncpy(f->iast, iast, sizeof(f->iast) - 1);
    free(iast);
  }
  char *deva = enc_slp1_to_devanagari(f->slp1);
  if (deva) {
    strncpy(f->devanagari, deva, sizeof(f->devanagari) - 1);
    free(deva);
  }
}

static bool samasa_join(const char *purva, const char *uttara, char *out, size_t out_len) {
  if (!purva || !uttara || !out) return false;
  if (!sandhi_vowel_join(purva, uttara, out, out_len)) {
    snprintf(out, out_len, "%s%s", purva, uttara);
  }
  return true;
}

ASH_Form samasa_derive(const SutraDB *db,
                       const char *purva_slp1, const char *uttara_slp1,
                       ASH_SamasaType type, ASH_Linga linga) {
  (void)db;
  (void)linga;
  if (!purva_slp1 || !uttara_slp1) return make_error_form(purva_slp1, uttara_slp1);

  ASH_Form f = {0};
  f.valid = true;
  if (!samasa_join(purva_slp1, uttara_slp1, f.slp1, sizeof(f.slp1))) {
    return make_error_form(purva_slp1, uttara_slp1);
  }

  if (type == ASH_SAMASA_AVYAYIBHAVA) {
    size_t n = strlen(f.slp1);
    if (n + 1 < sizeof(f.slp1) && f.slp1[n - 1] != 'm') {
      f.slp1[n] = 'm';
      f.slp1[n + 1] = '\0';
    }
  } else if (type == ASH_SAMASA_DVANDVA) {
    size_t n = strlen(f.slp1);
    if (n > 0 && f.slp1[n - 1] == 'a') {
      /* Nom. dual ending in the common a-stem case: -a -> -O */
      f.slp1[n - 1] = 'O';
    } else if (n + 1 < sizeof(f.slp1)) {
      f.slp1[n] = 'O';
      f.slp1[n + 1] = '\0';
    }
  }

  fill_encodings(&f);
  f.step_count = 1;
  f.steps = calloc(1, sizeof(ASH_PrakriyaStep));
  if (!f.steps) {
    f.valid = false;
    strncpy(f.error, "allocation failure", sizeof(f.error) - 1);
    return f;
  }
  switch (type) {
    case ASH_SAMASA_TATPURUSHA:
      f.steps[0].sutra_id = 201022;
      strncpy(f.steps[0].note, "tatpuruSa", sizeof(f.steps[0].note) - 1);
      break;
    case ASH_SAMASA_KARMADHAARAYA:
      f.steps[0].sutra_id = 201057;
      strncpy(f.steps[0].note, "karmadhAraya", sizeof(f.steps[0].note) - 1);
      break;
    case ASH_SAMASA_BAHUVRIHI:
      f.steps[0].sutra_id = 202023;
      strncpy(f.steps[0].note, "bahuvrIhi", sizeof(f.steps[0].note) - 1);
      break;
    case ASH_SAMASA_DVANDVA:
      f.steps[0].sutra_id = 202029;
      strncpy(f.steps[0].note, "dvandva", sizeof(f.steps[0].note) - 1);
      break;
    case ASH_SAMASA_AVYAYIBHAVA:
      f.steps[0].sutra_id = 201006;
      strncpy(f.steps[0].note, "avyayIBhAva", sizeof(f.steps[0].note) - 1);
      break;
    case ASH_SAMASA_DVIGU:
      f.steps[0].sutra_id = 201052;
      strncpy(f.steps[0].note, "dvigu", sizeof(f.steps[0].note) - 1);
      break;
  }
  strncpy(f.steps[0].before_slp1, purva_slp1, sizeof(f.steps[0].before_slp1) - 1);
  strncat(f.steps[0].before_slp1, "+", sizeof(f.steps[0].before_slp1) - strlen(f.steps[0].before_slp1) - 1);
  strncat(f.steps[0].before_slp1, uttara_slp1, sizeof(f.steps[0].before_slp1) - strlen(f.steps[0].before_slp1) - 1);
  strncpy(f.steps[0].after_slp1, f.slp1, sizeof(f.steps[0].after_slp1) - 1);
  return f;
}
