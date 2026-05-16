/* lat_bhvadi.c — basic laT derivation helpers */
#include "lat_bhvadi.h"
#include "lakara.h"
#include "vikaranas.h"
#include "varna.h"
#include "anubandha.h"
#include "samjna.h"
#include "sandhi_natva.h"
#include "reduplication.h"
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
                                  bool skip_vikarana,
                                  bool block_guna,
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
     fires *before* every other gaṇa-1 transformation. These present-
     stem substitutions only apply when the gaṇa vikaraṇa is being
     used (i.e. LAT/LAN/LOT/VIDHILIN). LRT/LUT/LIT/LUN bypass them. */
  const char *sub = (gana == 1 && !i_anubandha && !skip_vikarana)
                        ? root_substitute_lookup(stem) : NULL;
  if (sub) {
    strncpy(stem, sub, stem_len - 1);
    stem[stem_len - 1] = '\0';
    *class_sutra = 703078;
  } else if (gana == 1 && !skip_vikarana && root_in_list(stem, IV_GAM_YAM)) {
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
  } else if (gana == 1 && !i_anubandha && !block_guna) {
    /* gaṇa-1 default: guṇa applies in two distinct configurations:
         (a) 7.3.84 sārvadhātukārdhadhātukayoḥ — vowel-final aṅga's
             final ik-vowel is guṇa'd (BU → Bo, kf → kar).
         (b) 7.3.86 pugantalaghūpadhasya — consonant-final aṅga whose
             upadha vowel is laghu (short and not part of a heavy
             cluster) is guṇa'd (gam upadha a, sev upadha e — but e is
             not ik so no change).
       For consonant-final stems with guru upadha (e.g. nIv, UW), guṇa
       does NOT apply. block_guna (set by ASIRLIN) suppresses entirely. */
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
      } else if (gana == 2 && sn >= 2) {
        /* Consonant-final stem: 7.3.86 pugantalaghūpadhasya — guṇa
           the laghu upadha (short ik vowel just before final
           consonant). */
        char upadha = stem[sn - 2];
        if (upadha == 'i' || upadha == 'u' || upadha == 'f' || upadha == 'x') {
          replace_first_vowel(stem, false);
          *used_guna = true;
        }
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
  /* For lakāras that introduce their own augment (LRT's sya, LUT's
     tās, LIT's reduplication-stem) the gaṇa vikaraṇa is skipped: the
     ending replaces it. Apply ec→ay sandhi at root final (already
     handled above by the *used_ec_ay flag inspection on the post-
     guṇa stem). */
  if (skip_vikarana) {
    /* If guṇa produced a final e/o, expand it via ec→ay so the stem
       can host a vowel-initial augment correctly. ASIRLIN's yA suffix
       is consonant-initial and the ASIRLIN stem stays bare, so this
       transformation doesn't apply there. */
    if (*used_ec_ay && !block_guna) {
      n = strlen(stem);
      char last = stem[n - 1];
      const char *rep = NULL;
      switch (last) {
        case 'o': rep = "av"; break;
        case 'O': rep = "Av"; break;
        case 'e': rep = "ay"; break;
        case 'E': rep = "Ay"; break;
        default: rep = NULL;
      }
      if (rep && n + 1 < stem_len) {
        stem[n - 1] = rep[0];
        stem[n] = rep[1];
        stem[n + 1] = '\0';
      }
    }
    return true;
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

bool lakara_derive_ctx(ASH_Lakara lakara,
                       const char *dhatu_slp1, int gana, ASH_Purusha p,
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
  t = ting_get(lakara, p, v, pd);
  if (!t) return false;
  prakriya_init_tinanta(ctx_out, dhatu_slp1, gana, lakara, p, v, pd);
  bool i_anubandha = has_i_anubandha(dhatu_slp1);
  /* Strong/weak distinction: pit-anubandha endings (tip, sip, mip —
     all three EKAVACANA endings) cause sārvadhātuka guṇa per 7.3.84.
     Non-pit endings (tas, anti, Tas, Ta, vas, mas) are treated as kit
     by 1.2.4 sārvadhātukam apit, blocking guṇa per 1.1.5. For LOT
     uttama-puruṣa (Ani/Ava/Ama) the suffix is pit, so all three
     uttama forms also count as strong. */
  bool is_strong = (v == ASH_EKAVACANA) ||
                   (lakara == ASH_LOT && p == ASH_UTTAMA);
  clean_dhatu_upadesa(dhatu_slp1, clean_root, sizeof(clean_root));
  if (clean_root[0] == '\0') return false;
  /* LRT/LUT/LRN/ASIRLIN introduce their own augments (sya, tā,
     a-sya, yA) that replace the gaṇa vikaraṇa. */
  bool skip_vikarana = (lakara == ASH_LRT || lakara == ASH_LUT ||
                        lakara == ASH_LRN || lakara == ASH_ASHIRLIM ||
                        lakara == ASH_LIT);
  /* For ASIRLIN, the suffix is treated as kit (1.2.10 halaḥ śnaḥ
     śānajbhyām), so guṇa is blocked entirely. */
  bool block_guna_completely = (lakara == ASH_ASHIRLIM);
  if (block_guna_completely) is_strong = false;
  if (!apply_class_transform(clean_root, gana, pd, i_anubandha, is_strong,
                             skip_vikarana, block_guna_completely,
                             stem, sizeof(stem),
                             after_class, sizeof(after_class),
                             &vik_sutra, &class_sutra,
                             &used_guna, &used_ec_ay)) {
    return false;
  }
  /* Story 3.18 LIT — replace the post-class-transform stem with the
     reduplicated form. For strong forms (eka) vrddhi applies, for
     weak forms (dvi/bahu) the stem keeps the reduplicated root with
     no further guṇa. Special root-replacement table handles the
     historic irregulars (gam → ja+gam → jagāma, kṛ → ca+kar →
     cakāra, etc.). */
  if (lakara == ASH_LIT) {
    char reduped[64] = {0};
    /* 7.1.58 idito num: for i-anubandha roots, insert n before the
       final consonant in the CLEAN ROOT first, so reduplication sees
       the augmented root and can apply the correct abhyāsa pattern
       (especially for vowel-initial roots which need "An"-abhyāsa
       when the consonant cluster has 2+ chars). */
    char augmented_root[64];
    strncpy(augmented_root, clean_root, sizeof(augmented_root) - 1);
    augmented_root[sizeof(augmented_root) - 1] = '\0';
    if (i_anubandha) {
      size_t cl = strlen(augmented_root);
      size_t insert_at = cl;
      for (size_t i = cl; i > 0; i--) {
        if (!varna_is_vowel(augmented_root[i - 1])) {
          insert_at = i - 1;
          break;
        }
      }
      if (cl + 1 < sizeof(augmented_root)) {
        memmove(augmented_root + insert_at + 1, augmented_root + insert_at, cl - insert_at + 1);
        augmented_root[insert_at] = 'n';
      }
    }
    if (!reduplicate(augmented_root, reduped, sizeof(reduped))) {
      return false;
    }
    /* Identify root portion (after the abhyāsa). For "An"-abhyāsa
       vowel-initial roots, the reduplicate function emits 2 chars of
       abhyāsa (vowel + n) before the root, so root_start scans past
       them. The generic logic below skips consonants then a vowel. */
    size_t root_start = 0;
    while (root_start < strlen(reduped) && !varna_is_vowel(reduped[root_start])) root_start++;
    root_start++;  /* past the abhyāsa vowel */
    /* For "An"-abhyāsa case, the next char is 'n' which is still part of
       the abhyāsa; skip it too. We detect this when the abhyāsa starts
       at index 0 with a vowel (vowel-initial root case) and the char
       after the vowel is 'n'. */
    if (reduped[0] && varna_is_vowel(reduped[0]) && reduped[root_start] == 'n' &&
        varna_is_vowel(reduped[root_start + 1])) {
      root_start++;  /* skip the abhyāsa-final 'n' */
    }

    /* Classify the root for LIT treatment.
       - Vowel-final (BU, hu, śru): 7.4.69 inserts "v" before a vowel-
         initial Ṇal ending, NO vrddhi on the root vowel.
       - Consonant-final (kf, gam, pat): 7.2.115 vrddhi in strong
         forms (prathama-eka, uttama-eka); weak forms keep bare root. */
    size_t rlen = strlen(reduped);
    char root_final = (rlen > 0) ? reduped[rlen - 1] : 0;
    bool root_vowel_final = varna_is_vowel(root_final);

    /* BU (bhū) takes a "v" augment instead of vrddhi in LIT
       (per 7.4.69 vasoḥ-) AND has its abhyāsa-u replaced by 'a'
       (per 7.4.66 ut paratasya in the BU-specific context).
       Other vowel-final roots (Sru, kf, nI) get standard vrddhi. */
    static const char *const LIT_V_AUGMENT_ROOTS[] = {"BU", NULL};
    bool needs_v_augment = false;
    for (size_t i = 0; LIT_V_AUGMENT_ROOTS[i]; i++) {
      if (strcmp(clean_root, LIT_V_AUGMENT_ROOTS[i]) == 0) {
        needs_v_augment = true; break;
      }
    }
    /* For BU: override the abhyāsa vowel to 'a' (which shorten_vowel
       no longer applies for U). */
    if (needs_v_augment && root_start > 0 && reduped[root_start - 1] == 'u') {
      reduped[root_start - 1] = 'a';
    }
    if (needs_v_augment) {
      /* Skip vrddhi; append "v" before any vowel-initial ending
         (including iṭ-augmented ones like iTa, iva, ima). */
      if (rlen + 2 < sizeof(reduped) && varna_is_vowel(t->clean[0])) {
        reduped[rlen] = 'v';
        reduped[rlen + 1] = '\0';
      }
    } else if (v == ASH_EKAVACANA) {
      /* Strong forms (eka): the root vowel changes by 7.2.116 ata
         upadhāyāḥ (vrddhi of 'a'-upadhā before ṇit pratyay) or 7.3.86
         pugantalaghūpadhasya (guṇa of i/u-upadhā) — both gated on a
         LAGHU upadhā position. For madhyama-eka (Tal pratyay with iṭ),
         only guṇa applies to i/u/f; 'a' stays unchanged. */
      /* Find last vowel position in the root portion (post-abhyāsa). */
      size_t last_v = (size_t)-1;
      for (size_t i = root_start; i < rlen; i++) {
        if (varna_is_vowel(reduped[i])) last_v = i;
      }
      if (last_v != (size_t)-1) {
        size_t cons_after = (rlen - last_v - 1);
        char upadha = reduped[last_v];
        /* Laghu upadhā per 1.4.10 requires SHORT vowel + single cons. */
        bool upadha_short = (upadha == 'a' || upadha == 'i' || upadha == 'u' ||
                             upadha == 'f' || upadha == 'x');
        bool laghu = upadha_short && (cons_after <= 1);
        bool vowel_final = (cons_after == 0);
        bool strong13 = (p != ASH_MADHYAMA);  /* 1eka or 3eka */
        bool do_change = false;
        bool do_vrddhi = false;
        if (vowel_final && upadha_short) {
          /* kf, nI etc. with SHORT vowel: 1eka/3eka vrddhi; 2eka guṇa. */
          do_change = true;
          do_vrddhi = strong13;
        } else if (vowel_final) {
          /* Long-vowel-final (nI with I, BU with U etc.): vrddhi for
             1eka/3eka; nothing for 2eka. */
          do_change = strong13;
          do_vrddhi = true;
        } else if (laghu) {
          if (upadha == 'a') {
            /* 7.2.116 vrddhi a→A only in 1eka/3eka (Ral-ṇit). */
            do_change = strong13;
            do_vrddhi = true;
          } else {
            /* 7.3.86 guṇa for laghu i/u/f upadhā in all eka forms. */
            do_change = true;
            do_vrddhi = false;
          }
        }
        if (do_change) {
          char tmp[64];
          strncpy(tmp, reduped + root_start, sizeof(tmp) - 1);
          tmp[sizeof(tmp) - 1] = '\0';
          replace_first_vowel(tmp, do_vrddhi);
          strncpy(reduped + root_start, tmp, sizeof(reduped) - root_start - 1);
          reduped[sizeof(reduped) - 1] = '\0';
        }
      }
    }
    (void)root_vowel_final;

    /* 8.3.59 ādeśapratyayoḥ: 's' inside the post-abhyāsa root portion
       becomes 'ṣ' (z) when preceded by an iṇ-vowel from the abhyāsa
       or earlier. Skip the abhyāsa itself (first root_start chars). */
    sandhi_apply_satva(reduped, root_start);

    log_single_term_change(ctx_out, 601008, stem, reduped, "liwi DAtor anabhyAsasya");
    strncpy(stem, reduped, sizeof(stem) - 1);
    stem[sizeof(stem) - 1] = '\0';
  }
  /* 8.4.1 + 8.4.2 ṇatva post-process. The unified helper handles
     n + vowel adjacency. Retroflex stop (q/Q/w/W) targets are
     handled separately below since they aren't covered by the
     n+vowel rule the helper enforces. */
  sandhi_apply_natva(stem);
  for (size_t i = 0; stem[i]; i++) {
    char nxt = stem[i+1];
    if (stem[i] == 'n' && (nxt == 'q' || nxt == 'Q' || nxt == 'w' || nxt == 'W')) {
      /* Pre-retroflex nasal becomes ṇ regardless of trigger
         (8.4.58 anusvārasya yayi parasavarṇaḥ). */
      stem[i] = 'R';
    } else if (stem[i] == 'n' && (nxt == 'k' || nxt == 'K' || nxt == 'g' || nxt == 'G')) {
      /* 8.4.58 parasavarṇa before velars: n → N (ṅ). */
      stem[i] = 'N';
    } else if (stem[i] == 'n' && (nxt == 'c' || nxt == 'C' || nxt == 'j' || nxt == 'J')) {
      /* 8.4.58 parasavarṇa before palatals: n → Y (ñ). */
      stem[i] = 'Y';
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
  /* For LRT/LUT/LRN, after the (vikaraṇa-skipped) class transform we
     still need to apply 7.3.84 guṇa to the root vowel since these
     endings are sārvadhātuka/ārdhadhātuka. Skip when:
     - ASIRLIN suppresses guṇa (block_guna_completely), OR
     - i-anubandha already inserted nuM (makes upadha guru, blocking
       7.3.86 guṇa per laghu requirement). */
  if ((lakara == ASH_LRT || lakara == ASH_LUT || lakara == ASH_LRN) &&
      !block_guna_completely && !i_anubandha) {
    bool has_unstrong = false;
    for (size_t i = 0; stem[i]; i++) {
      char c = stem[i];
      if (c == 'i' || c == 'u' || c == 'f' || c == 'x') { has_unstrong = true; break; }
    }
    if (has_unstrong && !used_guna) {
      replace_first_vowel(stem, false);
      used_guna = true;
    }
  }

  /* 3.1.33 syatāsi luṭos — for LRT/LRN insert sya. For LUT the
     ending table absorbs tā so we just need the iṭ. seṭ-class roots
     get an iṭ before sya/tā (7.2.10 ekāca upadeśe). Most roots are
     seṭ; the LRT-/LUT-specific aniṭ list is closed and short. */
  if (lakara == ASH_LRT || lakara == ASH_LRN || lakara == ASH_LUT) {
    static const char *const ANIT_ROOTS[] = {
      "ad", "vac", "vap", "vah", "vid", NULL
    };
    bool is_anit = false;
    for (size_t i = 0; ANIT_ROOTS[i]; i++) {
      if (strcmp(clean_root, ANIT_ROOTS[i]) == 0) {
        is_anit = true;
        break;
      }
    }
    char extended[128] = {0};
    if (lakara == ASH_LUT) {
      /* LUT ending table already includes tā; just prepend iṭ if seṭ. */
      if (is_anit) {
        snprintf(extended, sizeof(extended), "%s", stem);
      } else {
        snprintf(extended, sizeof(extended), "%si", stem);
      }
    } else {
      /* LRT / LRN: append (i)sya to stem. */
      if (is_anit) {
        snprintf(extended, sizeof(extended), "%ssya", stem);
      } else {
        snprintf(extended, sizeof(extended), "%sizya", stem);
      }
    }
    log_single_term_change(ctx_out, 301033, stem, extended,
                           "syatAsi luwoH");
    strncpy(stem, extended, sizeof(stem) - 1);
    stem[sizeof(stem) - 1] = '\0';
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
    /* 6.1.78 eco'yavāyāvaḥ — stem-final e/o/E/O before vowel-initial
       ending: e → ay, o → av, E → Ay, O → Av. Important for LIT
       vrddhi-results like juhO + a → juhAv + a = juhAva. */
    if ((stem_final == 'o' || stem_final == 'O' || stem_final == 'e' || stem_final == 'E') &&
        (ending_initial == 'a' || ending_initial == 'A' ||
         ending_initial == 'i' || ending_initial == 'I' ||
         ending_initial == 'u' || ending_initial == 'U')) {
      char rep_v = (stem_final == 'o' || stem_final == 'O') ? 'v' : 'y';
      char rep_a = (stem_final == 'o' || stem_final == 'e') ? 'a' : 'A';
      form[fl - 1] = rep_a;
      form[fl] = rep_v;
      form[fl + 1] = '\0';
      strcat(form, t->clean);
    } else if (stem_final == 'a' && ending_initial == 'a') {
      form[fl - 1] = '\0';
      strcat(form, t->clean);
    } else if (stem_final == 'A' && ending_initial == 'a') {
      strcat(form, t->clean + 1);
    } else if (stem_final == 'a' && ending_initial == 'A') {
      /* 6.1.101 savarṇa-dīrgha: a + A → A (drop stem-final a). */
      form[fl - 1] = '\0';
      strcat(form, t->clean);
    } else if (stem_final == 'I' && ending_initial == 'i') {
      strcat(form, t->clean + 1);
    } else if (stem_final == 'U' && ending_initial == 'u') {
      strcat(form, t->clean + 1);
    } else if (stem_final == 'a' && ending_initial == 'e') {
      /* 6.1.87 ad guṇaḥ: a + e → e (drop stem-final a). */
      form[fl - 1] = '\0';
      strcat(form, t->clean);
    } else if (stem_final == 'a' && ending_initial == 'E') {
      /* 6.1.88 vṛddhir eci: a + ai → ai (drop stem-final a). */
      form[fl - 1] = '\0';
      strcat(form, t->clean);
    } else if (stem_final == 'a' && ending_initial == 'O') {
      /* 6.1.88 vṛddhir eci: a + au → au. */
      form[fl - 1] = '\0';
      strcat(form, t->clean);
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

  /* H-final root sandhi (8.2.32 dāder dhātor ghaḥ + 8.2.40 jhaṣas
     tathor dho 'dhaḥ + 8.3.78 dadhastathos ca + companion rules):
     - h + t/T → g + D (aspiration of root-h transfers forward to t/T)
     - h + s   → k + z (h drops, leaving k; aspiration backwards to
       initial d → D, since the forward path is blocked)
     - h before m / v / vowel-initial: h is preserved.
     We apply this on the joined form by a simple two-character lookup
     covering the common diha~/duha~/lih-style roots. */
  {
    char *p = form;
    while (*p) {
      if (*p == 'h') {
        char nxt = p[1];
        if (nxt == 't' || nxt == 'T') {
          p[0] = 'g';
          p[1] = 'D';
        } else if (nxt == 's') {
          p[0] = 'k';
          p[1] = 'z';
          /* Aspiration transfer backwards to a dhātu-initial d. */
          if (form[0] == 'd') form[0] = 'D';
        }
      }
      p++;
    }
  }

  /* 8.3.59 ādeśapratyayoḥ — pratyaya-region `s` becomes ṣ after iṇ
     vowel. Routed through the unified helper. */
  sandhi_apply_satva(form, strlen(stem));
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
  /* 6.4.71 luṅ-laṅ-lṛṅ-kṣv aḍudāttaḥ — past-tense lakāras (LAN, LRN,
     LUN) take an a-augment prepended to the form. Vowel sandhi at the
     a- + root junction: a + a → A, a + i → e, a + u → o. */
  if (lakara == ASH_LAN || lakara == ASH_LRN) {
    char augmented[128] = {0};
    char before[128] = {0};
    strncpy(before, form, sizeof(before) - 1);
    char first = form[0];
    if (first == 'a') {
      /* a + a → A (savarṇa-dīrgha 6.1.101). */
      augmented[0] = 'A';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if (first == 'i' || first == 'I') {
      /* a + i/I → e (guṇa 6.1.87). */
      augmented[0] = 'e';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if (first == 'u' || first == 'U') {
      /* a + u/U → o (guṇa 6.1.87). */
      augmented[0] = 'o';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else {
      augmented[0] = 'a';
      strncpy(augmented + 1, form, sizeof(augmented) - 2);
    }
    augmented[sizeof(augmented) - 1] = '\0';
    log_single_term_change(ctx_out, 604071, before, augmented,
                           "luN-laN-lfN-kzv aDudAttaH");
    strncpy(form, augmented, sizeof(form) - 1);
    form[sizeof(form) - 1] = '\0';
  }
  return true;
}

bool lat_bhvadi_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                           ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out) {
  return lakara_derive_ctx(ASH_LAT, dhatu_slp1, gana, p, v, pd, ctx_out);
}

bool lakara_derive(ASH_Lakara lakara,
                   const char *dhatu_slp1, int gana, ASH_Purusha p,
                   ASH_Vacana v, ASH_Pada pd, char *out_slp1, size_t out_len) {
  PrakriyaCtx ctx = {0};
  if (!out_slp1 || out_len == 0) return false;
  if (!lakara_derive_ctx(lakara, dhatu_slp1, gana, p, v, pd, &ctx)) return false;
  prakriya_current_form(&ctx, out_slp1, out_len);
  return true;
}

bool lat_bhvadi_derive(const char *dhatu_slp1, int gana, ASH_Purusha p,
                       ASH_Vacana v, ASH_Pada pd, char *out_slp1, size_t out_len) {
  return lakara_derive(ASH_LAT, dhatu_slp1, gana, p, v, pd, out_slp1, out_len);
}
