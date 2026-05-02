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
  /* Trailing Y as final consonant indicates ñ-it (1.3.3). We don't
     also include S because it commonly appears as the root-final of
     real dhātus (e.g. dfS = dṛś); the substitution table for those
     uses the SLP1 root-form including S. */
  size_t n = strlen(s);
  if (n >= 2) {
    char last = s[n - 1];
    if (last == 'Y') return true;
  }
  return false;
}

/* Detect i-anubandha (the `i~` cluster at end) per 7.1.58 idito num
   dhātoḥ — such dhātus take a nuM (n) augment before the final
   consonant of the root in sārvadhātuka derivations. */
static bool has_i_anubandha(const char *s) {
  if (!s) return false;
  size_t n = strlen(s);
  if (n < 2) return false;
  return s[n - 2] == 'i' && s[n - 1] == '~';
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
  /* 6.1.64 dhātv-ādeḥ ṣaḥ saḥ — initial ṣ (z) of a dhātu is realised
     as dental s in the derivation. When ṣ is part of a ṣṭ-cluster
     (zw in SLP1), the following ṭ also de-retroflexes to t (zwUp →
     stUp per 8.4.41 in reverse for dhātu-initial). */
  if (dst[0] == 'z') {
    dst[0] = 's';
    if (dst[1] == 'w') dst[1] = 't';
    if (dst[1] == 'W') dst[1] = 'T';
  }
  /* 6.1.65 ṇo naḥ — initial ṇ (R) of a dhātu is realised as dental n.
     Also apply when ṇ stands second after a converted ṣ (zRA → snA),
     i.e. anywhere within the dhātu-initial cluster. */
  if (dst[0] == 'R') dst[0] = 'n';
  if (dst[0] == 's' && dst[1] == 'R') dst[1] = 'n';
}

static void replace_first_vowel(char *root, bool vrddhi) {
  if (!root) return;
  for (size_t i = 0; root[i] != '\0'; i++) {
    char c = root[i];
    /* Eligible `ik` vowels for guṇa/vṛddhi. We exclude 'A' (long ā)
       because varna_guna(A)='a' would shorten the stem; the other
       long vowels I/U/F return their correct guṇa results
       (e/o/Ar+r) so they are safe to include. */
    if (c == 'a' || c == 'i' || c == 'I' || c == 'u' || c == 'U' ||
        c == 'f' || c == 'F' || c == 'x' || c == 'X') {
      char rep = vrddhi ? varna_vrddhi(c) : varna_guna(c);
      /* For ṛ → ar / ḹ → ār / ḷ → al / ḹ → āl, varna_guna/vrddhi
         returns just the vowel part; the trailing r/l must be inserted
         by the caller. */
      if (c == 'f' || c == 'F' || c == 'x' || c == 'X') {
        char tail = (c == 'x' || c == 'X') ? 'l' : 'r';
        size_t n = strlen(root);
        if (i + 1 < n) {
          memmove(root + i + 2, root + i + 1, n - i - 1);
        }
        root[i] = rep;
        root[i + 1] = tail;
        root[n + 1] = '\0';
      } else {
        root[i] = rep;
      }
      return;
    }
  }
}

