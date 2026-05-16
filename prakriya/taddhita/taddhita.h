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
  /* Phase δ taddhita expansion */
  TD_TAL,    /* abstract -tā per 5.1.119 */
  TD_TVAL,   /* abstract -tva per 5.1.119 */
  TD_INI,    /* possessive -in per 5.2.115 */
  TD_VINI,   /* possessive -vin per 5.2.121 */
  TD_LAC,    /* -la possessive per 5.2.97 */
  TD_NA,     /* -na adjective per 5.2.27 */
  TD_THAK,   /* -ika (gotra) per 4.1.83 */
  TD_THAN,   /* -tha per 5.2.51 */
  TD_VATUP,  /* -vat (comparable to) per 5.2.94 */
  TD_DA,     /* -da per 5.2.108 */
  TD_DVAYASAC, /* -dvayasa per 5.2.37 (measure) */
  TD_DAGHNAC,  /* -daghna per 5.2.37 */
  TD_MAATRAC,  /* -mātra per 5.2.37 */
  TD_KALPAP,   /* -kalpa per 5.3.67 (almost-X) */
  TD_DESHIYAR, /* -deśīya per 5.3.67 */
  TD_TVAN,     /* -tva (alt with anubandha) per 5.1.119 */
  TD_THAL,     /* -tha (kit alternate) */
  TD_AIYA,     /* -aiya for gotra/family */
  TD_DHAN,     /* -dha (measure) per 5.3.42 */
  TD_VAT2,     /* -vat (like, similar to) per 5.1.115 */
} TaddhitaType;

/* Derives one taddhita form from a base stem in SLP1. */
ASH_Form taddhita_derive(const SutraDB *db, const char *base_slp1,
                         TaddhitaType suffix);

/* Returns true when this suffix triggers first-vowel vrddhi behavior. */
bool taddhita_causes_vrddhi(TaddhitaType suffix);

/* Returns ASCII name for logging and tests. */
const char *taddhita_type_name(TaddhitaType suffix);
#endif
