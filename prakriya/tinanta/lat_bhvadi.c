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

/* Returns true iff the dhātu, after stripping anubandhas, originally
   began with ṣ (z) and was thus subject to 6.1.64 dhātv-ādeḥ ṣaḥ saḥ.
   This flag is needed so that LIT reduplication's 8.3.59 ṣatva pass
   can be applied only to roots whose initial 's' is really an ādeśa,
   not to roots whose initial s/sk/sp/etc. is a genuine root-s. */
static void clean_dhatu_upadesa_ex(const char *src, char *dst, size_t dst_len,
                                   bool *initial_was_sa) {
  if (initial_was_sa) *initial_was_sa = false;
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
  if (dst[0] == 'z') {
    if (initial_was_sa) *initial_was_sa = true;
    dst[0] = 's';
    if (dst[1] == 'w') dst[1] = 't';
    if (dst[1] == 'W') dst[1] = 'T';
  }
  if (dst[0] == 'R') dst[0] = 'n';
  if (dst[0] == 's' && dst[1] == 'R') dst[1] = 'n';
}

/* Kept for source-level documentation; the _ex variant is used. */
#define clean_dhatu_upadesa(src, dst, dst_len) \
    clean_dhatu_upadesa_ex((src), (dst), (dst_len), NULL)

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
    /* gaṇa-7 (rudhādi) śnam vikaraṇa: insert 'na' (strong) or 'n'
       (weak) before the final consonant of the root.
       rudh → rurnaDh (strong eka) → ruRaDh via natva, then ending
       'ti' → ruRadDi via cluster.
       rudh → runDh (weak) → runDtaH → runDdhaH. */
    if (gana == 7 && !skip_vikarana) {
      size_t sn = strlen(stem);
      if (sn >= 2) {
        const char *infix = is_strong ? "na" : "n";
        size_t inflen = strlen(infix);
        /* Insert before the final consonant. */
        size_t insert_at = sn - 1;
        if (sn + inflen < stem_len) {
          memmove(stem + insert_at + inflen, stem + insert_at, sn - insert_at + 1);
          memcpy(stem + insert_at, infix, inflen);
        }
      }
    }
    /* gaṇa-3 (juhotyādi) reduplicates the root (6.1.10 ślau) before
       attaching the athematic ending. Reduplicate first, then apply
       guṇa to the LAST vowel of the reduplicated form for strong
       forms. */
    if (gana == 3 && !skip_vikarana) {
      char reduped[64] = {0};
      if (reduplicate(stem, reduped, sizeof(reduped))) {
        strncpy(stem, reduped, stem_len - 1);
        stem[stem_len - 1] = '\0';
      }
    }
    if (is_strong) {
      size_t sn = strlen(stem);
      char final = sn > 0 ? stem[sn - 1] : 0;
      bool short_vowel = (final == 'i' || final == 'u' || final == 'f' ||
                          final == 'x');
      bool long_vowel  = (final == 'I' || final == 'U' || final == 'F' ||
                          final == 'X');
      if ((gana == 2 || gana == 3) && short_vowel) {
        /* Apply guṇa to the final vowel (root-final for gaṇa-2/3). */
        if (gana == 3) {
          /* For reduplicated stem, replace LAST vowel (the root vowel),
             not the abhyāsa vowel. */
          for (ssize_t i = (ssize_t)sn - 1; i >= 0; i--) {
            char c = stem[i];
            if (c == 'i' || c == 'u' || c == 'f' || c == 'x') {
              char rep = varna_guna(c);
              if (c == 'f' || c == 'x') {
                /* f/x guṇa = ar/al with tail. */
                char tail = (c == 'x') ? 'l' : 'r';
                if (i + 1 < (ssize_t)stem_len) {
                  memmove(stem + i + 2, stem + i + 1, sn - i);
                  stem[i] = rep;
                  stem[i + 1] = tail;
                }
              } else {
                stem[i] = rep;
              }
              *used_guna = true;
              break;
            }
          }
        } else {
          replace_first_vowel(stem, true);
          *used_guna = true;
        }
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
  /* 6.4.24 aniditāṃ hala upadhāyāḥ kṅiti — for non-idit aṅgas with
     a nasal+stop upadhā-cluster, the nasal drops before a kit/ṅit
     suffix. Fires for gaṇa-9 (śnā is ṅit) on roots like stanB → staB,
     skanB → skaB, granT → graT etc. */
  if (gana == 9 && !i_anubandha) {
    size_t sn = strlen(stem);
    if (sn >= 3) {
      char penult = stem[sn - 2];
      char final = stem[sn - 1];
      bool is_nasal = (penult == 'n' || penult == 'm' || penult == 'N' ||
                       penult == 'Y' || penult == 'R');
      bool is_stop = (final == 'k' || final == 'K' || final == 'g' || final == 'G' ||
                      final == 'c' || final == 'C' || final == 'j' || final == 'J' ||
                      final == 'w' || final == 'W' || final == 'q' || final == 'Q' ||
                      final == 't' || final == 'T' || final == 'd' || final == 'D' ||
                      final == 'p' || final == 'P' || final == 'b' || final == 'B');
      if (is_nasal && is_stop) {
        /* Drop the nasal. */
        stem[sn - 2] = final;
        stem[sn - 1] = '\0';
      }
    }
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
  /* Gaṇa-9 (kryādi) śnā vikaraṇa weak-form alternations per 6.4.113
     śnābhyastayoḥ ātaḥ: in weak forms (kit/ṅit endings), the
     vikaraṇa-A is replaced by I before consonant-initial endings.
     For vowel-initial endings the A drops entirely. The append
     above places "nA" at the end; we trim/replace here. */
  if (gana == 9 && !is_strong) {
    size_t sl = strlen(stem);
    if (sl > 0 && stem[sl - 1] == 'A') {
      /* Replace 'A' with 'I' as a default; the form-builder will
         drop the I again when concatenating a vowel-initial ending
         via the existing a+a → a sandhi (treat 'I' analogously). */
      stem[sl - 1] = 'I';
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
  bool initial_was_sa = false;
  clean_dhatu_upadesa_ex(dhatu_slp1, clean_root, sizeof(clean_root), &initial_was_sa);
  if (clean_root[0] == '\0') return false;

  bool ch_i_anubandha = has_i_anubandha(dhatu_slp1);

  /* 8.2.76 r-vor upadhāyā dīrghaḥ — an ik-upadhā (i/u/ṛ) before
     r/v + final-consonant context lengthens to I/U/F. Applies to
     roots like turv, hurC, mūrch, sphurC. Fires once at clean-root
     so all downstream derivations see the long-vowel form. */
  {
    size_t cl = strlen(clean_root);
    for (size_t i = 1; i + 1 < cl; i++) {
      char vowel = clean_root[i];
      char next = clean_root[i + 1];
      bool is_short_ik = (vowel == 'i' || vowel == 'u' || vowel == 'f');
      bool next_is_rv = (next == 'r' || next == 'v');
      if (is_short_ik && next_is_rv && i + 2 < cl) {
        /* Position i+2 should be another consonant for the upadhā
           context to apply. */
        char after = clean_root[i + 2];
        bool after_is_cons = !varna_is_vowel(after);
        if (after_is_cons) {
          /* Lengthen. */
          if (vowel == 'i') clean_root[i] = 'I';
          else if (vowel == 'u') clean_root[i] = 'U';
          else if (vowel == 'f') clean_root[i] = 'F';
          break;
        }
      }
    }
  }
  /* 6.1.73 che ca — a vowel before ch (C in SLP1) takes a prosthetic
     't' realized as 'c' (8.4.40 stoḥ ścunā ścuḥ), so an internal "VC"
     becomes "VcC". Apply at clean-root time so the doubled cluster
     is visible to all downstream upadhā-laghu checks and vrddhi/
     guṇa decisions. yuC → yucC → no guṇa, lacC → no vrddhi.
     Skip for i-anubandha roots — those will get a num augment
     before the final consonant (7.1.58 idito num), making the cluster
     "nC" / "YC" rather than "VC". The nasal already serves the
     buffering function that ch-doubling would. */
  if (!ch_i_anubandha) {
    char rebuilt[64] = {0};
    size_t cl = strlen(clean_root), oi = 0;
    for (size_t i = 0; i < cl && oi + 2 < sizeof(rebuilt); i++) {
      char c = clean_root[i];
      if (c == 'C' && i > 0) {
        char prev = clean_root[i - 1];
        bool vowel = (prev == 'a' || prev == 'A' || prev == 'i' || prev == 'I' ||
                      prev == 'u' || prev == 'U' || prev == 'f' || prev == 'F' ||
                      prev == 'x' || prev == 'X' || prev == 'e' || prev == 'o' ||
                      prev == 'E' || prev == 'O');
        bool already_doubled = (i >= 1 && clean_root[i - 1] == 'c');
        if (vowel && !already_doubled) {
          rebuilt[oi++] = 'c';
        }
      }
      rebuilt[oi++] = c;
    }
    rebuilt[oi] = '\0';
    strncpy(clean_root, rebuilt, sizeof(clean_root) - 1);
    clean_root[sizeof(clean_root) - 1] = '\0';
  }
  /* Periphrastic LIT short-circuit: form already contains the
     auxiliary, skip the ending-concat path. */
  bool periphrastic_lit_used = false;
  /* LRT/LUT/LRN/ASIRLIN introduce their own augments (sya, tā,
     a-sya, yA) that replace the gaṇa vikaraṇa. */
  bool skip_vikarana = (lakara == ASH_LRT || lakara == ASH_LUT ||
                        lakara == ASH_LRN || lakara == ASH_ASHIRLIM ||
                        lakara == ASH_LIT || lakara == ASH_LUN);
  /* 1.2.10 halaḥ śnaḥ śānajbhyām — kit applies only to ASHIRLIM-P
     (yāsuṭ-augmented). ASHIRLIM-Ā is sīsaT-augmented and not kit, so
     guṇa fires normally for Ā. */
  bool block_guna_completely = (lakara == ASH_ASHIRLIM && pd == ASH_PARASMAI);
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
    /* LIT_OVERRIDE_TABLE: closed list of irregulars where the LIT
       1eka/3eka P form bypasses the normal reduplication/vrddhi/
       ec→ay pipeline. Checked FIRST so it wins over the periphrastic
       detection. */
    struct LitOverride { const char *root; const char *aux_form; };
    static const struct LitOverride LIT_OVERRIDE_TABLE[] = {
      /* yajādi vac-samprasāraṇa, LIT 1eka/3eka P */
      {"vad", "uvAda"}, {"vac", "uvAca"}, {"vas", "uvAsa"},
      {"vap", "uvApa"}, {"vah", "uvAha"}, {"yaj", "iyAja"},
      {"ji", "jigAya"}, {"f",  "Ara"},
      {"aj", "vivAya"},
      {"dew", "daDO"}, {"dEp", "dadO"}, {"dAR", "dadO"},
      {"Dew", "daDO"},
      {"saR", "sasAna"},
      {"pER", "pipERa"}, {"prER", "piprERa"},
      {"CadiH", "cacCAda"},
      /* sūrkṣ (initial s, was-not-z): susūrkṣa; ṣūrkṣya (initial ṣ): suzūkṣya.
         Distinguish via the discriminator suffix '@1' for was-z. */
      {"sUrkz", "susUrkza"},
      {"sUrkzy@1", "suzUkzya"},  /* initial was-ṣ */
      {"sUrkzy",   "susUrkzya"}, /* initial was-s */
      {"sasj", "sasajja"},
      {"sTiv", "tizWeva"},
      {"kzIv", "cikzeva"},
      /* Periphrastic LIT with extended stem for specific roots. */
      {"gup", "gopAyAYcakAra"},
      {"DUp", "DUpAyAYcakAra"},
      {"urv", "UrvAYcakAra"},
      {"kit", "cikitsAYcakAra"},
      {"uC",  "uYCAYcakAra"},     /* uCi~ — i-anubandha → num+parasavarṇa */
      {"ucC", "ucCAYcakAra"},     /* uCI~ — post-ch-doubling */
      {"uz",  "uvoza"},
      {NULL, NULL},
    };
    bool strong_p_eka_check = (pd == ASH_PARASMAI && v == ASH_EKAVACANA &&
                              (p == ASH_PRATHAMA || p == ASH_UTTAMA));
    if (strong_p_eka_check) {
      /* Build a discriminated key for roots whose original initial
         was ṣ: append "@1" so the override can distinguish identical
         post-clean roots that came from different originals (e.g.
         sUrkzya~ vs zUrkzya~ both clean to "sUrkzy"). */
      char keyed[80];
      if (initial_was_sa) {
        snprintf(keyed, sizeof(keyed), "%s@1", clean_root);
      } else {
        snprintf(keyed, sizeof(keyed), "%s", clean_root);
      }
      /* Try keyed first (with @1 discriminator for was-ṣ roots);
         fall back to plain clean_root. */
      const char *match_form = NULL;
      for (size_t i = 0; LIT_OVERRIDE_TABLE[i].root; i++) {
        if (strcmp(keyed, LIT_OVERRIDE_TABLE[i].root) == 0) {
          match_form = LIT_OVERRIDE_TABLE[i].aux_form;
          break;
        }
      }
      if (!match_form) {
        for (size_t i = 0; LIT_OVERRIDE_TABLE[i].root; i++) {
          if (strcmp(clean_root, LIT_OVERRIDE_TABLE[i].root) == 0) {
            match_form = LIT_OVERRIDE_TABLE[i].aux_form;
            break;
          }
        }
      }
      if (match_form) {
        strncpy(stem, match_form, sizeof(stem) - 1);
        stem[sizeof(stem) - 1] = '\0';
        periphrastic_lit_used = true;
        goto lit_done;
      }
    }
    /* 3.1.35-36 periphrastic LIT (LIT-paribhāṣā): for vowel-initial
       roots with i-anubandha (3.1.36 ij-ādeśca gurumato'naṛcchaḥ —
       which after num insertion have a guru upadhā), and for the
       closed list of long-vowel-initial roots (oKf, ejf, UWa, Uza,
       Iza, Irkzya, Irzya, urvI, Uza), the LIT is built periphrastically
       as: stem + Am + LIT-of-kṛ. We emit the cakāra-form (first of
       the three accepted alternates). */
    bool lit_periphrastic = false;
    /* 3.1.36 ijādeśca gurumato'naṛcchaḥ: vowel-initial roots whose
       first vowel is i/I/u/U/e/o/E/O (NOT 'a') with i-anubandha use
       periphrastic LIT. 'a'-initial roots (ati, adi, arda) stay on
       the An-abhyāsa reduplication path. */
    if (clean_root[0] != '\0') {
      char first = clean_root[0];
      /* 3.1.36 ijādeśca gurumato'naṛcchaḥ — vowel-initial roots
         (excluding 'a') whose first vowel is long (E, O, I, U, F)
         OR whose i-anubandha will introduce num (making upadhā guru)
         take periphrastic LIT. */
      bool long_initial = (first == 'I' || first == 'U' || first == 'F' ||
                           first == 'e' || first == 'o' ||
                           first == 'E' || first == 'O');
      bool short_iu_with_anubandha = i_anubandha &&
                                     (first == 'i' || first == 'u');
      if (long_initial || short_iu_with_anubandha) {
        lit_periphrastic = true;
      }
    }
    static const char *const PERIPH_LIT_LONG_VOWEL[] = {
      "oK", "ej", "UW", "Uz", "Iz", "Irkzy", "Irzy", "urv", "oR",
      "kakKa", "gup", "DUp", "paR", "pan", "kit", "dAn", "SAn", "uC",
      "iv", "ukz", "uz",
      NULL
    };
    for (size_t i = 0; PERIPH_LIT_LONG_VOWEL[i]; i++) {
      if (strcmp(clean_root, PERIPH_LIT_LONG_VOWEL[i]) == 0) {
        lit_periphrastic = true; break;
      }
    }

    /* (LIT_OVERRIDE_TABLE moved earlier — fires before periphrastic.) */
    if (lit_periphrastic) {
      /* Compute the stem (with num inserted for i-anubandha). */
      char per_stem[64];
      strncpy(per_stem, clean_root, sizeof(per_stem) - 1);
      per_stem[sizeof(per_stem) - 1] = '\0';
      if (i_anubandha) {
        size_t cl = strlen(per_stem);
        size_t insert_at = cl;
        for (size_t i = cl; i > 0; i--) {
          if (!varna_is_vowel(per_stem[i - 1])) { insert_at = i - 1; break; }
        }
        if (cl + 1 < sizeof(per_stem)) {
          memmove(per_stem + insert_at + 1, per_stem + insert_at, cl - insert_at + 1);
          per_stem[insert_at] = 'n';
        }
      }
      /* Auxiliary kṛ-LIT forms keyed by purusha/vacana for P. */
      static const char *const AUX_P[9] = {
        "cakAra", "cakratuH", "cakruH",
        "cakarTa", "cakraTuH", "cakra",
        "cakAra", "cakfva", "cakfma",
      };
      static const char *const AUX_A[9] = {
        "cakre", "cakrAte", "cakrire",
        "cakfze", "cakrATe", "cakfQve",
        "cakre", "cakfvahe", "cakfmahe",
      };
      int idx = (int)p * 3 + (int)v;
      const char *aux = (pd == ASH_PARASMAI) ? AUX_P[idx] : AUX_A[idx];
      /* For vowel-initial + i-anubandha stems, the connector is "AY"
         (Am with parasavarṇa anusvāra before palatal c-). If the stem
         ends in 'a', 6.1.101 savarṇa-dīrgha collapses a + A → A. */
      size_t psl = strlen(per_stem);
      if (psl > 0 && per_stem[psl - 1] == 'a') per_stem[psl - 1] = '\0';
      char form_per[128];
      snprintf(form_per, sizeof(form_per), "%sAY%s", per_stem, aux);
      strncpy(stem, form_per, sizeof(stem) - 1);
      stem[sizeof(stem) - 1] = '\0';
      periphrastic_lit_used = true;
      /* Skip the rest of the LIT reduplication branch. */
      goto lit_done;
    }
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
      /* 7.1.58 idito num dhātor — insert num AFTER the upadhā vowel
         (i.e. before the final consonant cluster, not just the last
         consonant). For roots like kAkz, this gives kAnkz which then
         parasavarṇas to kANkz before velars (8.4.58). */
      size_t cl = strlen(augmented_root);
      size_t last_vowel_pos = cl;  /* not found */
      for (size_t i = cl; i > 0; i--) {
        if (varna_is_vowel(augmented_root[i - 1])) {
          last_vowel_pos = i - 1;
          break;
        }
      }
      size_t insert_at = (last_vowel_pos < cl) ? last_vowel_pos + 1 : cl;
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

    /* 6.1.73 che ca — apply ch-doubling at the abhyāsa→root boundary
       too: when the abhyāsa's final vowel meets a C-initial root
       (e.g. Cam → caCam → cacCam). */
    {
      char rebuilt[64] = {0};
      size_t rl = strlen(reduped), oi = 0;
      for (size_t i = 0; i < rl && oi + 2 < sizeof(rebuilt); i++) {
        char c = reduped[i];
        if (c == 'C' && i > 0) {
          char prev = reduped[i - 1];
          bool vowel = (prev == 'a' || prev == 'A' || prev == 'i' || prev == 'I' ||
                        prev == 'u' || prev == 'U' || prev == 'f' || prev == 'F' ||
                        prev == 'x' || prev == 'X' || prev == 'e' || prev == 'o' ||
                        prev == 'E' || prev == 'O');
          bool already_doubled = (i >= 1 && reduped[i - 1] == 'c');
          if (vowel && !already_doubled) rebuilt[oi++] = 'c';
        }
        rebuilt[oi++] = c;
      }
      rebuilt[oi] = '\0';
      if (strcmp(reduped, rebuilt) != 0) {
        /* If we inserted, the position of root_start may have shifted
           if the insertion was before it. Recompute. */
        size_t shift = strlen(rebuilt) - rl;
        if (shift > 0) {
          /* The 'c' was inserted at some position i; if i < root_start
             we need to bump root_start. Simpler: rescan. */
          strncpy(reduped, rebuilt, sizeof(reduped) - 1);
          reduped[sizeof(reduped) - 1] = '\0';
          root_start = 0;
          while (root_start < strlen(reduped) && !varna_is_vowel(reduped[root_start])) root_start++;
          root_start++;
          if (reduped[0] && varna_is_vowel(reduped[0]) && reduped[root_start] == 'n' &&
              varna_is_vowel(reduped[root_start + 1])) {
            root_start++;
          }
        }
      }
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
        /* P 1eka/3eka are ñit/ṇit (Ral) so trigger 7.2.115/116 vṛddhi
           and 7.3.86 guṇa. Ā 1eka/3eka are kit/ṅit, so 1.1.5 blocks
           guṇa/vṛddhi. P 2eka (Tal) is ṇit, gets guṇa but not vṛddhi.
           Ā 2eka (TAs) is kit, blocks. */
        bool strong13 = (p != ASH_MADHYAMA) && (pd == ASH_PARASMAI);
        bool do_change = false;
        bool do_vrddhi = false;
        /* In LIT, Ā endings (te, AtAm, Ja, TAs, ize, ATAm, iDve,
           e, ivahe, imahe) are kit per 1.2.4 vac-class, blocking
           1.1.5 guṇa/vṛddhi. Skip the whole strong-form logic for
           Ā. P endings keep the normal upadhā-aware path below. */
        bool lit_parasmai = (pd == ASH_PARASMAI);
        if (!lit_parasmai) {
          /* Ā in LIT: bare reduplicated stem, no upadhā change. */
        } else if (vowel_final && upadha_short) {
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
            /* 7.3.86 guṇa for laghu i/u/f upadhā in all eka forms (P only). */
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

    /* 8.3.59 ādeśapratyayoḥ: only fires when the dhātu-initial 's'
       is an ādeśa from ṣ (per 6.1.64). For roots that originally
       began with plain 's' (skud, smṛ, snā), no ṣatva in abhyāsa
       restoration. */
    if (initial_was_sa) {
      sandhi_apply_satva(reduped, root_start);
    }

    /* 7.2.10 ekāca upadeśe — aniṭ roots refuse the iṭ-augment that the
       LIT_PARASMAI table builds into iTa / iva / ima / iDve. Strip the
       leading 'i' for the closed list of aniṭ roots BEFORE the
       saṃprasāraṇa check, so the f→r rule only fires when the (now
       trimmed) ending is genuinely vowel-initial. */
    static const char *const LIT_ANIT_ROOTS[] = {
      /* ṛ-final aniṭ class: kṛ, hṛ, vṛ, sṛ */
      "kf", "hf", "vf", "sf",
      /* short-vowel aniṭ vowel-finals (śru, stu, nī) */
      "Sru", "stu", "nI",
      NULL
    };
    bool lit_anit = false;
    for (size_t i = 0; LIT_ANIT_ROOTS[i]; i++) {
      if (strcmp(clean_root, LIT_ANIT_ROOTS[i]) == 0) { lit_anit = true; break; }
    }
    if (lit_anit && t->clean[0] == 'i' && t->clean[1] != '\0') {
      static char lit_anit_override[16];
      strncpy(lit_anit_override, t->clean + 1, sizeof(lit_anit_override) - 1);
      lit_anit_override[sizeof(lit_anit_override) - 1] = '\0';
      static TingEntry lit_anit_local;
      lit_anit_local = *t;
      lit_anit_local.clean = lit_anit_override;
      t = &lit_anit_local;
    }

    /* 6.1.108 saṃprasāraṇāc ca + 6.4.83 oḥ supi: in LIT weak forms
       of ṛ-final roots (kṛ → cakṛ-, smṛ → sasmṛ-, hṛ → jahṛ-),
       the root-final ṛ surfaces as r before vowel-initial endings. */
    if (v != ASH_EKAVACANA) {
      size_t rl = strlen(reduped);
      if (rl > 0 && reduped[rl - 1] == 'f' && varna_is_vowel(t->clean[0])) {
        reduped[rl - 1] = 'r';
      }
    }

    /* 6.4.98 gama-hana-jana-Khana-Gasāṃ lopaḥ kṅiti — anudātta-upadeśa
       roots (gam, han, jan, khan, ghas) drop their medial 'a' in LIT
       weak forms (non-eka) — strong-eka stays jagAma / jahAna /
       cakAra-style. The drop applies only when the root portion of
       the reduplicated stem still contains a residual 'a' before the
       final consonant. */
    static const char *const GAM_CLASS[] = {
      "gam", "han", "jan", "Kan", "Gas", NULL
    };
    bool is_gam_class = false;
    for (size_t gi = 0; GAM_CLASS[gi]; gi++) {
      if (strcmp(clean_root, GAM_CLASS[gi]) == 0) { is_gam_class = true; break; }
    }
    if (is_gam_class && v != ASH_EKAVACANA) {
      size_t rl = strlen(reduped);
      /* Find the residual 'a' between root_start and the final
         consonant, and delete it. The root_start vowel is the
         abhyāsa, so skip past it. */
      if (rl > root_start + 1) {
        /* Locate the 'a' in the root portion (it sits between two
           consonants like in "jagam" → 'a' at index 3, root_start=2). */
        for (size_t i = root_start; i < rl - 1; i++) {
          if (reduped[i] == 'a' && !varna_is_vowel(reduped[i + 1])) {
            memmove(reduped + i, reduped + i + 1, rl - i);
            break;
          }
        }
      }
    }

    /* 6.4.64 ato lopa iṭi ca + 7.4.40-41 — A/E-final dhātus in LIT:
       in strong 1eka/3eka P, the root-final long vowel becomes O
       (au) and the ending 'a' is absorbed (jaglO, SaSrO). In weak
       forms the final long vowel is dropped (SaSratuH, jaglatuH).
       Detect by clean root ending in 'A' or 'E'. */
    {
      size_t cl = strlen(clean_root);
      char cf = (cl > 0) ? clean_root[cl - 1] : 0;
      bool ae_final = (cf == 'A' || cf == 'E');
      if (ae_final) {
        size_t rl = strlen(reduped);
        bool strong13_p = (v == ASH_EKAVACANA && p != ASH_MADHYAMA &&
                           pd == ASH_PARASMAI);
        if (rl > 0 && (reduped[rl - 1] == 'A' || reduped[rl - 1] == 'E')) {
          if (strong13_p) {
            reduped[rl - 1] = 'O';
            /* Mark for ending-absorb: the 'a' of LIT 1eka/3eka P is
               already in the form via t->clean; we'll suppress it. */
            periphrastic_lit_used = true;  /* reuses the skip-ending flag */
          } else {
            /* Weak forms: drop the final vowel entirely. */
            reduped[rl - 1] = '\0';
          }
        }
      }
    }

    /* 6.4.77 acijñiti — iyaṅ/uvaṅ-ādeśa: at the abhyāsa→post-guṇa-root
       boundary, short i/u + V becomes iy/uv + V. Applies to vowel-
       initial single-cons roots like uK, iK, uW, iw whose reduplicate
       did NOT merge and which got guṇa on the root vowel.
       For "uoK" we should produce "uvoK"; for "ieK" → "iyeK".
       Only scan the boundary at position root_start-1 (the abhyāsa
       vowel slot). */
    if (root_start >= 1) {
      size_t pre = root_start - 1;
      char abh = reduped[pre];
      char next = reduped[pre + 1];
      bool short_iu = (abh == 'i' || abh == 'u');
      bool next_vowel = (next == 'a' || next == 'A' || next == 'i' || next == 'I' ||
                         next == 'u' || next == 'U' || next == 'e' || next == 'o' ||
                         next == 'E' || next == 'O' || next == 'f' || next == 'F' ||
                         next == 'x' || next == 'X');
      if (short_iu && next_vowel) {
        /* Insert 'y' (after i) or 'v' (after u) at position pre+1. */
        char glide = (abh == 'i') ? 'y' : 'v';
        size_t rl = strlen(reduped);
        if (rl + 1 < sizeof(reduped)) {
          memmove(reduped + pre + 2, reduped + pre + 1, rl - pre);
          reduped[pre + 1] = glide;
        }
      }
    }

    log_single_term_change(ctx_out, 601008, stem, reduped, "liwi DAtor anabhyAsasya");
    strncpy(stem, reduped, sizeof(stem) - 1);
    stem[sizeof(stem) - 1] = '\0';
  lit_done:;
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
    } else if (stem[i] == 'n' && (nxt == 's' || nxt == 'S' || nxt == 'z' || nxt == 'h')) {
      /* 8.3.24 naś ca a-pada-antasya jhali — n before sibilant /
         h (jhal class) becomes anusvāra (M). e.g. dfh + i-anubandha
         → dfnh → dfMh; Sansu → Sansa → SaMsa. */
      stem[i] = 'M';
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
  /* LUN root-aorist (cli replaced by zero per 3.1.55 / 3.1.56 puṣādi-
     and ad-class roots): the form is just a-augment + root + secondary
     endings (LAN-style), no guṇa, no iṣ-marker. Vowel-initial endings
     get a v-augment when the root ends in a vowel. The list is closed;
     for these roots we discard any guṇa that apply_class_transform
     applied and reset the stem to the bare clean root. */
  bool lun_root_aorist = false;
  if (lakara == ASH_LUN) {
    static const char *const LUN_ROOT_AORIST_ROOTS[] = {
      "BU", NULL
    };
    for (size_t i = 0; LUN_ROOT_AORIST_ROOTS[i]; i++) {
      if (strcmp(clean_root, LUN_ROOT_AORIST_ROOTS[i]) == 0) {
        lun_root_aorist = true; break;
      }
    }
  }
  if (lun_root_aorist) {
    strncpy(stem, clean_root, sizeof(stem) - 1);
    stem[sizeof(stem) - 1] = '\0';
    used_guna = false;
  }
  if ((lakara == ASH_LRT || lakara == ASH_LUT || lakara == ASH_LRN ||
       lakara == ASH_LUN) &&
      !block_guna_completely && !i_anubandha && !lun_root_aorist) {
    bool has_unstrong = false;
    size_t unstrong_pos = 0;
    for (size_t i = 0; stem[i]; i++) {
      char c = stem[i];
      if (c == 'i' || c == 'u' || c == 'f' || c == 'x') {
        has_unstrong = true;
        unstrong_pos = i;
        break;
      }
    }
    bool laghu = true;
    if (has_unstrong) {
      /* 7.3.86 pugantalaghūpadhasya: guṇa only fires when the short
         vowel is in a laghu position — followed by at most one
         consonant before the next vowel or end of stem. For LUN
         specifically, this gates the over-eager guṇa we used to apply
         to roots like bukk (u + kk = guru) and SunDa (u + nD = guru). */
      size_t cons_after = 0;
      for (size_t i = unstrong_pos + 1; stem[i]; i++) {
        if (stem[i] == 'a' || stem[i] == 'i' || stem[i] == 'u' ||
            stem[i] == 'A' || stem[i] == 'I' || stem[i] == 'U' ||
            stem[i] == 'f' || stem[i] == 'F' || stem[i] == 'x' ||
            stem[i] == 'X' || stem[i] == 'e' || stem[i] == 'o' ||
            stem[i] == 'E' || stem[i] == 'O') break;
        cons_after++;
      }
      laghu = (cons_after <= 1);
    }
    if (has_unstrong && laghu && !used_guna) {
      replace_first_vowel(stem, false);
      used_guna = true;
    }
  }
  /* Override the LUN ending for root-aorist class. Endings are LAN's
     secondary endings; vowel-initial endings (1bahu "an" / 3eka "am")
     get a v-augment after vowel-final roots like BU, Sru. */
  static const TingEntry LUN_ROOT_AORIST_P[9] = {
    {ASH_LUN, ASH_PRATHAMA, ASH_EKAVACANA,  ASH_PARASMAI, "tip", "t",   SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_PRATHAMA, ASH_DVIVACANA,  ASH_PARASMAI, "tas", "tAm", SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_PRATHAMA, ASH_BAHUVACANA, ASH_PARASMAI, "Ji",  "van", SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_MADHYAMA, ASH_EKAVACANA,  ASH_PARASMAI, "sip", "H",   SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_MADHYAMA, ASH_DVIVACANA,  ASH_PARASMAI, "Tas", "tam", SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_MADHYAMA, ASH_BAHUVACANA, ASH_PARASMAI, "Ta",  "ta",  SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_UTTAMA,   ASH_EKAVACANA,  ASH_PARASMAI, "mip", "vam", SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_UTTAMA,   ASH_DVIVACANA,  ASH_PARASMAI, "vas", "va",  SJ_TING | SJ_PARASMAIPADA},
    {ASH_LUN, ASH_UTTAMA,   ASH_BAHUVACANA, ASH_PARASMAI, "mas", "ma",  SJ_TING | SJ_PARASMAIPADA},
  };
  if (lun_root_aorist && pd == ASH_PARASMAI) {
    int idx = (int)p * 3 + (int)v;
    if (idx >= 0 && idx < 9) t = &LUN_ROOT_AORIST_P[idx];
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
  if (periphrastic_lit_used) {
    /* Periphrastic stem already includes the auxiliary; skip the rest. */
    goto finalize;
  }
  /* 7.1.4 ad-abhyastāt — the JhI ending (anti) becomes "ati" for ad-
     and abhyasta-class verbs (gana 3 reduplicated forms). Also strip
     the medial 'n' from JhI-derived endings (anta → ata in LAN/LOT
     prathama-bahu and similar slots). */
  if (gana == 3 && p == ASH_PRATHAMA && v == ASH_BAHUVACANA) {
    static char abhyasta_jhi_override[16];
    /* Drop the 'n' from "anti" → "ati", "antu" → "atu",
       "an" → "uH" for LIT (special), "anta" → "ata" for LAN. */
    const char *c = t->clean;
    if (c && c[0] == 'a' && c[1] == 'n') {
      abhyasta_jhi_override[0] = 'a';
      strncpy(abhyasta_jhi_override + 1, c + 2, sizeof(abhyasta_jhi_override) - 2);
      abhyasta_jhi_override[sizeof(abhyasta_jhi_override) - 1] = '\0';
      static TingEntry abhyasta_local;
      abhyasta_local = *t;
      abhyasta_local.clean = abhyasta_jhi_override;
      t = &abhyasta_local;
    }
  }
  /* Athematic vowel-final + vowel-initial ending: insert a glide
     (v after u/U, y after i/I, r after ṛ/ṝ) so the surface keeps the
     root vowel. ru + anti → ruvanti, vI + anti → viyanti (with I→i),
     yu + anti → yuvanti. This corresponds to the 6.4.77 acijñiti
     iyaṅ/uvaṅ + the underlying root vowel surfacing as a short.
     Skipped for LIT — the reduplication branch handles its own
     stem→ending boundary (saṃprasāraṇa, v-augment, ec→ay).
     For gaṇa-3 we instead apply 6.1.77 yaṇ-ādeśa: short u/i/ṛ at
     stem-end REPLACED by v/y/r before the vowel-initial ending. */
  if (gana == 3 && lakara != ASH_LIT) {
    size_t fl = strlen(form);
    char stem_final = fl > 0 ? form[fl - 1] : 0;
    char ending_initial = t->clean[0];
    bool ending_vowel = (ending_initial == 'a' || ending_initial == 'A' ||
                         ending_initial == 'i' || ending_initial == 'I' ||
                         ending_initial == 'u' || ending_initial == 'U');
    if (ending_vowel) {
      if (stem_final == 'u') { form[fl - 1] = 'v'; }
      else if (stem_final == 'i') { form[fl - 1] = 'y'; }
      else if (stem_final == 'f') { form[fl - 1] = 'r'; }
    }
  } else if ((gana == 2 || gana == 5 || gana == 7 || gana == 8) &&
             lakara != ASH_LIT) {
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
    } else if (gana == 9 && stem_final == 'I' &&
               (ending_initial == 'a' || ending_initial == 'A')) {
      /* Gaṇa-9 weak: śnā-I drops before vowel-initial endings,
         leaving just the n. krIRI + anti → krIRanti. */
      form[fl - 1] = '\0';
      strcat(form, t->clean);
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

  /* Cluster sandhi at the stem→ending boundary for athematic gaṇa-7
     and similar voiced-asp / voiced-palatal / voiced-stop finals.
     Rules collapsed into surface-level rewrites:
       8.2.40 jhalāṃ jaś jhaśi   — voiceless after voiced asp → voiced
       8.4.41 stho ścunā ścuḥ    — palatal+t → palatal-aspirate after voiced
       8.4.55 khari ca            — voiced before voiceless → voiceless
       8.2.30 coḥ kuḥ            — palatal at end → velar
       aspiration-migration: Vasp + t/T → V + asp-stop  (D+t → dD,
         G+t → gG, B+t → bB, J+t → jJ, Q+t → qQ).
     Applied as a single post-form scan; safe to no-op on other roots. */
  if (gana == 7) {
    char before_cluster[128] = {0};
    strncpy(before_cluster, form, sizeof(before_cluster) - 1);
    /* Scan for the specific clusters that result from gaṇa-7 śnam
       infix + ending. Apply at most one substitution per scan to
       keep the rules predictable. */
    for (size_t i = 0; form[i] && form[i + 1]; i++) {
      char a = form[i], b = form[i + 1];
      /* Voiced aspirate + t → aspiration migrate (8.2.40 + 8.4.41). */
      if (b == 't' && (a == 'D' || a == 'G' || a == 'B' ||
                       a == 'J' || a == 'Q')) {
        char unasp = 0;
        switch (a) {
          case 'D': unasp = 'd'; break;
          case 'G': unasp = 'g'; break;
          case 'B': unasp = 'b'; break;
          case 'J': unasp = 'j'; break;
          case 'Q': unasp = 'q'; break;
        }
        form[i] = unasp;
        form[i + 1] = a;  /* aspirate moves right */
      } else if (b == 'T' && (a == 'D' || a == 'G' || a == 'B' ||
                              a == 'J' || a == 'Q')) {
        /* 2dvi / 2bahu cluster — D+T → dD (T elides into the asp). */
        char unasp = 0;
        switch (a) {
          case 'D': unasp = 'd'; break;
          case 'G': unasp = 'g'; break;
          case 'B': unasp = 'b'; break;
          case 'J': unasp = 'j'; break;
          case 'Q': unasp = 'q'; break;
        }
        form[i] = unasp;
        form[i + 1] = a;
      } else if (b == 's' && (a == 'D' || a == 'G' || a == 'B')) {
        /* 8.4.55 khari ca: voiced asp before voiceless s →
           corresponding voiceless stop. D+s → t+s, B+s → p+s,
           G+s → k+s. */
        char devoiced = 0;
        switch (a) {
          case 'D': devoiced = 't'; break;
          case 'G': devoiced = 'k'; break;
          case 'B': devoiced = 'p'; break;
        }
        form[i] = devoiced;
      } else if ((b == 't' || b == 'T' || b == 's') &&
                 (a == 'd' || a == 'g' || a == 'b')) {
        /* 8.4.55 khari ca: plain voiced before voiceless → voiceless.
           d+t → t+t, g+t → k+t, b+t → p+t. Same for T and s. */
        char devoiced = 0;
        switch (a) {
          case 'd': devoiced = 't'; break;
          case 'g': devoiced = 'k'; break;
          case 'b': devoiced = 'p'; break;
        }
        form[i] = devoiced;
      } else if ((b == 's' || b == 't' || b == 'T') &&
                 (a == 'c' || a == 'j')) {
        /* 8.2.30 coḥ kuḥ + 8.4.55 khari ca: palatal → velar before
           consonant; before a voiceless (s/t/T) the velar is voiceless
           (c → k, j → k). Before a voiced consonant (handled
           elsewhere) it'd be g.
           Also re-classify preceding palatal nasal Y → velar N. */
        form[i] = 'k';  /* both c and j go to k before voiceless */
        if (i > 0 && form[i - 1] == 'Y') form[i - 1] = 'N';
      }
    }
    if (strcmp(before_cluster, form) != 0) {
      /* Re-apply 8.3.59 ṣatva so any newly-exposed k+s clusters get
         k+ṣ (riRaksi → riRakzi). */
      sandhi_apply_satva(form, 0);
      log_single_term_change(ctx_out, 802040, before_cluster, form,
                             "jhalAM jaS jhaSi + aspiration migration");
    }
  }
finalize:;

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
  if (lakara == ASH_LAN || lakara == ASH_LRN || lakara == ASH_LUN) {
    char augmented[128] = {0};
    char before[128] = {0};
    strncpy(before, form, sizeof(before) - 1);
    char first = form[0];
    char second = form[1];
    bool followed_by_nasal = (second == 'n' || second == 'm' || second == 'N' ||
                              second == 'Y' || second == 'R');
    if (first == 'a') {
      /* a + a → A (savarṇa-dīrgha 6.1.101). */
      augmented[0] = 'A';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if ((first == 'i' || first == 'I') && i_anubandha && followed_by_nasal) {
      /* Vowel-initial i-anubandha (idi → ind): guṇa fires first (i→e),
         then a-augment + e = ai (E) per 6.1.90 vṛddhi-of-augment. */
      augmented[0] = 'E';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if ((first == 'u' || first == 'U') && i_anubandha && followed_by_nasal) {
      /* Same pattern with u-initial roots: a + o → au (O) by 6.1.90. */
      augmented[0] = 'O';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if (first == 'i' || first == 'I') {
      /* a + i/I → e (guṇa 6.1.87). */
      augmented[0] = 'e';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if (first == 'u' || first == 'U') {
      /* a + u/U → o (guṇa 6.1.87). */
      augmented[0] = 'o';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if (first == 'e' || first == 'E') {
      /* 6.1.90 āṭ ca — a + e/ai → ai (vṛddhi). */
      augmented[0] = 'E';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if (first == 'o' || first == 'O') {
      /* 6.1.90 āṭ ca — a + o/au → au (vṛddhi). */
      augmented[0] = 'O';
      strncpy(augmented + 1, form + 1, sizeof(augmented) - 2);
    } else if (first == 'f' || first == 'F') {
      /* 6.1.90 āṭ ca — a + ṛ → ār (vṛddhi). */
      augmented[0] = 'A';
      augmented[1] = 'r';
      strncpy(augmented + 2, form + 1, sizeof(augmented) - 3);
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
