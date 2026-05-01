/* lat_bhvadi.c — basic laT derivation helpers */
#include "lat_bhvadi.h"
#include "lakara.h"
#include "vikaranas.h"
#include "varna.h"
#include "anubandha.h"
#include "samjna.h"
#include <string.h>

/* Reduce a dhātupāṭha upadeśa form to the clean root used in derivation.
   Inputs come in two flavours:
   - Bare clean roots (e.g. "BU", "gam") supplied directly by callers/tests:
     pass through unchanged apart from accent-marker / R→n adjustment.
   - Upadeśa with anubandha markers (e.g. "RIva~", "gamx~", "YiPal"):
     run anubandha_strip to apply 1.3.2 anunāsika-vowel-it,
     1.3.3 final-consonant-it (with vibhakti exception), and 1.3.5 Yi/wu/qu
     initials.
   Then in either case, apply 6.1.65 ṇo naḥ (dhātu-initial ṇ → n) and drop
   the optional `^` accent marker. */
static bool has_anubandha_marker(const char *s) {
  if (!s) return false;
  if (strchr(s, '~') || strchr(s, '^')) return true;
  /* Leading Yi/wu/qu pairs (1.3.5) are upadeśa markers. */
  if (s[0] && s[1]) {
    if ((s[0] == 'Y' && s[1] == 'i') ||
        (s[0] == 'w' && s[1] == 'u') ||
        (s[0] == 'q' && s[1] == 'u')) return true;
  }
  return false;
}

