/* lat_bhvadi.c — basic laT derivation helpers */
#include "lat_bhvadi.h"
#include "lakara.h"
#include "vikaranas.h"
#include "varna.h"
#include <string.h>

static void copy_clean_root(const char *src, char *dst, size_t dst_len) {
  size_t pos = 0;
  if (!src || !dst || dst_len == 0) return;
  dst[0] = '\0';
  for (size_t i = 0; src[i] != '\0' && pos + 1 < dst_len; i++) {
    if (src[i] == '~' || src[i] == '^') continue;
    dst[pos++] = src[i];
  }
  dst[pos] = '\0';
}

static void replace_first_vowel(char *root, bool vrddhi) {
  if (!root) return;
  for (size_t i = 0; root[i] != '\0'; i++) {
    if (root[i] == 'a' || root[i] == 'i' || root[i] == 'u' || root[i] == 'f' || root[i] == 'U') {
      root[i] = vrddhi ? varna_vrddhi(root[i]) : varna_guna(root[i]);
      return;
    }
  }
}

static bool append_with_vowel_sandhi(char *stem, size_t stem_len, const char *vik) {
  size_t used;
  if (!stem || !vik || stem_len == 0) return false;
  used = strlen(stem);
  if (used == 0) return false;
  if (strcmp(vik, "a") == 0) {
    char final = stem[used - 1];
    if (final == 'o' || final == 'O') {
      stem[used - 1] = '\0';
      return strncat(stem, final == 'o' ? "ava" : "Ava", stem_len - strlen(stem) - 1) != NULL;
    }
    if (final == 'e' || final == 'E') {
      stem[used - 1] = '\0';
      return strncat(stem, final == 'e' ? "aya" : "Aya", stem_len - strlen(stem) - 1) != NULL;
    }
  }
  if (used + strlen(vik) + 1 > stem_len) return false;
  strncat(stem, vik, stem_len - used - 1);
  return true;
}

static bool apply_class_transform(const char *clean_root_in, int gana,
                                  char *stem, size_t stem_len,
                                  char *after_class, size_t after_class_len,
                                  uint32_t *vik_sutra,
                                  bool *used_guna, bool *used_ec_ay) {
  char vik[16] = {0};
  size_t n;
  if (!clean_root_in || !stem || stem_len == 0 || !after_class || after_class_len == 0)
    return false;
  if (!vikarana_for_gana(gana, vik, sizeof(vik))) return false;
  strncpy(stem, clean_root_in, stem_len - 1);
  stem[stem_len - 1] = '\0';
  n = strlen(stem);
  if (n == 0) return false;

  *used_guna = false;
  *used_ec_ay = false;
  switch (gana) {
    case 4: *vik_sutra = 301069; break;
    case 6: *vik_sutra = 301077; break;
    case 10: *vik_sutra = 301025; break;
    case 1:
    default: *vik_sutra = 301068; break;
  }

  if (gana == 1 && n >= 2 && stem[n - 2] == 'a' && stem[n - 1] == 'm') {
    stem[n - 2] = 'a';
    stem[n - 1] = 'c';
    if (n + 2 >= stem_len) return false;
    stem[n] = 'C';
    stem[n + 1] = '\0';
  } else if (gana == 4) {
    for (size_t i = 0; stem[i] != '\0'; i++) {
      if (stem[i] == 'i') { stem[i] = 'I'; break; }
    }
  } else if (gana == 10 && n > 0 && stem[n - 1] == 'u') {
    stem[n - 1] = 'o';
    *used_guna = true;
  } else if (gana == 10 && stem[0] == 'c' && stem[1] == 'u') {
    stem[1] = 'o';
    *used_guna = true;
  } else if (gana_uses_vrddhi(gana)) {
    replace_first_vowel(stem, true);
    *used_guna = true;
  } else if (gana_uses_guna(gana)) {
    replace_first_vowel(stem, false);
    *used_guna = true;
  }

  /* Snapshot the post-class, pre-vikarana stem so callers can log it. */
  strncpy(after_class, stem, after_class_len - 1);
  after_class[after_class_len - 1] = '\0';

  n = strlen(stem);
  if (n > 0 && (stem[n - 1] == 'o' || stem[n - 1] == 'O' || stem[n - 1] == 'e' || stem[n - 1] == 'E')) {
    *used_ec_ay = true;
  }
  return append_with_vowel_sandhi(stem, stem_len, vik);
}

static void set_single_term(PrakriyaCtx *ctx, const char *value) {
  if (!ctx || !value) return;
  ctx->term_count = 1;
  strncpy(ctx->terms[0].value, value, TERM_VALUE_LEN - 1);
  ctx->terms[0].value[TERM_VALUE_LEN - 1] = '\0';
}

static void log_single_term_change(PrakriyaCtx *ctx, uint32_t sutra_id,
                                   const char *before, const char *after,
                                   const char *desc) {
  if (!ctx || !before || !after) return;
  prakriya_log_transition(ctx, sutra_id, before, after, desc);
  set_single_term(ctx, after);
}

bool lat_bhvadi_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                           ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out) {
  const TingEntry *t;
  char clean_root[64] = {0};
  char after_class[64] = {0};
  char stem[64] = {0};
  char form[128] = {0};
  uint32_t vik_sutra = 0;
  bool used_guna = false;
  bool used_ec_ay = false;
  if (!dhatu_slp1 || !ctx_out) return false;
  t = ting_get(ASH_LAT, p, v, pd);
  if (!t) return false;
  prakriya_init_tinanta(ctx_out, dhatu_slp1, gana, ASH_LAT, p, v, pd);
  copy_clean_root(dhatu_slp1, clean_root, sizeof(clean_root));
  if (!apply_class_transform(clean_root, gana, stem, sizeof(stem),
                             after_class, sizeof(after_class),
                             &vik_sutra, &used_guna, &used_ec_ay)) {
    return false;
  }
  /* Order of logged steps:
     1. guṇa/vṛddhi (clean_root → after_class) when class actually mutated the stem
     2. vikaraṇa assignment + concatenation (after_class → stem)
     3. ec→ay sandhi at root+vikaraṇa boundary, if it fired
     4. tiṅ assignment (stem → form) */
  if (used_guna && strcmp(clean_root, after_class) != 0) {
    log_single_term_change(ctx_out, 703084, clean_root, after_class,
                           "sArvadhAtukArdhadhAtukayoH");
  }
  log_single_term_change(ctx_out, vik_sutra, after_class, stem,
                         "vikaraRa assignment");
  if (used_ec_ay) {
    log_single_term_change(ctx_out, 601078, after_class, stem, "eco'yavAyAvaH");
  }
  if (strlen(stem) + strlen(t->clean) + 1 > sizeof(form)) return false;
  strcpy(form, stem);
  strcat(form, t->clean);
  log_single_term_change(ctx_out, 304078, stem, form, "tiN assignment");
  return true;
}

bool lat_bhvadi_derive(const char *dhatu_slp1, int gana, ASH_Purusha p,
                       ASH_Vacana v, ASH_Pada pd, char *out_slp1, size_t out_len) {
  PrakriyaCtx ctx = {0};
  if (!out_slp1 || out_len == 0) return false;
  if (!lat_bhvadi_derive_ctx(dhatu_slp1, gana, p, v, pd, &ctx)) return false;
  prakriya_current_form(&ctx, out_slp1, out_len);
  return true;
}
