/* lakara.h — la-kAra and tiN suffix utilities */
#ifndef LAKARA_H
#define LAKARA_H
#include "ashtadhyayi.h"
#include "samjna.h"
#include "context.h"

typedef struct {
  ASH_Lakara  lakara;
  ASH_Purusha purusha;
  ASH_Vacana  vacana;
  ASH_Pada    pada;
  const char *upadesa;
  const char *clean;
  Samjna      samjna;
} TingEntry;

const TingEntry *ting_get(ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd);
int ting_assign(PrakriyaCtx *ctx, ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd);
bool lakara_is_sarvadhatuka(ASH_Lakara l);
const char *lakara_name(ASH_Lakara l);
#endif
