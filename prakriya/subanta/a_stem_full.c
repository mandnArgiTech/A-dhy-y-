/* a_stem_full.c — Story 4.10: full 24-slot a-stem paradigms.
   Replaces the legacy a_stem_masc_derive's hand-tuned suffix selection
   with proper slot tables for both masculine (rAma-style) and neuter
   (vana-style) a-stems. The legacy helper is retained for the few
   coverage tests that call it directly. */

#include "a_stem.h"
#include "varna.h"
#include "sandhi_natva.h"
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

static inline void apply_natva(const char *stem, char *form) {
  (void)stem; sandhi_apply_natva(form);
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

/* 2.4.59 lup + 7.4.59 hrasvaḥ — for gotra-pratyaya stems (Agastya,
 * gAlava, etc.), the gotra-suffix drops in BAHUVACANA and the
 * initial vṛddhi-A reverts to short a. Closed seed list. Returns
 * the derived stem and i/as classification. */
typedef struct {
  const char *full_stem;     /* e.g. "Agastya", "ANgirasa" */
  const char *short_stem;    /* e.g. "agasti", "aNgiras" */
  bool is_as_stem;           /* aṅgiras → as-stem-style 3-7 bahu */
  bool is_i_stem;            /* agasti → i-stem-style 1-2 bahu */
} GotraOverride;

static const GotraOverride GOTRA_TABLE[] = {
  {"Agastya",  "agasti",   false, true},
  {"ANgirasa", "aNgiras",  true,  false},
  {"gAlava",   "galava",   false, false},
  {NULL, NULL, false, false},
};

static const GotraOverride *find_gotra(const char *stem) {
  for (int i = 0; GOTRA_TABLE[i].full_stem; i++) {
    if (strcmp(stem, GOTRA_TABLE[i].full_stem) == 0)
      return &GOTRA_TABLE[i];
  }
  return NULL;
}

bool a_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  /* 2.4.59 gotra-luk in BAHUVACANA: switch stem and paradigm. */
  if (vac == ASH_BAHUVACANA) {
    const GotraOverride *g = find_gotra(stem_slp1);
    if (g) {
      char form[TERM_VALUE_LEN] = {0};
      const char *suffix = NULL;
      if (g->is_i_stem) {
        switch (vib) {
          case ASH_PRATHAMA_VIB:   suffix = "ayaH"; break;
          case ASH_DVITIYA_VIB:    suffix = "In";   break;
          case ASH_TRITIYA_VIB:    suffix = "BiH";  break;
          case ASH_CATURTHI_VIB:   suffix = "ByaH"; break;
          case ASH_PANCAMI_VIB:    suffix = "ByaH"; break;
          case ASH_SHASTHI_VIB:    suffix = "nAm";  break;
          case ASH_SAPTAMI_VIB:    suffix = "zu";   break;
          default: suffix = NULL;
        }
        if (suffix) {
          if (vib == ASH_PRATHAMA_VIB) {
            /* i-stem 1bahu: drop final i and use -ayaH (guṇa).
               agasti + jas → agastay + aH = agastayaH. */
            size_t sl = strlen(g->short_stem);
            char trunc[64];
            strncpy(trunc, g->short_stem, sl - 1);
            trunc[sl - 1] = '\0';
            snprintf(form, sizeof(form), "%s%s", trunc, suffix);
          } else if (vib == ASH_SHASTHI_VIB) {
            /* i-stem 6bahu: 7.1.55 nāmi — short i → long I before nām.
               agasti + nAm → agastInAm. */
            size_t sl = strlen(g->short_stem);
            char lengthened[64];
            strncpy(lengthened, g->short_stem, sl);
            lengthened[sl] = '\0';
            if (sl > 0 && lengthened[sl - 1] == 'i') lengthened[sl - 1] = 'I';
            else if (sl > 0 && lengthened[sl - 1] == 'u') lengthened[sl - 1] = 'U';
            snprintf(form, sizeof(form), "%s%s", lengthened, suffix);
          } else if (vib == ASH_SAPTAMI_VIB) {
            snprintf(form, sizeof(form), "%s%s", g->short_stem, suffix);
          } else if (vib == ASH_DVITIYA_VIB) {
            /* "agasti" → drop final i, add In: agastIn */
            size_t sl = strlen(g->short_stem);
            char trunc[64];
            strncpy(trunc, g->short_stem, sl - 1);
            trunc[sl - 1] = '\0';
            snprintf(form, sizeof(form), "%sIn", trunc);
          } else {
            snprintf(form, sizeof(form), "%s%s", g->short_stem, suffix);
          }
        }
      } else if (g->is_as_stem) {
        switch (vib) {
          case ASH_PRATHAMA_VIB:
          case ASH_DVITIYA_VIB:    suffix = "aH";   break;
          case ASH_TRITIYA_VIB:    suffix = "BiH";  break;
          case ASH_CATURTHI_VIB:   suffix = "ByaH"; break;
          case ASH_PANCAMI_VIB:    suffix = "ByaH"; break;
          case ASH_SHASTHI_VIB:    suffix = "Am";   break;
          case ASH_SAPTAMI_VIB:    suffix = "su";   break;
          default: suffix = NULL;
        }
        if (suffix) {
          /* For 3-5-bahu (-ByaH-class), as-stem turns 's' → 'o' first
             via 8.2.66 then 8.3.15: aṅgiras + BiH → aṅgiro + BiH. */
          bool is_byah = (vib == ASH_TRITIYA_VIB || vib == ASH_CATURTHI_VIB ||
                          vib == ASH_PANCAMI_VIB);
          if (is_byah) {
            char modified[64];
            size_t sl = strlen(g->short_stem);
            strncpy(modified, g->short_stem, sl - 2);
            modified[sl - 2] = 'o';
            modified[sl - 1] = '\0';
            snprintf(form, sizeof(form), "%s%s", modified, suffix);
          } else if (vib == ASH_SAPTAMI_VIB) {
            /* aṅgiras + su → aṅgiraHsu (visarga before su). */
            char modified[64];
            size_t sl = strlen(g->short_stem);
            strncpy(modified, g->short_stem, sl - 1);
            modified[sl - 1] = 'H';
            modified[sl] = '\0';
            snprintf(form, sizeof(form), "%s%s", modified, suffix);
          } else {
            snprintf(form, sizeof(form), "%s%s", g->short_stem, suffix);
          }
        }
      }
      if (form[0]) {
        prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
        ctx_out->term_count = 1;
        strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
        ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
        prakriya_log_transition(ctx_out, 204059, stem_slp1, form,
                                "gotra-luk bahuvacane (2.4.59)");
        return true;
      }
    }
  }
  return a_stem_full_derive(stem_slp1, A_MASC, ASH_PUMS, vib, vac, ctx_out);
}

bool a_stem_neut_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  return a_stem_full_derive(stem_slp1, A_NEUT, ASH_NAPUMSAKA, vib, vac,
                            ctx_out);
}
