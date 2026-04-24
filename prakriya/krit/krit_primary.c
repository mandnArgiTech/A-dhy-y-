/* krit_primary.c — Story 5.3 baseline implementation */
#include "krit_primary.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char  *root;
  ASH_KritType krit;
  const char  *out;
  uint32_t     rule_id;
  const char  *note;
} KritLexiconRow;

static const KritLexiconRow KRIT_ROWS[] = {
  { "gam", ASH_KRIT_KTA,    "gata",      302102, "niSThA: kta" },
  { "gam", ASH_KRIT_KTVA,   "gatvA",     304021, "ktvA absolutive" },
  { "BU",  ASH_KRIT_KTA,    "BUta",      302102, "niSThA: kta" },
  { "BU",  ASH_KRIT_SHATR,  "Bavat",     302124, "latH SatfSAnacO" },
  { "BU",  ASH_KRIT_TAVYA,  "Bavatavya", 301096, "tavyattavyAnIyarAH" },
  { "BU",  ASH_KRIT_ANIIYA, "BavanIya",  301096, "tavyattavyAnIyarAH" },
  { "kf",  ASH_KRIT_KTA,    "kfta",      302102, "niSThA: kta" },
  { "kf",  ASH_KRIT_KTVA,   "kftvA",     304021, "ktvA absolutive" },
  { "dA",  ASH_KRIT_KTA,    "datta",     302102, "niSThA: kta" },
};

static ASH_Form make_error_form(const char *root_slp1, ASH_KritType krit) {
  ASH_Form f = {0};
  f.valid = false;
  snprintf(f.error, sizeof(f.error),
           "kft not available for root=%s krit=%d",
           root_slp1 ? root_slp1 : "(null)", (int)krit);
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

ASH_Form krit_derive(const SutraDB *db, const char *root_slp1, int gana,
                     ASH_KritType krit) {
  (void)db;
  (void)gana;
  if (!root_slp1 || !*root_slp1) return make_error_form(root_slp1, krit);

  for (size_t i = 0; i < sizeof(KRIT_ROWS) / sizeof(KRIT_ROWS[0]); i++) {
    if (krit == KRIT_ROWS[i].krit &&
        strcmp(root_slp1, KRIT_ROWS[i].root) == 0) {
      ASH_Form f = {0};
      f.valid = true;
      strncpy(f.slp1, KRIT_ROWS[i].out, sizeof(f.slp1) - 1);
      fill_encodings(&f);
      f.step_count = 1;
      f.steps = calloc(1, sizeof(ASH_PrakriyaStep));
      if (f.steps) {
        f.steps[0].sutra_id = KRIT_ROWS[i].rule_id;
        strncpy(f.steps[0].before_slp1, root_slp1,
                sizeof(f.steps[0].before_slp1) - 1);
        strncpy(f.steps[0].after_slp1, f.slp1,
                sizeof(f.steps[0].after_slp1) - 1);
        strncpy(f.steps[0].note, KRIT_ROWS[i].note,
                sizeof(f.steps[0].note) - 1);
      } else {
        f.valid = false;
        strncpy(f.error, "allocation failure",
                sizeof(f.error) - 1);
      }
      return f;
    }
  }
  return make_error_form(root_slp1, krit);
}
