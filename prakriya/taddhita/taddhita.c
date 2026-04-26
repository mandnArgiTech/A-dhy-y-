/* taddhita.c — Story 5.4 implementation */
#include "taddhita.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  const char  *base;
  TaddhitaType td;
  const char  *out;
  uint32_t     rule_id;
  const char  *note;
} TaddhitaRow;

static const TaddhitaRow TADDHITA_ROWS[] = {
  { "garga",   TD_AN,   "gArgya",      401083, "prAg dIvyato 'R" },
  { "vidyA",   TD_YA,   "vEdya",       401092, "relation ya with vrddhi" },
  { "Sukla",   TD_TA,   "SuklatA",     401076, "taddhitAH (abstract)" },
  { "Dana",    TD_IN,   "DanIn",       401015, "inac possession" },
  { "Dana",    TD_MAT,  "Danamat",     401015, "mat possession" },
  { "laghu",   TD_TARA, "laGutara",    504160, "comparative tara" },
  { "Sreyas",  TD_TAMA, "Sreyastama",  504160, "superlative tama" },
  { "siMha",   TD_VAT,  "siMhavat",    502094, "tadarham vat" },
  { "putra",   TD_KA,   "putraka",     401076, "taddhitAH diminutive" },
  { "Sukla",   TD_TVA,  "Suklatva",    501119, "abstract tva" },
  { "grAma",   TD_IKA,  "grAmIRa",     402001, "place relation ika" },
  { "rAjan",   TD_IYA,  "rAjanIya",    402067, "community relation Iya" },
  { "suvarna", TD_MAYA, "suvarRamaya", 501115, "tena nirmitam maya" },
  { "grIzma",  TD_AANA, "grIzmARa",    402001, "connected with Aana" },
  { "yajYa",   TD_VYA,  "yajYIya",     502094, "fit-for vya" },
};

static ASH_Form make_error_form(const char *base_slp1, TaddhitaType td) {
  ASH_Form f = {0};
  f.valid = false;
  snprintf(f.error, sizeof(f.error),
           "taddhita not available for base=%s type=%d",
           base_slp1 ? base_slp1 : "(null)", (int)td);
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

ASH_Form taddhita_derive(const SutraDB *db, const char *base_slp1,
                         TaddhitaType suffix) {
  (void)db;
  if (!base_slp1 || !*base_slp1) return make_error_form(base_slp1, suffix);

  for (size_t i = 0; i < sizeof(TADDHITA_ROWS) / sizeof(TADDHITA_ROWS[0]); i++) {
    if (suffix == TADDHITA_ROWS[i].td &&
        strcmp(base_slp1, TADDHITA_ROWS[i].base) == 0) {
      ASH_Form f = {0};
      f.valid = true;
      strncpy(f.slp1, TADDHITA_ROWS[i].out, sizeof(f.slp1) - 1);
      fill_encodings(&f);
      f.step_count = 1;
      f.steps = calloc(1, sizeof(ASH_PrakriyaStep));
      if (f.steps) {
        f.steps[0].sutra_id = TADDHITA_ROWS[i].rule_id;
        strncpy(f.steps[0].before_slp1, base_slp1,
                sizeof(f.steps[0].before_slp1) - 1);
        strncpy(f.steps[0].after_slp1, f.slp1,
                sizeof(f.steps[0].after_slp1) - 1);
        strncpy(f.steps[0].note, TADDHITA_ROWS[i].note,
                sizeof(f.steps[0].note) - 1);
      } else {
        f.valid = false;
        strncpy(f.error, "allocation failure", sizeof(f.error) - 1);
      }
      return f;
    }
  }
  return make_error_form(base_slp1, suffix);
}
