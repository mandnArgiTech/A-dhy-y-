/* pronouns.c — sarvanāma (pronoun) PUMS and NAPUMSAKA paradigms for
   tad, yad, etad, kim, sarva. Sarvanāma stems deviate from the
   regular a-stem in five slots: 1bahu (-e instead of -AH), 4eka
   (-asmE for -Aya), 5eka (-asmAt for -At), 6bahu (-ezAm for -ANAm),
   and 7eka (-asmin for -e). The nom-eka of tad/etad uses a stem
   alternation: "sa" for tad, "eza" for etad.

   Implementation strategy: a generic SARVA_MASC table holds the 24
   surface forms of "sarva" (stem-final = a). For other stems we
   substitute the prefix and special-case the nom-eka. */

#include "pronouns.h"
#include "../context.h"
#include "sandhi_natva.h"
#include <string.h>
#include <stdio.h>

/* Pronominal endings (with stem-final a from a-stem already merged).
   Each row is what gets appended to the bare prefix (e.g. "ta", "ya",
   "ka", "sarva"). For nom-eka, the stem itself is replaced by an
   irregular form (saH for tad, ezaH for etad), so the table here
   leaves nom-eka as "aH" and the caller overrides as needed. */
typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *suffix;  /* appended after the prefix */
} SarvanamaSlot;

static const SarvanamaSlot SARVA_MASC_SUF[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,    "aH"},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,    "O"},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA,   "e"},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,    "am"},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,    "O"},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA,   "An"},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,    "ena"},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,    "AByAm"},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA,   "EH"},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,    "asmE"},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,    "AByAm"},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA,   "eByaH"},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,    "asmAt"},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,    "AByAm"},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA,   "eByaH"},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,    "asya"},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,    "ayoH"},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA,   "ezAm"},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,    "asmin"},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,    "ayoH"},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA,   "ezu"},
  /* Pronouns have no vocative; emit empty strings. */
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,    ""},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,    ""},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA,   ""},
};

static const SarvanamaSlot SARVA_NEUT_SUF[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,    "at"},     /* tad → tat */
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,    "e"},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA,   "Ani"},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,    "at"},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,    "e"},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA,   "Ani"},
  /* III-VII: same as PUMS */
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,    "ena"},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,    "AByAm"},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA,   "EH"},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,    "asmE"},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,    "AByAm"},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA,   "eByaH"},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,    "asmAt"},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,    "AByAm"},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA,   "eByaH"},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,    "asya"},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,    "ayoH"},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA,   "ezAm"},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,    "asmin"},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,    "ayoH"},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA,   "ezu"},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,    ""},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,    ""},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA,   ""},
};

static const SarvanamaSlot *slot_lookup(const SarvanamaSlot *table,
                                        ASH_Vibhakti vib, ASH_Vacana vac) {
  for (int i = 0; i < 24; i++) {
    if (table[i].vib == vib && table[i].vac == vac) return &table[i];
  }
  return NULL;
}

/* Translate a pronoun upadeśa (tad, yad, etad, kim, sarva) into the
   stem prefix used for inflection (ta, ya, eta, ka, sarva). */
static const char *pronoun_prefix(const char *upadesa) {
  if (!upadesa) return NULL;
  if (strcmp(upadesa, "tad") == 0) return "t";
  if (strcmp(upadesa, "yad") == 0) return "y";
  if (strcmp(upadesa, "etad") == 0) return "et";
  if (strcmp(upadesa, "kim") == 0) return "k";
  if (strcmp(upadesa, "sarva") == 0) return "sarv";
  return NULL;
}

/* Some pronouns have an irregular nom-eka where the prefix changes.
   tad → sa (per 7.2.106 tyad-Adīnām asaḥ), etad → eza. */
static const char *pronoun_nom_eka_override(const char *upadesa,
                                            ASH_Linga li) {
  if (!upadesa) return NULL;
  if (li == ASH_PUMS) {
    if (strcmp(upadesa, "tad") == 0)  return "saH";
    if (strcmp(upadesa, "etad") == 0) return "ezaH";
  }
  return NULL;
}

bool sarvanama_masc_full(const char *stem_slp1, ASH_Vibhakti vib,
                         ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  const char *prefix = pronoun_prefix(stem_slp1);
  if (!prefix) return false;
  const SarvanamaSlot *slot = slot_lookup(SARVA_MASC_SUF, vib, vac);
  if (!slot) return false;

  prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
  ctx_out->term_count = 1;

  /* Nom-eka may override the stem (tad → saH, etad → ezaH). */
  if (vib == ASH_PRATHAMA_VIB && vac == ASH_EKAVACANA) {
    const char *over = pronoun_nom_eka_override(stem_slp1, ASH_PUMS);
    if (over) {
      strncpy(ctx_out->terms[0].value, over, TERM_VALUE_LEN - 1);
      ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
      prakriya_log_transition(ctx_out, 702106, stem_slp1, over,
                              "tyad-AdInAm asaH");
      return true;
    }
  }

  char form[64];
  snprintf(form, sizeof(form), "%s%s", prefix, slot->suffix);
  /* 8.4.1 raṣābhyāṃ no ṇaḥ: e.g. sarvena → sarveRa. */
  sandhi_apply_natva(form);
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, 700301, stem_slp1, form,
                          "sarvanAma masc paradigm");
  return true;
}

bool sarvanama_neut_full(const char *stem_slp1, ASH_Vibhakti vib,
                         ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  const char *prefix = pronoun_prefix(stem_slp1);
  if (!prefix) return false;
  const SarvanamaSlot *slot = slot_lookup(SARVA_NEUT_SUF, vib, vac);
  if (!slot) return false;

  prakriya_init_subanta(ctx_out, stem_slp1, ASH_NAPUMSAKA, vib, vac);
  ctx_out->term_count = 1;

  char form[64];
  snprintf(form, sizeof(form), "%s%s", prefix, slot->suffix);
  /* kim NAPUMSAKA nom/acc-eka is "kim" not "kat"; sarva NAPUMSAKA
     nom/acc-eka uses the regular a-stem ending "am" (the -at suffix is
     a tyad-class feature for tad/yad/etad/kim only). */
  if ((vib == ASH_PRATHAMA_VIB || vib == ASH_DVITIYA_VIB) &&
      vac == ASH_EKAVACANA) {
    if (strcmp(stem_slp1, "kim") == 0) {
      strncpy(form, "kim", sizeof(form) - 1);
      form[sizeof(form) - 1] = '\0';
    } else if (strcmp(stem_slp1, "sarva") == 0) {
      snprintf(form, sizeof(form), "%sam", prefix);
    }
  }
  /* 8.4.1 raṣābhyāṃ no ṇaḥ: e.g. sarvAni → sarvARi (ṇatva). */
  sandhi_apply_natva(form);
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, 700302, stem_slp1, form,
                          "sarvanAma neut paradigm");
  return true;
}

bool pronoun_is_sarvanama(const char *upadesa) {
  return pronoun_prefix(upadesa) != NULL;
}
