/* numerals.c — sankhyā (numeral) paradigms for dvi (2), tri (3),
   catur (4), paJcan (5), zaz (6), saptan (7), azwan (8), navan (9),
   daSan (10). All except dvi/tri/catur are liṅga-invariant in BV.

   Numerals 5-10 only have bahuvacana surface forms (the cardinality
   is bound into the stem). Their paradigm is the same shape for
   PUMS / NAPUMSAKA / STRI. dvi has only dvivacana (dual). tri has
   PUMS / NAPUMSAKA / STRI variants in bahuvacana. catur similar. */

#include "numerals.h"
#include "../context.h"
#include "sandhi_natva.h"
#include <string.h>
#include <stdio.h>

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *form;
} NumSlot;

/* dvi — dual only. */
static const NumSlot DVI_MASC[24] = {
  {ASH_PRATHAMA_VIB, ASH_DVIVACANA, "dvO"},
  {ASH_DVITIYA_VIB,  ASH_DVIVACANA, "dvO"},
  {ASH_TRITIYA_VIB,  ASH_DVIVACANA, "dvAByAm"},
  {ASH_CATURTHI_VIB, ASH_DVIVACANA, "dvAByAm"},
  {ASH_PANCAMI_VIB,  ASH_DVIVACANA, "dvAByAm"},
  {ASH_SHASTHI_VIB,  ASH_DVIVACANA, "dvayoH"},
  {ASH_SAPTAMI_VIB,  ASH_DVIVACANA, "dvayoH"},
  /* sentinel */
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA, ""},
};

static const NumSlot DVI_NEUT_FEM[24] = {
  {ASH_PRATHAMA_VIB, ASH_DVIVACANA, "dve"},
  {ASH_DVITIYA_VIB,  ASH_DVIVACANA, "dve"},
  {ASH_TRITIYA_VIB,  ASH_DVIVACANA, "dvAByAm"},
  {ASH_CATURTHI_VIB, ASH_DVIVACANA, "dvAByAm"},
  {ASH_PANCAMI_VIB,  ASH_DVIVACANA, "dvAByAm"},
  {ASH_SHASTHI_VIB,  ASH_DVIVACANA, "dvayoH"},
  {ASH_SAPTAMI_VIB,  ASH_DVIVACANA, "dvayoH"},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA, ""},
};

/* tri — plural only. Three liṅgas. */
static const NumSlot TRI_MASC[24] = {
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "trayaH"},
  {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "trIn"},
  {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "triBiH"},
  {ASH_CATURTHI_VIB, ASH_BAHUVACANA, "triByaH"},
  {ASH_PANCAMI_VIB,  ASH_BAHUVACANA, "triByaH"},
  {ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "trayARAm"},
  {ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "trizu"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, ""},
};

static const NumSlot TRI_NEUT[24] = {
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "trIRi"},
  {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "trIRi"},
  {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "triBiH"},
  {ASH_CATURTHI_VIB, ASH_BAHUVACANA, "triByaH"},
  {ASH_PANCAMI_VIB,  ASH_BAHUVACANA, "triByaH"},
  {ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "trayARAm"},
  {ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "trizu"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, ""},
};

static const NumSlot TRI_FEM[24] = {
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "tisraH"},
  {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "tisraH"},
  {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "tisfBiH"},
  {ASH_CATURTHI_VIB, ASH_BAHUVACANA, "tisfByaH"},
  {ASH_PANCAMI_VIB,  ASH_BAHUVACANA, "tisfByaH"},
  {ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "tisfRAm"},
  {ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "tisfzu"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, ""},
};

/* catur — plural only. Three liṅgas. */
static const NumSlot CATUR_MASC[24] = {
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "catvAraH"},
  {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "caturaH"},
  {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "caturBiH"},
  {ASH_CATURTHI_VIB, ASH_BAHUVACANA, "caturByaH"},
  {ASH_PANCAMI_VIB,  ASH_BAHUVACANA, "caturByaH"},
  {ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "caturRAm"},
  {ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "caturzu"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, ""},
};

static const NumSlot CATUR_NEUT[24] = {
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "catvAri"},
  {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "catvAri"},
  {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "caturBiH"},
  {ASH_CATURTHI_VIB, ASH_BAHUVACANA, "caturByaH"},
  {ASH_PANCAMI_VIB,  ASH_BAHUVACANA, "caturByaH"},
  {ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "caturRAm"},
  {ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "caturzu"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, ""},
};

/* Generic "an-stem numeral" template: paJcan, saptan, azwan, navan,
   daSan. The shared paradigm prepends the stem-without-final-n to the
   slot suffix. azwan deviates in two slots (alternating azwa/azwO).
   Numerals 5-10 are liṅga-invariant in BAHUVACANA. */
typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *suffix;  /* appended after stem-minus-final-n */
} NumSuffixSlot;

static const NumSuffixSlot AN_NUM_SUF[24] = {
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, ""},
  {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, ""},
  {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "BiH"},
  {ASH_CATURTHI_VIB, ASH_BAHUVACANA, "ByaH"},
  {ASH_PANCAMI_VIB,  ASH_BAHUVACANA, "ByaH"},
  /* 6.4.7 / 7.1.55 nāmi: 'a' before nām is lengthened. */
  {ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "AnAm"},
  /* The 7bahu in -su survives, with the stem's 'a' kept short. */
  {ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "su"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, ""},
};

