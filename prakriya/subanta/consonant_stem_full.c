/* consonant_stem_full.c — Story 4.7 full consonant-stem paradigms.
   Covers an-stems (rAjan-type, masculine), as-stems (manas-type, neuter),
   and ṛ-stems (pitf-type, masculine). Legacy stub helpers in
   consonant_stems.c are kept for backwards compatibility. */

#include "consonant_stems.h"
#include <stdio.h>
#include <string.h>

/* ── an-stem (rAjan-style) ─────────────────────────────────────────── */

typedef enum {
  N_STRONG,    /* rAjAn — sarvanāmasthāna: a→A, n preserved */
  N_WEAK_CONS, /* rAja  — n dropped (8.2.7), suffix is consonant-initial */
  N_WEAK_VOW,  /* rAjY  — n+a both dropped (6.4.134), Y inserted */
  N_NLOPA,     /* rAj   — for prathama-eka where 8.2.7 strips n and the
                  8.2.27 a-elision yields the rAjA form via "A" suffix */
  N_RAW,       /* rAjan — sambodhana-eka, full stem */
} NBaseKind;

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  NBaseKind    base;
  const char  *suffix;
  uint32_t     sutra_id;
} NSlot;

static const NSlot N_MASC_SLOTS[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  N_NLOPA,    "A",    802007},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  N_STRONG,   "O",    604008},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, N_STRONG,   "aH",   604008},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  N_STRONG,   "am",   604008},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  N_STRONG,   "O",    604008},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, N_WEAK_VOW, "aH",   604134},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  N_WEAK_VOW, "A",    604134},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  N_WEAK_CONS,"ByAm", 802007},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, N_WEAK_CONS,"BiH",  802007},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  N_WEAK_VOW, "e",    604134},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  N_WEAK_CONS,"ByAm", 802007},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, N_WEAK_CONS,"ByaH", 802007},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  N_WEAK_VOW, "aH",   604134},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  N_WEAK_CONS,"ByAm", 802007},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, N_WEAK_CONS,"ByaH", 802007},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  N_WEAK_VOW, "aH",   604134},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  N_WEAK_VOW, "oH",   604134},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, N_WEAK_VOW, "Am",   604134},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  N_WEAK_VOW, "i",    604134},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  N_WEAK_VOW, "oH",   604134},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, N_WEAK_CONS,"su",   802007},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  N_RAW,      "",     401002},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  N_STRONG,   "O",    604008},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, N_STRONG,   "aH",   604008},
};

static const NSlot *n_slot_lookup(ASH_Vibhakti vib, ASH_Vacana vac) {
  for (size_t i = 0; i < 24; i++) {
    if (N_MASC_SLOTS[i].vib == vib && N_MASC_SLOTS[i].vac == vac) {
      return &N_MASC_SLOTS[i];
    }
  }
  return NULL;
}

/* For an "an"-final stem, build the four bases. The stem must end in
   "an"; otherwise this helper fails. The Y in WEAK_VOW reflects 8.2.79
   ujjvalakaṃ saṃyoge for the consonant cluster (technically jñ from
   raja+jña; we use SLP1 'Y' = ñ). */
static bool n_build_bases(const char *stem, char *strong, char *weak_cons,
                          char *weak_vow, char *raw, char *nlopa,
                          size_t buf_len) {
  size_t n = stem ? strlen(stem) : 0;
  if (n < 2) return false;
  if (stem[n - 2] != 'a' || stem[n - 1] != 'n') return false;
  /* X = stem minus final "an" */
  size_t x_len = n - 2;
  if (x_len + 3 > buf_len) return false;
  memcpy(raw, stem, n);     raw[n] = '\0';
  memcpy(strong, stem, x_len);
  strong[x_len] = 'A';
  strong[x_len + 1] = 'n';
  strong[x_len + 2] = '\0';
  memcpy(weak_cons, stem, x_len);
  weak_cons[x_len] = 'a';
  weak_cons[x_len + 1] = '\0';
  memcpy(weak_vow, stem, x_len);
  weak_vow[x_len] = 'Y';
  weak_vow[x_len + 1] = '\0';
  memcpy(nlopa, stem, x_len);
  nlopa[x_len] = '\0';
  return true;
}