static void clean_dhatu_upadesa(const char *src, char *dst, size_t dst_len) {
  if (!src || !dst || dst_len == 0) return;
  dst[0] = '\0';
  const char *source = src;
  AnubandhaResult ar;
  if (has_anubandha_marker(src)) {
    anubandha_strip(src, SJ_DHATU, &ar);
    source = ar.clean_slp1;
  }
  size_t pos = 0;
  for (size_t i = 0; source[i] != '\0' && pos + 1 < dst_len; i++) {
    if (source[i] == '^' || source[i] == '~') continue;
    dst[pos++] = source[i];
  }
  dst[pos] = '\0';
  if (dst[0] == 'R') {
    dst[0] = 'n';
  }
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

/* 7.3.77 iṣu-gami-yam-āṃ chaḥ — replaces the final consonant of these four
   specific roots with `cC` before a vowel-initial sārvadhātuka suffix. The
   list is closed: it must NOT be derived from a "ends in am" pattern. */
static const char *const IV_GAM_YAM[] = {"iz", "gam", "yam", "iyaN", NULL};

/* 7.3.76 kramaḥ parasmaipadeṣu — root `kram` lengthens its `a` to `A` when
   conjugated in parasmaipada. Treat this as a small explicit list rather
   than a structural ends-in-am pattern. */
static const char *const KRAM_VRDDHI[] = {"kram", NULL};

/* Selected gaṇa-4 roots whose stem-internal `i` is lengthened to `I` before
   the śyan vikaraṇa. This is a small dhātupāṭha-aligned subset. */
static const char *const GANA4_IDIRGHA[] = {"div", "siv", "sriv", "zWiv", NULL};

static bool root_in_list(const char *stem, const char *const list[]) {
  if (!stem) return false;
  for (size_t i = 0; list[i]; i++) {
    if (strcmp(stem, list[i]) == 0) return true;
  }
  return false;
}

static bool apply_class_transform(const char *clean_root_in, int gana,
                                  ASH_Pada pd,
                                  char *stem, size_t stem_len,
                                  char *after_class, size_t after_class_len,
                                  uint32_t *vik_sutra,
                                  uint32_t *class_sutra,
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
  *class_sutra = 0;
  switch (gana) {
    case 4: *vik_sutra = 301069; break;
    case 6: *vik_sutra = 301077; break;
    case 10: *vik_sutra = 301025; break;
    case 1:
    default: *vik_sutra = 301068; break;
  }

  /* 7.3.77: only the four named roots get final consonant → cC. */
  if (gana == 1 && root_in_list(stem, IV_GAM_YAM)) {
    if (n + 2 >= stem_len) return false;
    /* Replace the last char (m or N for iyaN) with c, then append C. */
    stem[n - 1] = 'c';
    stem[n] = 'C';
    stem[n + 1] = '\0';
    *class_sutra = 703077;
  } else if (gana == 1 && pd == ASH_PARASMAI && root_in_list(stem, KRAM_VRDDHI)) {
    /* 7.3.76: kram → krAm in parasmaipada. */
    for (size_t i = 0; stem[i] != '\0'; i++) {
      if (stem[i] == 'a') { stem[i] = 'A'; break; }
    }
    *class_sutra = 703076;
  } else if (gana == 1) {
    /* Default gaṇa-1 path: 7.3.84 sārvadhātukārdhadhātukayoḥ applies guṇa
       to the stem's final ik-vowel before Sap. */
    replace_first_vowel(stem, false);
    *used_guna = true;
  } else if (gana == 4 && root_in_list(stem, GANA4_IDIRGHA)) {
    /* Selected divādi roots (div, siv, sriv, ṣṭhiv) lengthen internal i→I. */
    for (size_t i = 0; stem[i] != '\0'; i++) {
      if (stem[i] == 'i') { stem[i] = 'I'; break; }
    }
    *class_sutra = 703075;
  } else if (gana == 4) {
    /* Default gaṇa-4 path: śyan is ṅit so 1.1.5 blocks guṇa. */
  } else if (gana == 6) {
    /* Gaṇa-6 (tudādi) takes śa (a), also ṅit — no guṇa. */
  } else if (gana == 10) {
    /* Gaṇa-10 (curādi) takes ṇic (aya); 7.3.84 guṇa applies because ṇic is
       ṇit, not ṅit. */
    replace_first_vowel(stem, false);
    *used_guna = true;
  }

  /* Snapshot the post-class, pre-vikaraṇa stem so callers can log it. */
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
  uint32_t class_sutra = 0;
  bool used_guna = false;
  bool used_ec_ay = false;
  if (!dhatu_slp1 || !ctx_out) return false;
  t = ting_get(ASH_LAT, p, v, pd);
  if (!t) return false;
  prakriya_init_tinanta(ctx_out, dhatu_slp1, gana, ASH_LAT, p, v, pd);
  clean_dhatu_upadesa(dhatu_slp1, clean_root, sizeof(clean_root));
  if (clean_root[0] == '\0') return false;
  if (!apply_class_transform(clean_root, gana, pd, stem, sizeof(stem),
                             after_class, sizeof(after_class),
                             &vik_sutra, &class_sutra,
                             &used_guna, &used_ec_ay)) {
    return false;
  }
  /* Order of logged steps:
     1. guṇa or class-specific rule (clean_root → after_class) if it fired
     2. vikaraṇa assignment + concatenation (after_class → stem)
     3. ec→ay sandhi at root+vikaraṇa boundary, if it fired
     4. tiṅ assignment (stem → form) */
  if (strcmp(clean_root, after_class) != 0) {
    uint32_t id = class_sutra ? class_sutra : (used_guna ? 703084 : vik_sutra);
    const char *desc;
    if (class_sutra == 703077) desc = "izugamiyamAM CaH";
    else if (class_sutra == 703076) desc = "kramaH parasmEpadezu";
    else if (class_sutra == 703075) desc = "ziSraNAM Si";
    else if (used_guna) desc = "sArvadhAtukArdhadhAtukayoH";
    else desc = "class transform";
    log_single_term_change(ctx_out, id, clean_root, after_class, desc);
  }
  log_single_term_change(ctx_out, vik_sutra, after_class, stem,
                         "vikaraRa assignment");
  if (used_ec_ay) {
    log_single_term_change(ctx_out, 601078, after_class, stem, "eco'yavAyAvaH");
  }
  /* 7.3.101 ato dīrgho yaṅi — uttama-puruṣa endings begin with `m` or `v`
     in parasmaipada (mi, vas, mas) and `m`/`v` in ātmane (vahe, mahe).
     The stem-final `a` is lengthened to `A` before such endings, in
     sārvadhātuka context. Apply this BEFORE concatenation so the trace
     records a clean stem→stem transition. */
  {
    char joined_stem[64] = {0};
    strncpy(joined_stem, stem, sizeof(joined_stem) - 1);
    size_t sn = strlen(joined_stem);
    bool yan_initial = (t->clean[0] == 'm' || t->clean[0] == 'v');
    if (yan_initial && sn > 0 && joined_stem[sn - 1] == 'a') {
      joined_stem[sn - 1] = 'A';
      log_single_term_change(ctx_out, 703101, stem, joined_stem,
                             "ato dIrgho yaNi");
      strncpy(stem, joined_stem, sizeof(stem) - 1);
      stem[sizeof(stem) - 1] = '\0';
    }
  }
  if (strlen(stem) + strlen(t->clean) + 1 > sizeof(form)) return false;
  strcpy(form, stem);
  /* 6.1.97 ato guṇe — when stem-final `a` meets an `a`-initial ending, the
     two `a`s collapse to a single `a` (parā-rūpa). Drop the stem's final
     `a` before concatenation. */
  {
    size_t fl = strlen(form);
    if (fl > 0 && form[fl - 1] == 'a' && t->clean[0] == 'a') {
      form[fl - 1] = '\0';
    }
  }
  strcat(form, t->clean);
  log_single_term_change(ctx_out, 304078, stem, form, "tiN assignment");

  /* 8.2.66 sasajuṣo ruḥ + 8.3.15 kharavasānayor visarjanīyaḥ — final `s`
     of a finite verb form becomes `H` (visarga) at end of utterance. The
     two sūtras are collapsed into one logged step here. */
  {
    size_t fl = strlen(form);
    if (fl > 0 && form[fl - 1] == 's') {
      char before[128] = {0};
      strncpy(before, form, sizeof(before) - 1);
      form[fl - 1] = 'H';
      log_single_term_change(ctx_out, 802066, before, form, "sasajuzo ruH");
    }
  }
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