static const NumSlot *lookup_table(const NumSlot *table,
                                   ASH_Vibhakti vib, ASH_Vacana vac) {
  for (int i = 0; i < 24; i++) {
    if (table[i].form == NULL) continue;
    if (table[i].vib == vib && table[i].vac == vac && table[i].form[0] != '\0')
      return &table[i];
    /* The dvi/tri/catur tables are sparse: stop scanning at first empty. */
    if (table[i].vib == vib && table[i].vac == vac) return NULL;
  }
  return NULL;
}

bool numeral_is_known(const char *upadesa) {
  if (!upadesa) return false;
  return strcmp(upadesa, "dvi")    == 0 ||
         strcmp(upadesa, "tri")    == 0 ||
         strcmp(upadesa, "catur")  == 0 ||
         strcmp(upadesa, "paYcan") == 0 ||
         strcmp(upadesa, "zaz")    == 0 ||
         strcmp(upadesa, "saptan") == 0 ||
         strcmp(upadesa, "azwan")  == 0 ||
         strcmp(upadesa, "navan")  == 0 ||
         strcmp(upadesa, "daSan")  == 0;
}

static bool emit_form(PrakriyaCtx *ctx_out, const char *stem,
                      ASH_Linga li, ASH_Vibhakti vib, ASH_Vacana vac,
                      const char *surface) {
  if (!surface) return false;
  prakriya_init_subanta(ctx_out, stem, li, vib, vac);
  ctx_out->term_count = 1;
  strncpy(ctx_out->terms[0].value, surface, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, 700310, stem, surface, "numeral paradigm");
  return true;
}

/* Handle 5-10 (paJcan, saptan, etc.). zaz handled separately. */
static bool an_numeral_form(const char *stem, ASH_Linga li,
                            ASH_Vibhakti vib, ASH_Vacana vac,
                            PrakriyaCtx *ctx_out) {
  size_t sl = strlen(stem);
  /* All these end in "n"; drop it. */
  if (sl < 2 || stem[sl - 1] != 'n') return false;
  char prefix[32];
  size_t pl = sl - 1;
  if (pl >= sizeof(prefix)) return false;
  memcpy(prefix, stem, pl);
  prefix[pl] = '\0';

  for (int i = 0; i < 24; i++) {
    if (AN_NUM_SUF[i].vib == vib && AN_NUM_SUF[i].vac == vac) {
      char surface[64];
      const char *suf = AN_NUM_SUF[i].suffix;
      /* azwan has alternate forms in 1/2bahu and 7bahu. We emit the
         primary form (azwa / azwasu). */
      if (suf[0] == '\0') {
        /* Nom/acc: stem-without-final-n only. */
        snprintf(surface, sizeof(surface), "%s", prefix);
      } else if (suf[0] == 'A' && pl > 0 && prefix[pl - 1] == 'a') {
        /* 6.1.101 savarṇa-dīrgha at stem-final 'a' + 'A' of -AnAm. */
        char shortened[32];
        memcpy(shortened, prefix, pl - 1);
        shortened[pl - 1] = '\0';
        snprintf(surface, sizeof(surface), "%s%s", shortened, suf);
      } else {
        snprintf(surface, sizeof(surface), "%s%s", prefix, suf);
      }
      /* 8.4.1 ṇatva (e.g. saptAnAm → keep n, not ṇ — n after a stays). */
      sandhi_apply_natva(surface);
      return emit_form(ctx_out, stem, li, vib, vac, surface);
    }
  }
  return false;
}

/* zaz (six) — completely irregular. */
static const NumSlot ZAZ[24] = {
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "zaw"},
  {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "zaw"},
  {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "zaqBiH"},
  {ASH_CATURTHI_VIB, ASH_BAHUVACANA, "zaqByaH"},
  {ASH_PANCAMI_VIB,  ASH_BAHUVACANA, "zaqByaH"},
  {ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "zaRRAm"},
  {ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "zawsu"},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, ""},
};

bool numeral_full(const char *stem, ASH_Linga li, ASH_Vibhakti vib,
                  ASH_Vacana vac, PrakriyaCtx *ctx_out) {
  if (!stem || !ctx_out) return false;
  const NumSlot *table = NULL;
  if (strcmp(stem, "dvi") == 0) {
    table = (li == ASH_PUMS) ? DVI_MASC : DVI_NEUT_FEM;
  } else if (strcmp(stem, "tri") == 0) {
    table = (li == ASH_PUMS) ? TRI_MASC :
            (li == ASH_NAPUMSAKA ? TRI_NEUT : TRI_FEM);
  } else if (strcmp(stem, "catur") == 0) {
    if (li == ASH_STRI) {
      /* The STRI catur paradigm is in feminine_stems.c; defer there. */
      return false;
    }
    table = (li == ASH_PUMS) ? CATUR_MASC : CATUR_NEUT;
  } else if (strcmp(stem, "zaz") == 0) {
    table = ZAZ;
  } else if (strcmp(stem, "paYcan") == 0 || strcmp(stem, "saptan") == 0 ||
             strcmp(stem, "azwan")  == 0 || strcmp(stem, "navan")  == 0 ||
             strcmp(stem, "daSan")  == 0) {
    return an_numeral_form(stem, li, vib, vac, ctx_out);
  } else {
    return false;
  }
  const NumSlot *slot = lookup_table(table, vib, vac);
  if (!slot) return false;
  return emit_form(ctx_out, stem, li, vib, vac, slot->form);
}
