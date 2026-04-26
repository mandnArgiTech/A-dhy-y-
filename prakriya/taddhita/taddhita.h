/* taddhita.h — secondary derivative (taddhita) helpers */
#ifndef TADDHITA_H
#define TADDHITA_H
#include "ashtadhyayi.h"
#include "sutra.h"

/* Subset of Story 5.4 taddhita suffix kinds. */
typedef enum {
  TD_AN = 1,
  TD_YA,
  TD_IN,
  TD_MAT,
  TD_TA,
  TD_TVA,
  TD_KA,
  TD_IKA,
  TD_IYA,
  TD_TAMA,
  TD_TARA,
  TD_VAT,
  TD_MAYA,
  TD_AANA,
  TD_VYA,
} TaddhitaType;

/* Derives one taddhita form from a base stem in SLP1. */
ASH_Form taddhita_derive(const SutraDB *db, const char *base_slp1,
                         TaddhitaType suffix);

/* Returns true when this suffix triggers first-vowel vrddhi behavior. */
bool taddhita_causes_vrddhi(TaddhitaType suffix);

/* Returns ASCII name for logging and tests. */
const char *taddhita_type_name(TaddhitaType suffix);
#endif
