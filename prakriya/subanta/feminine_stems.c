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

/* 8.4.1 raṣābhyāṃ no ṇaḥ + 8.4.2 atkupvāṅnumvyavāye'pi: replace `n`
   with ṇ only when intervening chars between trigger and n are in
   the allowed set (vowels, k/p-class, y/v/h, anusvāra). Dentals,
   palatals, retroflex non-ṇ, ś/s, l block the rule. */
static bool natva_is_blocker(char c) {
  switch (c) {
    case 't': case 'T': case 'd': case 'D':
    case 'c': case 'C': case 'j': case 'J': case 'Y':
    case 'w': case 'W': case 'q': case 'Q':
    case 'R': case 'S': case 's':
    case 'l':
      return true;
  }
  return false;
}

static void apply_natva(const char *stem, char *form) {
  (void)stem;
  bool seen = false;
  for (size_t i = 0; form[i]; i++) {
    char c = form[i];
    if (c == 'r' || c == 'f' || c == 'z' || c == 'F') {
      seen = true;
    } else if (natva_is_blocker(c)) {
      seen = false;
    } else if (seen && c == 'n' && form[i + 1] && varna_is_vowel(form[i + 1])) {
      form[i] = 'R';
      seen = false;
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

/* ── arciz-style z-final feminine consonant stem ───────────────────── */

typedef enum {
  Z_VOWEL,    /* arciz — preserve z before vowel */
  Z_VOICED,   /* arcir — z → r before voiced consonant (8.2.66 + 8.4.40) */
  Z_VISARGA,  /* arciH — z → H at pada end (8.2.66 + 8.3.15) */
  Z_DUAL,     /* arciHzu — saptami-bahu special */
} ZBaseKind;

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  ZBaseKind    base;
  const char  *suffix;
  uint32_t     sutra_id;
} ZSlot;

static const ZSlot Z_FEM[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  Z_VISARGA, "",    802066},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  Z_VOWEL,   "O",   701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, Z_VOWEL,   "aH",  401002},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  Z_VOWEL,   "am",  401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  Z_VOWEL,   "O",   701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, Z_VOWEL,   "aH",  401002},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  Z_VOWEL,   "A",   401002},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  Z_VOICED,  "ByAm",802040},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, Z_VOICED,  "BiH", 802040},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  Z_VOWEL,   "e",   401002},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  Z_VOICED,  "ByAm",802040},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, Z_VOICED,  "ByaH",802040},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  Z_VOWEL,   "aH",  401002},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  Z_VOICED,  "ByAm",802040},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, Z_VOICED,  "ByaH",802040},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  Z_VOWEL,   "aH",  401002},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  Z_VOWEL,   "oH",  401002},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, Z_VOWEL,   "Am",  604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  Z_VOWEL,   "i",   401002},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  Z_VOWEL,   "oH",  401002},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, Z_DUAL,    "Hzu", 803015},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  Z_VISARGA, "",    802066},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  Z_VOWEL,   "O",   701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, Z_VOWEL,   "aH",  401002},
};

static const ZSlot *z_slot_lookup(ASH_Vibhakti vib, ASH_Vacana vac) {
  for (size_t i = 0; i < 24; i++) {
    if (Z_FEM[i].vib == vib && Z_FEM[i].vac == vac) return &Z_FEM[i];
  }
  return NULL;
}

bool z_stem_fem_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                     PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  size_t n = strlen(stem_slp1);
  if (n < 1 || stem_slp1[n - 1] != 'z') return false;
  const ZSlot *slot = z_slot_lookup(vib, vac);
  if (!slot) return false;
  /* Build the four bases. */
  char vowel_base[64], voiced_base[64], visarga_base[64];
  size_t x_len = n - 1;
  /* VOWEL = full stem (z preserved): arciz */
  memcpy(vowel_base, stem_slp1, n);
  vowel_base[n] = '\0';
  /* VOICED = X + r: arcir */
  memcpy(voiced_base, stem_slp1, x_len);
  voiced_base[x_len] = 'r';
  voiced_base[x_len + 1] = '\0';
  /* VISARGA = X + H: arciH */
  memcpy(visarga_base, stem_slp1, x_len);
  visarga_base[x_len] = 'H';
  visarga_base[x_len + 1] = '\0';
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_STRI, vib, vac);
  ctx_out->term_count = 1;
  const char *base;
  switch (slot->base) {
    case Z_VOWEL:   base = vowel_base;   break;
    case Z_VOICED:  base = voiced_base;  break;
    case Z_VISARGA: base = visarga_base; break;
    case Z_DUAL:    base = visarga_base; break;
    default: return false;
  }
  char form[TERM_VALUE_LEN] = {0};
  if (slot->base == Z_DUAL) {
    /* saptami-bahu: arci + Hzu (visarga + zu). The visarga base
       already adds H, so we just need the zu part. */
    char x_only[64];
    memcpy(x_only, stem_slp1, x_len);
    x_only[x_len] = '\0';
    snprintf(form, sizeof(form), "%sHzu", x_only);
  } else {
    snprintf(form, sizeof(form), "%s%s", base, slot->suffix);
  }
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, slot->sutra_id, stem_slp1, form,
                          "z-stem feminine paradigm slot");
  return true;
}

/* ── BallAtakI-style long-ī masculine paradigm ─────────────────────── */

