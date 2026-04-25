#ifndef LAT_BHVADI_H
#define LAT_BHVADI_H

#include "ashtadhyayi.h"

/* Simple laT derivation helper used by pipeline. */
bool lat_bhvadi_derive(const char *dhatu_slp1, int gana, ASH_Purusha p,
                       ASH_Vacana v, ASH_Pada pd, char *out_slp1, size_t out_len);

#endif