bool an_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                       PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  char strong[64], weak_cons[64], weak_vow[64], raw[64], nlopa[64];
  if (!n_build_bases(stem_slp1, strong, weak_cons, weak_vow, raw, nlopa,
                     sizeof(strong))) return false;
  const NSlot *slot = n_slot_lookup(vib, vac);
  if (!slot) return false;
  const char *base;
  switch (slot->base) {
    case N_STRONG:    base = strong;    break;
    case N_WEAK_CONS: base = weak_cons; break;
    case N_WEAK_VOW:  base = weak_vow;  break;
    case N_NLOPA:     base = nlopa;     break;
    case N_RAW:       base = raw;       break;
    default: return false;
  }
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
  ctx_out->term_count = 1;
  char form[TERM_VALUE_LEN] = {0};
  snprintf(form, sizeof(form), "%s%s", base, slot->suffix);
  /* 8.2.66 + 8.3.15 final-s → visarga (no `s` finals here, but harmless). */
  size_t fl = strlen(form);
  if (fl > 0 && form[fl - 1] == 's') form[fl - 1] = 'H';
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, slot->sutra_id, stem_slp1, form,
                          "an-stem paradigm slot");
  return true;
}

/* ── as-stem neuter (manas-style) ──────────────────────────────────── */

typedef enum {
  AS_STRONG,   /* manAMs — for prathama/dvitiya/sambodhana bahuvacana neuter */
  AS_VOICED,   /* mano   — before voiced consonant suffixes (ByAm, BiH, ByaH) */
  AS_VOWEL,    /* manas  — before vowel suffixes (preserves s) */
  AS_VISARGA,  /* manaH  — for prathama/dvitiya/sambodhana ekavacana */
} AsBaseKind;

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  AsBaseKind   base;
  const char  *suffix;
  uint32_t     sutra_id;
} AsSlot;

static const AsSlot AS_NEUT_SLOTS[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  AS_VISARGA, "",    802066},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  AS_VOWEL,   "I",   701018},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, AS_STRONG,  "i",   601060},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  AS_VISARGA, "",    802066},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  AS_VOWEL,   "I",   701018},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, AS_STRONG,  "i",   601060},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  AS_VOWEL,   "A",   401002},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  AS_VOICED,  "ByAm",802066},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, AS_VOICED,  "BiH", 802066},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  AS_VOWEL,   "e",   401002},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  AS_VOICED,  "ByAm",802066},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, AS_VOICED,  "ByaH",802066},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  AS_VOWEL,   "aH",  401002},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  AS_VOICED,  "ByAm",802066},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, AS_VOICED,  "ByaH",802066},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  AS_VOWEL,   "aH",  401002},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  AS_VOWEL,   "oH",  401002},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, AS_VOWEL,   "Am",  604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  AS_VOWEL,   "i",   401002},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  AS_VOWEL,   "oH",  401002},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, AS_VISARGA, "su",  802066},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  AS_VISARGA, "",    802066},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  AS_VOWEL,   "I",   701018},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, AS_STRONG,  "i",   601060},
};

static const AsSlot *as_slot_lookup(ASH_Vibhakti vib, ASH_Vacana vac) {
  for (size_t i = 0; i < 24; i++) {
    if (AS_NEUT_SLOTS[i].vib == vib && AS_NEUT_SLOTS[i].vac == vac) {
      return &AS_NEUT_SLOTS[i];
    }
  }
  return NULL;
}

/* For an "as"-final stem (e.g. "manas"), build the four bases. */
static bool as_build_bases(const char *stem, char *strong, char *voiced,
                           char *vowel, char *visarga, size_t buf_len) {
  size_t n = stem ? strlen(stem) : 0;
  if (n < 2) return false;
  if (stem[n - 2] != 'a' || stem[n - 1] != 's') return false;
  /* X = stem minus final "as" */
  size_t x_len = n - 2;
  if (x_len + 4 > buf_len) return false;
  /* STRONG: X + AMs (a→A, anusvāra inserted before s, then s preserved) */
  memcpy(strong, stem, x_len);
  strong[x_len + 0] = 'A';
  strong[x_len + 1] = 'M';
  strong[x_len + 2] = 's';
  strong[x_len + 3] = '\0';
  /* VOICED: X + o (final as → o by 6.1.113-114 before voiced) */
  memcpy(voiced, stem, x_len);
  voiced[x_len] = 'o';
  voiced[x_len + 1] = '\0';
  /* VOWEL: full stem with s preserved before vowel suffixes */
  memcpy(vowel, stem, n);
  vowel[n] = '\0';
  /* VISARGA: X + aH (final s → H at pada boundary) */
  memcpy(visarga, stem, x_len);
  visarga[x_len + 0] = 'a';
  visarga[x_len + 1] = 'H';
  visarga[x_len + 2] = '\0';
  return true;
}

