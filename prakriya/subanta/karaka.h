/* karaka.h — kāraka ↔ vibhakti mapping utilities */
#ifndef KARAKA_H
#define KARAKA_H
#include "ashtadhyayi.h"

/* Return the default case mapping for a semantic role. */
ASH_Vibhakti karaka_default_vibhakti(ASH_Karaka k);

/* Return a stable ASCII label for a kāraka enum value. */
const char *karaka_name(ASH_Karaka k);

/* Return a stable ASCII label for a vibhakti enum value. */
const char *vibhakti_name(ASH_Vibhakti v);
#endif