static bool append_with_vowel_sandhi(char *stem, size_t stem_len, const char *vik) {
  size_t used;
  if (!stem || !vik || stem_len == 0) return false;
  used = strlen(stem);
  if (used == 0) return false;
  /* Empty vikaraṇa (athematic gaṇa-2/3/7): nothing to append. */
  if (vik[0] == '\0') return true;
  /* 6.1.78 ec → ay/av sandhi at the stem→vikaraṇa junction when the
     vikaraṇa begins with a vowel. Stem-final e/o/E/O turns into
     ay/av/Ay/Av before the vikaraṇa's initial vowel. This applies to
     gaṇa-1's `a` (Sap), gaṇa-10's `aya` (ṇic), and similar a-initial
     vikaraṇas. */
  if (vik[0] == 'a') {
    char final = stem[used - 1];
    if (final == 'o' || final == 'O') {
      stem[used - 1] = (final == 'o') ? 'a' : 'A';
      stem[used] = 'v';
      stem[used + 1] = '\0';
      used = strlen(stem);
    } else if (final == 'e' || final == 'E') {
      stem[used - 1] = (final == 'e') ? 'a' : 'A';
      stem[used] = 'y';
      stem[used + 1] = '\0';
      used = strlen(stem);
    } else if (final == 'a') {
      /* 6.1.97 a + a → a (parā-rūpa): drop one `a` from the vikaraṇa. */
      if (used + strlen(vik) > stem_len) return false;
      strncat(stem, vik + 1, stem_len - used - 1);
      return true;
    } else if (final == 'A') {
      /* 6.1.101 A + a → A (savarṇa-dīrgha): drop the vikaraṇa's `a`. */
      if (used + strlen(vik) > stem_len) return false;
      strncat(stem, vik + 1, stem_len - used - 1);
      return true;
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
static const char *const GANA4_IDIRGHA[] = {"div", "siv", "sriv", "stiv", "zWiv", NULL};

/* 7.3.75 (śiti) — selected gaṇa-4 roots whose root-vowel `a` lengthens
   to `A` before the śyan vikaraṇa. */
static const char *const GANA4_ADIRGHA[] = {"dam", "kram", "klam", "cam", "Bram", NULL};

/* 7.3.78 pā-ghrā-… — explicit root substitutions before Sap. The list is
   closed and small; long-term this should come from a TSV alongside the
   dhātupāṭha but a static table is sufficient for current oracle coverage. */
typedef struct { const char *root; const char *substitute; } SubRule;
static const SubRule ROOT_SUBSTITUTIONS[] = {
  {"pA",   "piba"},
  {"GrA",  "jiGra"},
  {"DmA",  "Dama"},
  /* zWA and zad are normalised to sTA and sad by 6.1.64 (and the
     accompanying ṣṭ→st cluster handling for dhātu-initial). The
     substitution result preserves the W (ṭh) within tizWa per 8.4.41
     ṣṭunā ṣṭuḥ which retroflexes again after the t/ṣ adjacency. */
  {"sTA",  "tizWa"},
  {"mnA",  "mana"},
  {"dA",   "yacCa"},
  {"dfS",  "paSya"},
  {"fc",   "fcCa"},
  {"sad",  "sId"},
  {NULL, NULL}
};

static const char *root_substitute_lookup(const char *clean_root) {
  if (!clean_root) return NULL;
  for (size_t i = 0; ROOT_SUBSTITUTIONS[i].root; i++) {
    if (strcmp(clean_root, ROOT_SUBSTITUTIONS[i].root) == 0) {
      return ROOT_SUBSTITUTIONS[i].substitute;
    }
  }
  return NULL;
}

/* 7.2.116 ata upadhāyāḥ — penultimate `a` of the root lengthens to `A`
   before ṇit/ñit suffixes (here applied to gaṇa-10's ṇic-aya). Encoded
   as a small list of roots that exhibit this lengthening so we don't
   over-apply to roots like `kaTa` whose upadha is a consonant. */
static const char *const GANA10_UPADHA_ALENGTHEN[] = {
  "taq", "vad", "Gaw", "laq", "lab", "lap", "raq", "raB",
  NULL
};

static bool root_in_list(const char *stem, const char *const list[]) {
  if (!stem) return false;
  for (size_t i = 0; list[i]; i++) {
    if (strcmp(stem, list[i]) == 0) return true;
  }
  return false;
}

static bool apply_class_transform(const char *clean_root_in, int gana,
                                  ASH_Pada pd,
                                  bool i_anubandha,
                                  bool is_strong,
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

  /* 7.1.58 idito num dhātoḥ — i-anubandha dhātus take a nuM augment
     inserted before the final consonant of the root in sārvadhātuka
     contexts. The nuM insertion blocks the regular guṇa path because
     the upadha syllable becomes guru. The nuM letter assimilates to
     the homorganic nasal of the following consonant per 8.4.58
     anusvārasya yayi parasavarṇaḥ. */
  if (i_anubandha) {
    size_t sn = strlen(stem);
    if (sn >= 2 && sn + 1 < stem_len) {
      char final = stem[sn - 1];
      char nasal = 'n';
      /* Match nasal to the varga of the final consonant. */
      switch (final) {
        case 'k': case 'K': case 'g': case 'G':
          nasal = 'N'; break;  /* ṅ */
        case 'c': case 'C': case 'j': case 'J':
          nasal = 'Y'; break;  /* ñ */
        case 'w': case 'W': case 'q': case 'Q':
          nasal = 'R'; break;  /* ṇ */
        case 't': case 'T': case 'd': case 'D':
          nasal = 'n'; break;  /* n */
        case 'p': case 'P': case 'b': case 'B':
          nasal = 'm'; break;  /* m */
        default: nasal = 'n'; break;
      }
      memmove(stem + sn, stem + sn - 1, 2);
      stem[sn - 1] = nasal;
      sn = strlen(stem);
      *class_sutra = 701058;
    }
  }

  /* 7.3.78 root substitution (pā → piba, sthā → tiṣṭha, dṛś → paśya, …)
     fires *before* every other gaṇa-1 transformation. */
  const char *sub = (gana == 1 && !i_anubandha) ? root_substitute_lookup(stem) : NULL;
  if (sub) {
    strncpy(stem, sub, stem_len - 1);
    stem[stem_len - 1] = '\0';
    *class_sutra = 703078;
  } else if (gana == 1 && root_in_list(stem, IV_GAM_YAM)) {
    /* 7.3.77: only the four named roots get final consonant → cC. */
    if (n + 2 >= stem_len) return false;
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
  } else if (gana == 1 && !i_anubandha) {
    /* gaṇa-1 default: guṇa applies in two distinct configurations:
         (a) 7.3.84 sārvadhātukārdhadhātukayoḥ — vowel-final aṅga's
             final ik-vowel is guṇa'd (BU → Bo, kf → kar).
         (b) 7.3.86 pugantalaghūpadhasya — consonant-final aṅga whose
             upadha vowel is laghu (short and not part of a heavy
             cluster) is guṇa'd (gam upadha a, sev upadha e — but e is
             not ik so no change).
       For consonant-final stems with guru upadha (e.g. nIv, UW), guṇa
       does NOT apply. */
    size_t sn = strlen(stem);
    char final = sn > 0 ? stem[sn - 1] : 0;
    bool final_vowel = (final == 'a' || final == 'i' || final == 'I' ||
                        final == 'u' || final == 'U' || final == 'f' ||
                        final == 'F' || final == 'x' || final == 'X' ||
                        final == 'e' || final == 'o');
    char upadha = sn >= 2 ? stem[sn - 2] : 0;
    bool upadha_laghu = (upadha == 'a' || upadha == 'i' || upadha == 'u' ||
                         upadha == 'f' || upadha == 'x');
    if (final_vowel || upadha_laghu) {
      replace_first_vowel(stem, false);
      *used_guna = true;
    }
  } else if (gana == 2 || gana == 3 || gana == 7) {
    /* Athematic gaṇas: no vikaraṇa, the tiṅ ending attaches directly to
       the root stem. By 1.2.4 sārvadhātukam apit and 1.1.5 kñiti ca,
       only the pit endings (tip, sip, mip — i.e. ekavacana endings)
       trigger guṇa/vṛddhi. The non-pit endings (tas, anti, Tas, Ta,
       vas, mas) are kit and block guṇa, so root stays bare. */
    if (is_strong) {
      size_t sn = strlen(stem);
      char final = sn > 0 ? stem[sn - 1] : 0;
      bool short_vowel = (final == 'i' || final == 'u' || final == 'f' ||
                          final == 'x');
      bool long_vowel  = (final == 'I' || final == 'U' || final == 'F' ||
                          final == 'X');
      if (gana == 2 && short_vowel) {
        replace_first_vowel(stem, true);
        *used_guna = true;
      } else if (gana == 2 && long_vowel) {
        replace_first_vowel(stem, false);
        *used_guna = true;
      }
    }
  } else if (gana == 4 && root_in_list(stem, GANA4_IDIRGHA)) {
    /* Selected divādi roots (div, siv, sriv, ṣṭhiv) lengthen internal i→I. */
    for (size_t i = 0; stem[i] != '\0'; i++) {
      if (stem[i] == 'i') { stem[i] = 'I'; break; }
    }
    *class_sutra = 703075;
  } else if (gana == 4 && root_in_list(stem, GANA4_ADIRGHA)) {
    /* 7.3.75 ṣṭhivu-klamu-cama-...-āṃ śiti — selected gaṇa-4 roots
       lengthen root-vowel a → A before the śyan vikaraṇa. */
    for (size_t i = 0; stem[i] != '\0'; i++) {
      if (stem[i] == 'a') { stem[i] = 'A'; break; }
    }
    *class_sutra = 703075;
  } else if (gana == 4) {
    /* Default gaṇa-4 path: śyan is ṅit so 1.1.5 blocks guṇa.
       But 7.1.100 ṛto id dhātoḥ + 8.2.77 hali ca: dhātu-final ṝ
       (with intervening consonant) becomes Ir before consonant
       suffix. JFz → JIr (F → Ir, z dropped) → JIrya → JIryati. */
    size_t sn = strlen(stem);
    if (sn >= 2 && stem[sn - 2] == 'F') {
      stem[sn - 2] = 'I';
      stem[sn - 1] = 'r';
      stem[sn] = '\0';
      *class_sutra = 701100;
    }
  } else if (gana == 6) {
    /* Gaṇa-6 (tudādi) takes śa (a), which is ṅit/kit — no guṇa.
       6.1.16 vacisvapiyajāṃ kiti applies saṃprasāraṇa (ra/ya/va → ṛ/i/u)
       to specific roots before kit suffix. The śa vikaraṇa is kit, so:
       vrasc → vfSc (ra → ṛ; s + c → S + c via 8.4.40 stoḥ ścunā ścuḥ). */
    if (strcmp(stem, "vrasc") == 0) {
      strncpy(stem, "vfSc", stem_len - 1);
      stem[stem_len - 1] = '\0';
      *class_sutra = 601016;
    } else if (strcmp(stem, "praC") == 0) {
      strncpy(stem, "pfC", stem_len - 1);
      stem[stem_len - 1] = '\0';
      *class_sutra = 601016;
    } else if (strcmp(stem, "Brasj") == 0) {
      strncpy(stem, "Bfjj", stem_len - 1);
      stem[stem_len - 1] = '\0';
      *class_sutra = 601016;
    }
  } else if (gana == 10) {
    /* Gaṇa-10 (curādi) takes ṇic (aya). With i-anubandha + nuM augment
       the upadha is guru so guṇa does not fire. */
    if (i_anubandha) {
      /* nuM already inserted; no further transformation. */
    } else if (root_in_list(stem, GANA10_UPADHA_ALENGTHEN)) {
      /* 7.2.116 ata upadhāyāḥ: penultimate `a` lengthens to `A`. */
      size_t sn = strlen(stem);
      if (sn >= 2 && stem[sn - 2] == 'a') {
        stem[sn - 2] = 'A';
        *class_sutra = 702116;
      }
    } else {
      /* If the root ends in a vowel, 7.2.115 aco ñṇiti applies vṛddhi
         to that vowel (ñic is ṇit). For consonant-final roots the
         upadha-laghu rule 7.3.86 applies guṇa to the penultimate.
         When the root is given in dhātupāṭha pre-attached form
         (BAma, varRa, kaTa) — i.e. ends in `a` after a consonant —
         the trailing `a` is the inherent vowel and no vrddhi/guṇa
         applies; the `a` collapses with the `aya` vikaraṇa via
         6.1.97 in append_with_vowel_sandhi. */
      size_t sn = strlen(stem);
      char final = sn > 0 ? stem[sn - 1] : 0;
      bool final_a = (final == 'a');
      bool final_long_vowel = (final == 'A' || final == 'I' || final == 'U' ||
                               final == 'F' || final == 'X' || final == 'e' ||
                               final == 'o');
      bool final_short_vowel = (final == 'i' || final == 'u' || final == 'f' ||
                                final == 'x');
      if (final_a) {
        /* No transformation; let 6.1.97 handle the a+a junction. */
      } else if (final_long_vowel || final_short_vowel) {
        replace_first_vowel(stem, true);
        *used_guna = true;
      } else {
        /* consonant-final: 7.3.86 guṇa of laghu upadha. */
        replace_first_vowel(stem, false);
        *used_guna = true;
      }
    }
  }

  /* Snapshot the post-class, pre-vikaraṇa stem so callers can log it. */
  strncpy(after_class, stem, after_class_len - 1);
  after_class[after_class_len - 1] = '\0';

  n = strlen(stem);
  if (n > 0 && (stem[n - 1] == 'o' || stem[n - 1] == 'O' || stem[n - 1] == 'e' || stem[n - 1] == 'E')) {
    *used_ec_ay = true;
  }
  if (!append_with_vowel_sandhi(stem, stem_len, vik)) return false;
  /* gaṇa-5 (svādi nu) and gaṇa-8 (tanādi u) strong-form guṇa: the
     vikaraṇa-final u becomes o per 7.3.84 sārvadhātukārdhadhātukayoḥ
     when the suffix is pit (ekavacana). For weak forms the u stays. */
  if ((gana == 5 || gana == 8) && is_strong) {
    size_t sl = strlen(stem);
    if (sl > 0 && stem[sl - 1] == 'u') {
      stem[sl - 1] = 'o';
    }
  }
  return true;
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
  bool i_anubandha = has_i_anubandha(dhatu_slp1);
  /* Strong/weak distinction: pit-anubandha endings (tip, sip, mip —
     all three EKAVACANA endings) cause sārvadhātuka guṇa per 7.3.84.
     Non-pit endings (tas, anti, Tas, Ta, vas, mas) are treated as kit
     by 1.2.4 sārvadhātukam apit, blocking guṇa per 1.1.5. */
  bool is_strong = (v == ASH_EKAVACANA);
  clean_dhatu_upadesa(dhatu_slp1, clean_root, sizeof(clean_root));
  if (clean_root[0] == '\0') return false;
  if (!apply_class_transform(clean_root, gana, pd, i_anubandha, is_strong,
                             stem, sizeof(stem),
                             after_class, sizeof(after_class),
                             &vik_sutra, &class_sutra,
                             &used_guna, &used_ec_ay)) {
    return false;
  }
  /* 8.4.1 + 8.4.2 ṇatva post-process: in the present-tense stem, any
     `n` (including the nuM augment) immediately following an r/f
     trigger via allowed-only intervening characters becomes ṇ. The
     dispatch tables in subanta apply this internally; for tinanta we
     run a single pass over the joined stem here. */
  {
    bool seen = false;
    for (size_t i = 0; stem[i]; i++) {
      char c = stem[i];
      if (c == 'r' || c == 'f' || c == 'z' || c == 'F' || c == 'R') {
        seen = true;
      } else if (c == 't' || c == 'T' || c == 'd' || c == 'D' ||
                 c == 'c' || c == 'C' || c == 'j' || c == 'J' || c == 'Y' ||
                 c == 'S' || c == 's' || c == 'l') {
        seen = false;
      } else if (seen && c == 'n') {
        char next = stem[i + 1];
        if (next == 'q' || next == 'Q' || next == 'w' || next == 'W' ||
            (next >= 'A' && next <= 'z' && (next == 'a' || next == 'A' ||
             next == 'i' || next == 'I' || next == 'u' || next == 'U' ||
             next == 'e' || next == 'o' || next == 'E' || next == 'O'))) {
          stem[i] = 'R';
          seen = false;
        }
      }
    }
  }
  /* Order of logged steps:
     1. guṇa or class-specific rule (clean_root → after_class) if it fired
     2. vikaraṇa assignment + concatenation (after_class → stem)
     3. ec→ay sandhi at root+vikaraṇa boundary, if it fired
     4. tiṅ assignment (stem → form) */
  if (strcmp(clean_root, after_class) != 0) {
    uint32_t id = class_sutra ? class_sutra : (used_guna ? 703084 : vik_sutra);
    const char *desc;
    if (class_sutra == 703078) desc = "pAGrAdhmAsTAmnAdAR-pibajiGradhamatizWamanayacCa";
    else if (class_sutra == 703077) desc = "izugamiyamAM CaH";
    else if (class_sutra == 703076) desc = "kramaH parasmEpadezu";
    else if (class_sutra == 703075) desc = "ziSraNAM Si";
    else if (class_sutra == 702116) desc = "ata upaDAyAH";
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
  /* Athematic vowel-final + vowel-initial ending: insert a glide
     (v after u/U, y after i/I, r after ṛ/ṝ) so the surface keeps the
     root vowel. ru + anti → ruvanti, vI + anti → viyanti (with I→i),
     yu + anti → yuvanti. This corresponds to the 6.4.77 acijñiti
     iyaṅ/uvaṅ + the underlying root vowel surfacing as a short. */
  if (gana == 2 || gana == 3 || gana == 5 || gana == 7 || gana == 8) {
    size_t fl = strlen(form);
    char stem_final = fl > 0 ? form[fl - 1] : 0;
    char ending_initial = t->clean[0];
    bool ending_vowel = (ending_initial == 'a' || ending_initial == 'A' ||
                         ending_initial == 'i' || ending_initial == 'I' ||
                         ending_initial == 'u' || ending_initial == 'U');
    if (ending_vowel) {
      char glide = 0;
      char shortened = stem_final;
      if (stem_final == 'u' || stem_final == 'U') {
        glide = 'v';
        shortened = 'u';
      } else if (stem_final == 'i' || stem_final == 'I') {
        glide = 'y';
        shortened = 'i';
      } else if (stem_final == 'f' || stem_final == 'F') {
        glide = 'r';
        shortened = 'f';
      }
      if (glide) {
        form[fl - 1] = shortened;
        form[fl] = glide;
        form[fl + 1] = '\0';
      }
    }
  }
  /* 6.1.97 / 6.1.101 vowel-junction sandhi at the stem→ending boundary:
     - a + a → a (parā-rūpa, drop stem-final a)
     - A + a → A (savarṇa-dīrgha, drop ending-initial a)
     - i + i → ī, u + u → ū (analogous savarṇa-dīrgha forms)
     The savarṇa-dīrgha cases matter for athematic gaṇa-2 vowel-final
     roots like KyA + anti = KyAnti. */
  {
    size_t fl = strlen(form);
    char stem_final = fl > 0 ? form[fl - 1] : 0;
    char ending_initial = t->clean[0];
    if (stem_final == 'a' && ending_initial == 'a') {
      form[fl - 1] = '\0';
      strcat(form, t->clean);
    } else if (stem_final == 'A' && ending_initial == 'a') {
      strcat(form, t->clean + 1);
    } else if (stem_final == 'I' && ending_initial == 'i') {
      strcat(form, t->clean + 1);
    } else if (stem_final == 'U' && ending_initial == 'u') {
      strcat(form, t->clean + 1);
    } else {
      strcat(form, t->clean);
    }
  }
  /* Nasal place assimilation (cluster surface rule corresponding to
     8.4.58 anusvārasya yayi parasavarṇaḥ extended for plain nasals):
     `n` immediately before a labial stop (p/P/b/B/m) surfaces as m.
     This handles SunBati from SunBa+ti where the n of the root assimilates
     to the labial. */
  for (size_t i = 0; form[i]; i++) {
    if (form[i] != 'n') continue;
    char next = form[i + 1];
    if (next == 'p' || next == 'P' || next == 'b' || next == 'B' ||
        next == 'm') {
      form[i] = 'm';
    }
  }

  /* 8.3.59 ādeśapratyayoḥ — `s` of pratyaya becomes ṣ (z) when preceded
     by an iṇ letter (i, ī, u, ū, ṛ, ṝ, ḷ, e, o, ai, au, k). a/ā do not
     trigger this rule. We only flip suffix-region s, identified by
     being past the stem boundary (which equals the stem length we
     held before concatenation). */
  {
    size_t stem_len_at_join = strlen(stem);
    for (size_t i = stem_len_at_join; form[i]; i++) {
      if (form[i] != 's') continue;
      if (i == 0) continue;
      char prev = form[i - 1];
      bool in_iN = (prev == 'i' || prev == 'I' || prev == 'u' || prev == 'U' ||
                    prev == 'f' || prev == 'F' || prev == 'x' || prev == 'X' ||
                    prev == 'e' || prev == 'o' || prev == 'E' || prev == 'O' ||
                    prev == 'k');
      if (in_iN) form[i] = 'z';
    }
  }
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