bool as_stem_neut_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                       PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  char strong[64], voiced[64], vowel[64], visarga[64];
  if (!as_build_bases(stem_slp1, strong, voiced, vowel, visarga,
                      sizeof(strong))) return false;
  const AsSlot *slot = as_slot_lookup(vib, vac);
  if (!slot) return false;
  const char *base;
  switch (slot->base) {
    case AS_STRONG:  base = strong;  break;
    case AS_VOICED:  base = voiced;  break;
    case AS_VOWEL:   base = vowel;   break;
    case AS_VISARGA: base = visarga; break;
    default: return false;
  }
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_NAPUMSAKA, vib, vac);
  ctx_out->term_count = 1;
  char form[TERM_VALUE_LEN] = {0};
  snprintf(form, sizeof(form), "%s%s", base, slot->suffix);
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, slot->sutra_id, stem_slp1, form,
                          "as-stem paradigm slot");
  return true;
}

/* ── ṛ-stem masculine (pitf-style) ─────────────────────────────────── */

typedef enum {
  R_GUNA,      /* pitar — guṇa form (f → ar), sarvanāmasthāna and some
                  vowel-initial endings (saptami-eka). */
  R_WEAK,      /* pitf  — keeps ṛ, before consonant-initial suffixes. */
  R_DROP_F,    /* pit   — f dropped before vowel-initial suffixes. */
  R_LONG,      /* pitF  — long ṝ for dvitīyā-bahu and ṣaṣṭhī-bahu. */
  R_FINAL_A,   /* pitA  — f → A, prathama-eka with no suffix. */
  R_VOC,       /* pita  — f → a, sambodhana-eka. */
} RBaseKind;

typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  RBaseKind    base;
  const char  *suffix;
  uint32_t     sutra_id;
} RSlot;

static const RSlot R_MASC_SLOTS[24] = {
  {ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  R_FINAL_A,"",      701094},
  {ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  R_GUNA,   "O",     701096},
  {ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, R_GUNA,   "aH",    701096},
  {ASH_DVITIYA_VIB,    ASH_EKAVACANA,  R_GUNA,   "am",    701096},
  {ASH_DVITIYA_VIB,    ASH_DVIVACANA,  R_GUNA,   "O",     701096},
  {ASH_DVITIYA_VIB,    ASH_BAHUVACANA, R_LONG,   "n",     604003},
  {ASH_TRITIYA_VIB,    ASH_EKAVACANA,  R_DROP_F, "rA",    701102},
  {ASH_TRITIYA_VIB,    ASH_DVIVACANA,  R_WEAK,   "ByAm",  401002},
  {ASH_TRITIYA_VIB,    ASH_BAHUVACANA, R_WEAK,   "BiH",   401002},
  {ASH_CATURTHI_VIB,   ASH_EKAVACANA,  R_DROP_F, "re",    701102},
  {ASH_CATURTHI_VIB,   ASH_DVIVACANA,  R_WEAK,   "ByAm",  401002},
  {ASH_CATURTHI_VIB,   ASH_BAHUVACANA, R_WEAK,   "ByaH",  401002},
  {ASH_PANCAMI_VIB,    ASH_EKAVACANA,  R_DROP_F, "uH",    701102},
  {ASH_PANCAMI_VIB,    ASH_DVIVACANA,  R_WEAK,   "ByAm",  401002},
  {ASH_PANCAMI_VIB,    ASH_BAHUVACANA, R_WEAK,   "ByaH",  401002},
  {ASH_SHASTHI_VIB,    ASH_EKAVACANA,  R_DROP_F, "uH",    701102},
  {ASH_SHASTHI_VIB,    ASH_DVIVACANA,  R_DROP_F, "roH",   701102},
  {ASH_SHASTHI_VIB,    ASH_BAHUVACANA, R_LONG,   "RAm",   604003},
  {ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  R_GUNA,   "i",     701096},
  {ASH_SAPTAMI_VIB,    ASH_DVIVACANA,  R_DROP_F, "roH",   701102},
  {ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, R_WEAK,   "zu",    803059},
  {ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  R_VOC,    "H",     701096},
  {ASH_SAMBODHANA_VIB, ASH_DVIVACANA,  R_GUNA,   "O",     701096},
  {ASH_SAMBODHANA_VIB, ASH_BAHUVACANA, R_GUNA,   "aH",    701096},
};