static const FemSlot II_MASC[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "IH",   401002},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "yO",   601077},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "yaH",  601077},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "Im",   401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "yO",   601077},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "In",   601068},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "yA",   601077},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "IByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "IBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "ye",   703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "IByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "IByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "yaH",  703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "IByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "IByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "yaH",  703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "yoH",  601077},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "yAm",  604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "I",    703119},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "yoH",  601077},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "Izu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "IH",   401002},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "yO",   601077},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "yaH",  601077},
};

bool ii_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                       PrakriyaCtx *ctx_out) {
  if (!stem_slp1) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0 || stem_slp1[n - 1] != 'I') return false;
  /* Look up slot in II_MASC. */
  const FemSlot *slot = NULL;
  for (size_t i = 0; i < 24; i++) {
    if (II_MASC[i].vib == vib && II_MASC[i].vac == vac) {
      slot = &II_MASC[i];
      break;
    }
  }
  if (!slot) return false;
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
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
                          "long-ī masculine paradigm slot");
  return true;
}

/* ── kim STRI (interrogative pronoun feminine) ─────────────────────── */

static const struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *form;     /* full surface form including k- prefix */
} KIM_FEM[] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "kA"},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "ke"},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "kAH"},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "kAm"},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "ke"},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "kAH"},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "kayA"},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "kAByAm"},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "kABiH"},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "kasyE"},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "kAByAm"},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "kAByaH"},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "kasyAH"},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "kAByAm"},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "kAByaH"},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "kasyAH"},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "kayoH"},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "kAsAm"},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "kasyAm"},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "kayoH"},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "kAsu"},
};

bool kim_stri_full(const char *stem_slp1, ASH_Vibhakti vib,
                   ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  if (strcmp(stem_slp1, "kim") != 0) return false;
  for (size_t i = 0; i < sizeof(KIM_FEM) / sizeof(KIM_FEM[0]); i++) {
    if (KIM_FEM[i].vib == vib && KIM_FEM[i].vac == vac) {
      prakriya_init_subanta(ctx_out, stem_slp1, ASH_STRI, vib, vac);
      ctx_out->term_count = 1;
      strncpy(ctx_out->terms[0].value, KIM_FEM[i].form, TERM_VALUE_LEN - 1);
      ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
      prakriya_log_transition(ctx_out, 700300, stem_slp1, KIM_FEM[i].form,
                              "kim feminine pronoun paradigm");
      return true;
    }
  }
  return false;
}

/* ── Root-noun feminine ū-stems (pitfprasU style) ─────────────────── */

/* Compound feminines whose final component is a monosyllabic root
   (prasū, brū, dhū, etc.). They inflect with mostly-vowel-initial
   endings (vam, vaH, ve, vi, vAm) instead of the vaDU paradigm's
   long-U endings. Closed seed list; extend as more stems show up. */
static const char *const UU_ROOT_FEM_STEMS[] = {
  "pitfprasU", "brU", "DU", NULL
};

static bool is_uu_root_fem(const char *stem) {
  if (!stem) return false;
  for (size_t i = 0; UU_ROOT_FEM_STEMS[i]; i++) {
    if (strcmp(stem, UU_ROOT_FEM_STEMS[i]) == 0) return true;
  }
  return false;
}

static const FemSlot UU_ROOT_FEM[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "UH",   401002},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "vO",   601077},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "vaH",  601077},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "vam",  401002},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "vO",   601077},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "vaH",  601077},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "vA",   601077},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "UByAm",401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "UBiH", 401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "ve",   703111},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  "UByAm",401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "UByaH",401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "vaH",  703111},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  "UByAm",401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "UByaH",401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "vaH",  703111},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "voH",  601077},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "vAm",  604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "vi",   703116},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  "voH",  601077},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "Uzu",  803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "UH",   401002},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  "vO",   601077},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "vaH",  601077},
};

bool uu_root_fem_full(const char *stem_slp1, ASH_Vibhakti vib,
                      ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  if (!stem_slp1) return false;
  if (!is_uu_root_fem(stem_slp1)) return false;
  size_t n = strlen(stem_slp1);
  if (n == 0 || stem_slp1[n - 1] != 'U') return false;
  return fem_derive(stem_slp1, UU_ROOT_FEM, vib, vac, ctx_out);
}

/* ── catur (numeral feminine "four") ──────────────────────────────── */

static const struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *form;
} CATUR_FEM[] = {
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "catasraH"},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "catasraH"},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "catasfBiH"},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "catasfByaH"},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, "catasfByaH"},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "catasfRAm"},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "catasfzu"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, "catasraH"},
};

bool catur_stri_full(const char *stem_slp1, ASH_Vibhakti vib,
                     ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  if (strcmp(stem_slp1, "catur") != 0) return false;
  /* Numerals like 'four' only inflect in plural (bahuvacana). */
  if (vac != ASH_BAHUVACANA) return false;
  for (size_t i = 0; i < sizeof(CATUR_FEM) / sizeof(CATUR_FEM[0]); i++) {
    if (CATUR_FEM[i].vib == vib && CATUR_FEM[i].vac == vac) {
      prakriya_init_subanta(ctx_out, stem_slp1, ASH_STRI, vib, vac);
      ctx_out->term_count = 1;
      strncpy(ctx_out->terms[0].value, CATUR_FEM[i].form, TERM_VALUE_LEN - 1);
      ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
      prakriya_log_transition(ctx_out, 700400, stem_slp1, CATUR_FEM[i].form,
                              "catur numeral feminine paradigm");
      return true;
    }
  }
  return false;
}
