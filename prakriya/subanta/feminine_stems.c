/* feminine_stems.c — Story 4.8: feminine vowel-stem paradigms.
   Covers nadI (long-ī), vaDU (long-ū), mati (short-i fem), Denu
   (short-u fem). Each paradigm uses the same drop-stem-final-vowel +
   append-slot-suffix kernel as Story 4.6. */

#include "aaiu_stems.h"
#include "varna.h"
#include <stdio.h>
#include <string.h>

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *ending;     /* what to append after dropping stem-final vowel */
  uint32_t     sutra_id;
} FemSlot;

/* nadI paradigm (long-ī feminine, ñīp class): s of prathama-eka drops. */
static const FemSlot II_FEM[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "I",    603024},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "yO",   601077},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "yaH",  601077},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "Im",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "yO",   601077},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "IH",   601068},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "yA",   601077},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "IByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "IBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "yE",   703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "IByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "IByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "yAH",  703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "IByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "IByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "yAH",  703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "yoH",  601077},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "InAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "yAm",  703116},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "yoH",  601077},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "Izu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "i",    703108},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "yO",   601077},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "yaH",  601077},
};

/* vaDU paradigm (long-ū feminine, ūṅ class): s of prathama-eka surfaces
   as visarga. */
static const FemSlot UU_FEM[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "UH",   401002},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "vO",   601077},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "vaH",  601077},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "Um",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "vO",   601077},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "UH",   601068},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "vA",   601077},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "UByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "UBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "vE",   703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "UByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "UByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "vAH",  703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "UByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "UByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "vAH",  703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "voH",  601077},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "UnAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "vAm",  703116},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "voH",  601077},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "Uzu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "u",    703108},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "vO",   601077},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "vaH",  601077},
};

/* mati paradigm (short-i feminine). Differs from agni masc in
   dvitīyā-bahu (matIH not matIn) and a few eka variants. */
static const FemSlot I_FEM[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "iH",   401002},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "I",    701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "ayaH", 601078},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "im",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "I",    701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "IH",   604003},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "yA",   601077},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "iByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "iBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "aye",  703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "iByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "iByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "eH",   703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "iByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "iByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "eH",   703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "yoH",  601077},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "InAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "O",    703119},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "yoH",  601077},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "izu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "e",    703108},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "I",    701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "ayaH", 601078},
};

/* Denu paradigm (short-u feminine). Parallels mati with i↔u, e↔o. */
static const FemSlot U_FEM[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "uH",   401002},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "U",    701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "avaH", 601078},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "um",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "U",    701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "UH",   604003},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "vA",   601077},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "uByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "uBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "ave",  703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "uByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "uByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "oH",   703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "uByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "uByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "oH",   703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "voH",  601077},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "UnAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "O",    703119},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "voH",  601077},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "uzu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "o",    703108},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "U",    701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "avaH", 601078},
};

/* ramA paradigm (long-ā feminine, ṭāp class). Drop stem-final A, append
   slot ending. Note: tṛtīyā-eka uses "ayA" (ramayA), and dvivacana
   prathama/dvitīyā/sambodhana = "e" (ā → e via vowel-replacement). */
static const FemSlot AA_FEM[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "A",    603024},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "e",    701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "AH",   401002},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "Am",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "e",    701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "AH",   401002},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "ayA",  703105},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "AByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "ABiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "AyE",  703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "AByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "AByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "AyAH", 703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "AByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "AByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "AyAH", 703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "ayoH", 703105},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "AnAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "AyAm", 703116},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "ayoH", 703105},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "Asu",  401002},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "e",    701018},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "e",    701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "AH",   401002},
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

static const FemSlot *fem_slot_lookup(const FemSlot *table, ASH_Vibhakti vib,
                                      ASH_Vacana vac) {
  for (size_t i = 0; i < 24; i++) {
    if (table[i].vib == vib && table[i].vac == vac) return &table[i];
  }
  return NULL;
}

static bool fem_derive(const char *stem_slp1, const FemSlot *table,
                       ASH_Vibhakti vib, ASH_Vacana vac,
                       PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !table || !ctx_out) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0) return false;
  const FemSlot *slot = fem_slot_lookup(table, vib, vac);
  if (!slot) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_STRI, vib, vac);
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
                          "feminine paradigm slot");
  return true;
}

bool ii_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_slp1) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0 || stem_slp1[n - 1] != 'I') return false;
  return fem_derive(stem_slp1, II_FEM, vib, vac, ctx_out);
}

bool uu_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_slp1) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0 || stem_slp1[n - 1] != 'U') return false;
  return fem_derive(stem_slp1, UU_FEM, vib, vac, ctx_out);
}

bool i_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                     PrakriyaCtx *ctx_out) {
  if (!stem_slp1) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0 || stem_slp1[n - 1] != 'i') return false;
  return fem_derive(stem_slp1, I_FEM, vib, vac, ctx_out);
}

bool u_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                     PrakriyaCtx *ctx_out) {
  if (!stem_slp1) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0 || stem_slp1[n - 1] != 'u') return false;
  return fem_derive(stem_slp1, U_FEM, vib, vac, ctx_out);
}

bool aa_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_slp1) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0 || stem_slp1[n - 1] != 'A') return false;
  return fem_derive(stem_slp1, AA_FEM, vib, vac, ctx_out);
}