static const RSlot *r_slot_lookup(ASH_Vibhakti vib, ASH_Vacana vac) {
  for (size_t i = 0; i < 24; i++) {
    if (R_MASC_SLOTS[i].vib == vib && R_MASC_SLOTS[i].vac == vac) {
      return &R_MASC_SLOTS[i];
    }
  }
  return NULL;
}

/* For an "f" (ṛ)-final stem (e.g. "pitf"), build all six bases. */
static bool r_build_bases(const char *stem, char *guna, char *weak,
                          char *drop_f, char *long_base, char *final_a,
                          char *voc, size_t buf_len) {
  size_t n = stem ? strlen(stem) : 0;
  if (n < 2) return false;
  if (stem[n - 1] != 'f') return false;
  size_t x_len = n - 1;
  if (x_len + 3 > buf_len) return false;
  /* GUNA: X + ar (ṛ → ar by 7.1.96) */
  memcpy(guna, stem, x_len);
  guna[x_len] = 'a';
  guna[x_len + 1] = 'r';
  guna[x_len + 2] = '\0';
  /* WEAK: full stem (keeps ṛ) */
  memcpy(weak, stem, n);
  weak[n] = '\0';
  /* DROP_F: X (drop the final ṛ) */
  memcpy(drop_f, stem, x_len);
  drop_f[x_len] = '\0';
  /* LONG: X + F (long ṝ) */
  memcpy(long_base, stem, x_len);
  long_base[x_len] = 'F';
  long_base[x_len + 1] = '\0';
  /* FINAL_A: X + A (ṛ → ā for prathama-eka) */
  memcpy(final_a, stem, x_len);
  final_a[x_len] = 'A';
  final_a[x_len + 1] = '\0';
  /* VOC: X + a (short a, sambodhana-eka) */
  memcpy(voc, stem, x_len);
  voc[x_len] = 'a';
  voc[x_len + 1] = '\0';
  return true;
}

bool r_stem_masc_full(const char *stem_slp1, ASH_Vibhakti vib, ASH_Vacana vac,
                      PrakriyaCtx *ctx_out) {
  if (!stem_slp1 || !ctx_out) return false;
  char guna[64], weak[64], drop_f[64], long_base[64], final_a[64], voc[64];
  if (!r_build_bases(stem_slp1, guna, weak, drop_f, long_base, final_a, voc,
                     sizeof(guna))) return false;
  const RSlot *slot = r_slot_lookup(vib, vac);
  if (!slot) return false;
  const char *base;
  switch (slot->base) {
    case R_GUNA:    base = guna;      break;
    case R_WEAK:    base = weak;      break;
    case R_DROP_F:  base = drop_f;    break;
    case R_LONG:    base = long_base; break;
    case R_FINAL_A: base = final_a;   break;
    case R_VOC:     base = voc;       break;
    default: return false;
  }
  prakriya_init_subanta(ctx_out, stem_slp1, ASH_PUMS, vib, vac);
  ctx_out->term_count = 1;
  char form[TERM_VALUE_LEN] = {0};
  snprintf(form, sizeof(form), "%s%s", base, slot->suffix);
  /* 8.2.66 + 8.3.15 final-s → visarga */
  size_t fl = strlen(form);
  if (fl > 0 && form[fl - 1] == 's') form[fl - 1] = 'H';
  strncpy(ctx_out->terms[0].value, form, TERM_VALUE_LEN - 1);
  ctx_out->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  prakriya_log_transition(ctx_out, slot->sutra_id, stem_slp1, form,
                          "f-stem paradigm slot");
  return true;
}
