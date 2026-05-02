/* a_stem_full.c — Story 4.10: full 24-slot a-stem paradigms.
   Replaces the legacy a_stem_masc_derive's hand-tuned suffix selection
   with proper slot tables for both masculine (rAma-style) and neuter
   (vana-style) a-stems. The legacy helper is retained for the few
   coverage tests that call it directly. */

#include "a_stem.h"
#include "varna.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *ending;     /* what to append after dropping stem-final `a` */
  uint32_t     sutra_id;
} ASlot;

/* rAma paradigm — masculine a-stem. */
static const ASlot A_MASC[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "aH",    802066},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "O",     701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "AH",    802066},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "am",    401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "O",     701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "An",    701012},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "ena",   703102}, /* 8.4.1 may give eRa */
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "AByAm", 401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "EH",    701009}, /* atojaṣEs */
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "Aya",   701013},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "AByAm", 401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "eByaH", 701030},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "At",    701015},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "AByAm", 401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "eByaH", 701030},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "asya",  701012},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "ayoH",  701032},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "AnAm",  604003}, /* 8.4.1 may give ARAm */
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "e",     701013},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "ayoH",  701032},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "ezu",   803059}, /* 8.4.1 may give ezu→ezu */
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "a",     401002},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "O",     701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "AH",    802066},
};

/* vana paradigm — neuter a-stem. */
static const ASlot A_NEUT[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "am",    401002},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "e",     701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "Ani",   701073},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "am",    401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "e",     701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "Ani",   701073},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "ena",   703102},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "AByAm", 401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "EH",    701009},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "Aya",   701013},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "AByAm", 401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "eByaH", 701030},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "At",    701015},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "AByAm", 401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "eByaH", 701030},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "asya",  701012},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "ayoH",  701032},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "AnAm",  604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "e",     701013},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "ayoH",  701032},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "ezu",   803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "a",     401002},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "e",     701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "Ani",   701073},
};

static bool stem_triggers_natva(const char *stem) {
  if (!stem) return false;
  for (size_t i = 0; stem[i]; i++) {
    char c = stem[i];
    if (c == 'r' || c == 'f' || c == 'z' || c == 'F' || c == 'R') return true;
  }
  return false;
}

static void apply_natva(const char *stem, char *form) {
  if (!stem_triggers_natva(stem)) return;
  for (size_t i = 0; form[i]; i++) {
    if (form[i] == 'n' && form[i + 1] && varna_is_vowel(form[i + 1])) {
      form[i] = 'R';
    }
  }
}

static const ASlot *a_slot_lookup(const ASlot *table, ASH_Vibhakti vib,
                                  ASH_Vacana vac) {
  for (size_t i = 0; i < 24; i++) {
    if (table[i].vib == vib && table[i].vac == vac) return &table[i];
  }
  return NULL;
}

static bool a_stem_full_derive(const char *stem_slp1, const ASlot *table,
                               ASH_Linga linga, ASH_Vibhakti vib,
                               ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out || !table) return false;
  size_t n = strlen(stem_slp1);
  /* Accept both short-a and long-A finals: NAPUMSAKA stems are sometimes
     listed in the dhātupāṭha / lexicon with their long-A upadeśa form
     (e.g. "SrIpA" for the neuter of "SrIpa"). The slot tables drop the
     final vowel either way. */
  if (n == 0 || (stem_slp1[n - 1] != 'a' && stem_slp1[n - 1] != 'A')) {
    return false;
  }
  const ASlot *slot = a_slot_lookup(table, vib, vac);
  if (!slot) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, linga, vib, vac);
  ctx_out->term_count = 1;
  char base[TERM_VALUE_LEN] = {0};
  size_t base_len = n - 1;
  memcpy(base, stem_slp1, base_len);
  base[base_len] = '\0';
  char form[TERM_VALUE_LEN] = {0};
  snprintf(form, sizeof(form), "%s%s", base, slot->ending);
  apply_natva(stem_slp1, form);
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, slot->sutra_id, stem_slp1, form,
                          "a-stem paradigm slot");
  return true;
}

bool a_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  return a_stem_full_derive(stem_slp1, A_MASC, ASH_PUMS, vib, vac, ctx_out);
}

bool a_stem_neut_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  return a_stem_full_derive(stem_slp1, A_NEUT, ASH_NAPUMSAKA, vib, vac,
                            ctx_out);
}
