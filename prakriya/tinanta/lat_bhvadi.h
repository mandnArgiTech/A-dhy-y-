#ifndef LAT_BHVADI_H
#define LAT_BHVADI_H

#include "ashtadhyayi.h"
#include "context.h"

bool lat_bhvadi_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                           ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);

/* Simple laT derivation helper used by tests and pipeline. */
bool lat_bhvadi_derive(const char *dhatu_slp1, int gana, ASH_Purusha p,
                       ASH_Vacana v, ASH_Pada pd, char *out_slp1, size_t out_len);

/* Story 3.17–3.22: parameterised derivation supporting LAN, LOT,
   VIDHILIN, LRT in addition to LAT. */
bool lakara_derive_ctx(ASH_Lakara lakara,
                       const char *dhatu_slp1, int gana,
                       ASH_Purusha p, ASH_Vacana v, ASH_Pada pd,
                       PrakriyaCtx *ctx_out);
bool lakara_derive(ASH_Lakara lakara,
                   const char *dhatu_slp1, int gana,
                   ASH_Purusha p, ASH_Vacana v, ASH_Pada pd,
                   char *out_slp1, size_t out_len);

#endif
