/* vibhakti.h — sUP suffix table and oracle lookup helpers */
#ifndef VIBHAKTI_H
#define VIBHAKTI_H
#include "ashtadhyayi.h"
#include "samjna.h"
#include "context.h"

/* Represents one nominal suffix entry selected by case/number/gender. */
typedef struct {
  const char  *upadesa_slp1;
  char         clean_slp1[16];
  ASH_Vibhakti vibhakti;
  ASH_Vacana   vacana;
  ASH_Linga    linga;
  Samjna       samjna;
} SupEntry;

/* Returns the sUP entry for the requested case/number/gender. */
const SupEntry *sup_get(ASH_Vibhakti vib, ASH_Vacana vac, ASH_Linga lin);

/* Appends the sUP suffix as the next derivation term in the context. */
int sup_assign(PrakriyaCtx *ctx, ASH_Vibhakti vib, ASH_Vacana vac, ASH_Linga lin);

#endif
