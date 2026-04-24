/* taddhita.h — secondary derivative helpers; Story 5.4 */
#ifndef TADDHITA_H
#define TADDHITA_H

#include "ashtadhyayi.h"
#include "sutra.h"

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
  TD_VYA
} TaddhitaType;

ASH_Form taddhita_derive(const SutraDB *db, const char *base_slp1,
                         TaddhitaType suffix);
bool taddhita_causes_vrddhi(TaddhitaType suffix);
const char *taddhita_type_name(TaddhitaType suffix);

#endif
