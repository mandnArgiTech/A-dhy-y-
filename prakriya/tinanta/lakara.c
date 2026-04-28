/* lakara.c — laṭ tiṅ table utilities; Story 3.3 */
#include "lakara.h"
#include <string.h>

static const TingEntry LAT_PARASMAI[9] = {
    {ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI, "tip", "ti", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_PRATHAMA, ASH_DVIVACANA, ASH_PARASMAI, "tas", "tas", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_PRATHAMA, ASH_BAHUVACANA, ASH_PARASMAI, "Ji", "anti", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_MADHYAMA, ASH_EKAVACANA, ASH_PARASMAI, "sip", "si", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_MADHYAMA, ASH_DVIVACANA, ASH_PARASMAI, "Tas", "Tas", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_MADHYAMA, ASH_BAHUVACANA, ASH_PARASMAI, "Ta", "Ta", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_UTTAMA, ASH_EKAVACANA, ASH_PARASMAI, "mip", "mi", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_UTTAMA, ASH_DVIVACANA, ASH_PARASMAI, "vas", "vas", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_UTTAMA, ASH_BAHUVACANA, ASH_PARASMAI, "mas", "mas", SJ_TING | SJ_PARASMAIPADA | SJ_SARVADHATUKA},
};

static const TingEntry LAT_ATMANE[9] = {
    {ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_ATMANE, "ta", "te", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_PRATHAMA, ASH_DVIVACANA, ASH_ATMANE, "AtAm", "Ate", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_PRATHAMA, ASH_BAHUVACANA, ASH_ATMANE, "Ja", "ante", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_MADHYAMA, ASH_EKAVACANA, ASH_ATMANE, "TAs", "se", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_MADHYAMA, ASH_DVIVACANA, ASH_ATMANE, "ATAm", "ATe", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_MADHYAMA, ASH_BAHUVACANA, ASH_ATMANE, "Dvam", "Dve", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_UTTAMA, ASH_EKAVACANA, ASH_ATMANE, "iw", "e", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_UTTAMA, ASH_DVIVACANA, ASH_ATMANE, "vahi", "vahe", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
    {ASH_LAT, ASH_UTTAMA, ASH_BAHUVACANA, ASH_ATMANE, "mahiN", "mahe", SJ_TING | SJ_ATMANEPADA | SJ_SARVADHATUKA},
};

static int ting_index(ASH_Purusha p, ASH_Vacana v) {
  return ((int)p * 3) + (int)v;
}

const TingEntry *ting_get(ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd) {
  if (l != ASH_LAT || p > ASH_UTTAMA || v > ASH_BAHUVACANA) return NULL;
  int idx = ting_index(p, v);
  if (idx < 0 || idx >= 9) return NULL;
  if (pd == ASH_ATMANE) return &LAT_ATMANE[idx];
  return &LAT_PARASMAI[idx];
}

int ting_assign(PrakriyaCtx *ctx, ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd) {
  if (!ctx || ctx->term_count >= MAX_TERMS) return -1;
  const TingEntry *t = ting_get(l, p, v, pd);
  if (!t) return -1;

  Term *dst = &ctx->terms[ctx->term_count];
  term_init(dst, t->upadesa, SJ_PRATYAYA | SJ_TING);
  dst->samjna = samjna_add(dst->samjna, t->samjna);

  /* Ensure the clean form matches our target table for determinism. */
  strncpy(dst->value, t->clean, TERM_VALUE_LEN - 1);
  dst->value[TERM_VALUE_LEN - 1] = '\0';

  ctx->term_count++;
  return 0;
}

bool lakara_is_sarvadhatuka(ASH_Lakara l) {
  return (l == ASH_LAT || l == ASH_LOT || l == ASH_LAN || l == ASH_VIDHILIM);
}

const char *lakara_name(ASH_Lakara l) {
  static const char *const names[] = {
      "laT", "liT", "luT", "lfT", "loT", "laN", "viDiliN", "ASIrliN", "luN", "lfN"};
  if (l < 0 || l >= ASH_LAKARA_COUNT) return "unknown";
  return names[(int)l];
}
