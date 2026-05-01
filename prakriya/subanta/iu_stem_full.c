/* iu_stem_full.c — full 24-slot subanta paradigms for i- and u-stems.
   Story 4.6: replaces the legacy stub helpers in aaiu_stems.c which only
   covered 2-3 slots. The legacy helpers are retained for backwards
   compatibility with existing tests; new dispatch should prefer the
   `_full` variants. */

#include "aaiu_stems.h"
#include "varna.h"
#include <string.h>

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  /* Suffix to append after dropping the stem-final i/I/u/U. The suffix
     already encodes any vowel-fusion (e.g. -aye for caturthī-eka i-stem
     where the stem-final i fuses into the suffix). */
  const char  *ending;
  /* Primary sūtra ID responsible for this slot (best-effort). */
  uint32_t     sutra_id;
} StemSlot;

static const StemSlot I_MASC[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "iH",   401002}, /* svaujasamauṭ */
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "I",    701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "ayaH", 601078}, /* eco'yavāyāvaḥ */
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "im",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "I",    701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "In",   604003}, /* nāmi (long ī) */
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "inA",  701012},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "iByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "iBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "aye",  703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "iByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "iByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "eH",   703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "iByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "iByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "eH",   703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "yoH",  601077}, /* yaṆ */
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "InAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "O",    703119},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "yoH",  601077},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "izu",  803059}, /* ādeśapratyayoḥ */
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "e",    703108},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "I",    701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "ayaH", 601078},
};

static const StemSlot I_NEUT[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "i",    206069}, /* nyāp prātipadikāt */
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "inI",  701073},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "Ini",  604003},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "i",    206069},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "inI",  701073},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "Ini",  604003},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "inA",  701012},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "iByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "iBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "ine",  701012},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "iByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "iByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "inaH", 701012},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "iByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "iByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "inaH", 701012},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "inoH", 701012},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "InAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "ini",  701012},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "inoH", 701012},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "izu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "i",    206069},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "inI",  701073},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "Ini",  604003},
};

static const StemSlot U_MASC[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "uH",   401002},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "U",    701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "avaH", 601078},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "um",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "U",    701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "Un",   604003},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "unA",  701012},
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

static const StemSlot U_NEUT[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "u",    206069},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "unI",  701073},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "Uni",  604003},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "u",    206069},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "unI",  701073},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "Uni",  604003},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "unA",  701012},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "uByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "uBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "une",  701012},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "uByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "uByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "unaH", 701012},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "uByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "uByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "unaH", 701012},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "unoH", 701012},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "UnAm", 604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "uni",  701012},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "unoH", 701012},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "uzu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "u",    206069},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "unI",  701073},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "Uni",  604003},
};

/* 8.4.1 raṣābhyāṃ no ṇaḥ — within the same pada, suffix `n` followed by
   a vowel becomes `R` (ṇ) when the stem contains a triggering r/f/z/F.
   This is a simplified but practically sufficient implementation for the
   common i/u-stem paradigms. */
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

/* Apply 8.2.66 + 8.3.15 final-s → visarga at end of pada. */
static void apply_final_visarga(char *form) {
  size_t fl = strlen(form);
  if (fl > 0 && form[fl - 1] == 's') {
    form[fl - 1] = 'H';
  }
}

static const StemSlot *slot_lookup(const StemSlot *table, ASH_Vibhakti vib,
                                   ASH_Vacana vac) {
  for (size_t i = 0; i < 24; i++) {
    if (table[i].vib == vib && table[i].vac == vac) return &table[i];
  }
  return NULL;
}

static bool stem_ends_in(const char *stem, char ch) {
  size_t n = stem ? strlen(stem) : 0;
  return n > 0 && stem[n - 1] == ch;
}

/* Common derivation routine: drop stem-final vowel, append slot ending,
   apply 8.4.1 ṇatva, apply 8.2.66+8.3.15 visarga. The base is the stem
   minus its final vowel (kept short to keep junction sandhi readable). */
static bool derive_with_slot(const char *stem_slp1, char drop_vowel,
                             const StemSlot *slot, PrakriyaCtx *ctx_out,
                             ASH_Linga li) {
  if (!stem_slp1 || !slot || !ctx_out) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0) return false;
  if (stem_slp1[n - 1] != drop_vowel) {
    /* Accept long-vowel inputs too (e.g. agni/agnI both i-stems). */
    char up = drop_vowel;
    if (drop_vowel == 'i' && stem_slp1[n - 1] != 'I') return false;
    if (drop_vowel == 'u' && stem_slp1[n - 1] != 'U') return false;
    (void)up;
  }
  prakriya_init_subanta(ctx_out, stem_slp1, li, slot->vib, slot->vac);
  ctx_out->term_count = 1;
  char base[TERM_VALUE_LEN] = {0};
  size_t base_len = (n > 0) ? n - 1 : 0;
  if (base_len + strlen(slot->ending) + 1 > sizeof(base)) return false;
  memcpy(base, stem_slp1, base_len);
  base[base_len] = '\0';
  char form[TERM_VALUE_LEN] = {0};
  snprintf(form, sizeof(form), "%s%s", base, slot->ending);
  apply_natva(stem_slp1, form);
  apply_final_visarga(form);
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, slot->sutra_id, stem_slp1, form,
                          "subanta paradigm slot");
  return true;
}

bool i_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_ends_in(stem_slp1, 'i') && !stem_ends_in(stem_slp1, 'I')) return false;
  const StemSlot *slot = slot_lookup(I_MASC, vib, vac);
  return derive_with_slot(stem_slp1, 'i', slot, ctx_out, ASH_PUMS);
}

bool i_stem_neut_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_ends_in(stem_slp1, 'i') && !stem_ends_in(stem_slp1, 'I')) return false;
  const StemSlot *slot = slot_lookup(I_NEUT, vib, vac);
  return derive_with_slot(stem_slp1, 'i', slot, ctx_out, ASH_NAPUMSAKA);
}

bool u_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_ends_in(stem_slp1, 'u') && !stem_ends_in(stem_slp1, 'U')) return false;
  const StemSlot *slot = slot_lookup(U_MASC, vib, vac);
  return derive_with_slot(stem_slp1, 'u', slot, ctx_out, ASH_PUMS);
}

bool u_stem_neut_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_ends_in(stem_slp1, 'u') && !stem_ends_in(stem_slp1, 'U')) return false;
  const StemSlot *slot = slot_lookup(U_NEUT, vib, vac);
  return derive_with_slot(stem_slp1, 'u', slot, ctx_out, ASH_NAPUMSAKA);
}
