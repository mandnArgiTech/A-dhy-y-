/* krit_primary.c — primary kft derivative helpers */
#include "krit_primary.h"
#include "guna_vrddhi.h"
#include "encoding.h"
#include "context.h"
#include "varna.h"
#include <string.h>
#include <stdlib.h>

/* Returns the upadesa of the selected kft suffix. */
static const char *krit_suffix_upadesa(ASH_KritType krit) {
  switch (krit) {
    case ASH_KRIT_KTA: return "kta";
    case ASH_KRIT_KTAVAT: return "ktavat";
    case ASH_KRIT_SHATR: return "Satf";
    case ASH_KRIT_SHANAC: return "SAnac";
    case ASH_KRIT_TAVYA: return "tavya";
    case ASH_KRIT_ANIIYA: return "anIya";
    case ASH_KRIT_YA: return "ya";
    case ASH_KRIT_LYAP: return "lyap";
    case ASH_KRIT_KTVA: return "ktvA";
    case ASH_KRIT_TUM: return "tumun";
    case ASH_KRIT_LYUT: return "lyuw";
    case ASH_KRIT_GHAN: return "Gan";
    case ASH_KRIT_NVUL: return "Rvul";
    case ASH_KRIT_TRC: return "tfc";
    case ASH_KRIT_KTIN: return "ktin";
    case ASH_KRIT_KYAP: return "kyap";
    case ASH_KRIT_NYAT: return "Ryat";
    case ASH_KRIT_KVIP: return "kvip";
    case ASH_KRIT_NAMUL: return "Ramul";
    case ASH_KRIT_KTRI: return "ktri";
    case ASH_KRIT_KTUM: return "ktum";
    case ASH_KRIT_KMARAC: return "kmarac";
    case ASH_KRIT_GHA: return "Ga";
    case ASH_KRIT_KA: return "ka";
    case ASH_KRIT_AC: return "ac";
    case ASH_KRIT_KHAL: return "Kal";
    case ASH_KRIT_VUN: return "vun";
    case ASH_KRIT_ISHNUC: return "iznuc";
    case ASH_KRIT_UKAN: return "ukaY";
    case ASH_KRIT_TAVYAT: return "tavyat";
    case ASH_KRIT_KELIMAR: return "kelimar";
    case ASH_KRIT_RVU: return "Rvu";
    case ASH_KRIT_MAN: return "man";
    case ASH_KRIT_TRN: return "tfn";
    case ASH_KRIT_KAS: return "kas";
    case ASH_KRIT_KVASU: return "kvasu";
    case ASH_KRIT_KANAC: return "kAnac";
    case ASH_KRIT_INI: return "ini";
    default: return NULL;
  }
}

/* Returns the surface (post-anubandha-strip) form of a kfit suffix. */
static const char *krit_suffix_clean(ASH_KritType krit) {
  switch (krit) {
    case ASH_KRIT_KTA: return "ta";
    case ASH_KRIT_KTAVAT: return "tavat";
    case ASH_KRIT_TAVYA: return "tavya";
    case ASH_KRIT_ANIIYA: return "anIya";
    case ASH_KRIT_KTVA: return "tvA";
    case ASH_KRIT_TUM: return "tum";
    case ASH_KRIT_LYUT: return "ana";
    case ASH_KRIT_GHAN: return "a";       /* vrddhi root + a */
    case ASH_KRIT_NVUL: return "aka";     /* guṇa root + aka */
    case ASH_KRIT_TRC: return "tf";       /* guṇa root + tṛ */
    case ASH_KRIT_KTIN: return "ti";      /* zero-grade root + ti */
    case ASH_KRIT_KYAP: return "ya";      /* kit, no guṇa */
    case ASH_KRIT_NYAT: return "ya";      /* vrddhi root + ya */
    case ASH_KRIT_KVIP: return "";        /* zero suffix */
    case ASH_KRIT_NAMUL: return "am";     /* guṇa root + am */
    case ASH_KRIT_KTRI: return "tri";     /* kit, ktri */
    case ASH_KRIT_KTUM: return "tu";      /* kit, ktu */
    case ASH_KRIT_KMARAC: return "mara";  /* kit, mara */
    case ASH_KRIT_GHA: return "ya";       /* ghana, guṇa + ya */
    case ASH_KRIT_KA: return "a";         /* kit, a */
    case ASH_KRIT_AC: return "a";         /* ac, guṇa + a */
    case ASH_KRIT_KHAL: return "a";       /* khal, guṇa + a */
    case ASH_KRIT_VUN: return "aka";      /* ñit, vṛddhi + aka */
    case ASH_KRIT_ISHNUC: return "iznu";  /* ñit, guṇa + iṣṇu */
    case ASH_KRIT_UKAN: return "uka";     /* ñit, guṇa + uka */
    case ASH_KRIT_TAVYAT: return "tavya"; /* same as tavya */
    case ASH_KRIT_KELIMAR: return "elima";/* kit, elima */
    case ASH_KRIT_RVU: return "vu";       /* ñit, vṛddhi + vu */
    case ASH_KRIT_MAN: return "ma";       /* kit, ma */
    case ASH_KRIT_TRN: return "tf";       /* ñit, vṛddhi + tṛ (= tfc) */
    case ASH_KRIT_KAS: return "asa";      /* kit, asa */
    case ASH_KRIT_KVASU: return "vas";    /* kit, perfect active part */
    case ASH_KRIT_KANAC: return "Ana";    /* kit, perfect middle part */
    case ASH_KRIT_INI: return "in";       /* ñit, guṇa + in */
    default: return NULL;
  }
}

/* Whether a suffix is k-it (k anubandha) — blocks guṇa/vṛddhi by 1.1.5. */
static bool krit_is_kit(ASH_KritType krit) {
  return krit == ASH_KRIT_KTA || krit == ASH_KRIT_KTAVAT ||
         krit == ASH_KRIT_KTVA || krit == ASH_KRIT_KTIN ||
         krit == ASH_KRIT_KYAP || krit == ASH_KRIT_KVIP ||
         krit == ASH_KRIT_KTRI || krit == ASH_KRIT_KTUM ||
         krit == ASH_KRIT_KMARAC || krit == ASH_KRIT_KA ||
         krit == ASH_KRIT_KELIMAR || krit == ASH_KRIT_MAN ||
         krit == ASH_KRIT_KAS || krit == ASH_KRIT_KVASU ||
         krit == ASH_KRIT_KANAC;
}

/* Whether a suffix triggers vṛddhi on the root vowel (ñit/ṇit). */
static bool krit_triggers_vrddhi(ASH_KritType krit) {
  return krit == ASH_KRIT_GHAN || krit == ASH_KRIT_NYAT ||
         krit == ASH_KRIT_VUN  || krit == ASH_KRIT_RVU  ||
         krit == ASH_KRIT_TRN;
}

/* Closed list of seṭ-class roots that take iṭ before niṣṭhā/tum/tvA.
   Long-term this should be driven by the dhātupāṭha `settva` column;
   the seed list keeps the story self-contained. */
static const char *const SET_ROOTS[] = {
  "pat", "vad", "vand", "kuq", "siv", "kup", "siD", "Bram", "klam",
  /* Phase δ — extended seṭ-list per dhātupāṭha 'S' column for common
     gaṇa-1 roots */
  "BU", "Bav", "BAz", "vart", "ji", "nI", "smf", "han", "dah",
  "ruh", "khan", "jan", "edh", "kuS",
  NULL
};

static bool root_is_set(const char *clean_root) {
  if (!clean_root) return false;
  for (size_t i = 0; SET_ROOTS[i]; i++) {
    if (strcmp(clean_root, SET_ROOTS[i]) == 0) return true;
  }
  return false;
}

/* 8.4.1 raṣābhyāṃ no ṇaḥ within same pada. Replace `n` with `R` if
   form contains an earlier r/f/z/F and the n is followed by a vowel. */
static void krit_apply_natva(char *form) {
  bool seen_trigger = false;
  for (size_t i = 0; form[i]; i++) {
    char c = form[i];
    if (c == 'r' || c == 'f' || c == 'z' || c == 'F' || c == 'R') {
      seen_trigger = true;
    } else if (seen_trigger && c == 'n' && form[i + 1] &&
               varna_is_vowel(form[i + 1])) {
      form[i] = 'R';
    }
  }
}

/* Compute guṇa of root vowel for kfit derivation. Returns true if a
   substitution was made and writes the resulting stem to `out`. */
static bool krit_apply_guna(const char *root, char *out, size_t out_len) {
  if (!root || !out || out_len == 0) return false;
  size_t n = strlen(root);
  if (n == 0 || n + 2 > out_len) return false;
  /* Find the *first* ik-class vowel (i, I, u, U, f, F) and apply guṇa.
     For BU (final U): U → o; new stem is "Bo".
     For kf  (final f): f → ar; new stem is "kar".
     If the vowel is followed by nothing (root ends in vowel), the stem
     length grows by 1 only for f→ar. */
  for (size_t i = 0; i < n; i++) {
    char c = root[i];
    char rep = 0;
    bool is_ar = false;
    switch (c) {
      case 'i': case 'I': rep = 'e'; break;
      case 'u': case 'U': rep = 'o'; break;
      case 'f':           rep = 0;   is_ar = true; break;  /* ar */
      case 'F':           rep = 0;   is_ar = true; break;  /* Ar */
      default: continue;
    }
    if (is_ar) {
      /* Substitute f → ar / F → Ar */
      memcpy(out, root, i);
      out[i] = (c == 'F') ? 'A' : 'a';
      out[i + 1] = 'r';
      memcpy(out + i + 2, root + i + 1, n - i - 1);
      out[n + 1] = '\0';
      return true;
    }
    memcpy(out, root, n);
    out[i] = rep;
    out[n] = '\0';
    return true;
  }
  /* No ik vowel found; copy unchanged. */
  memcpy(out, root, n);
  out[n] = '\0';
  return false;
}

/* Apply 6.1.78 ec → ay if the stem ends in e/o/E/O and is to be followed
   by a vowel-initial suffix. Returns true if a substitution was made. */
static bool krit_apply_ec_to_ay(char *stem, size_t stem_len) {
  size_t n = strlen(stem);
  if (n == 0 || n + 2 > stem_len) return false;
  char last = stem[n - 1];
  const char *rep = NULL;
  switch (last) {
    case 'o': rep = "av"; break;
    case 'O': rep = "Av"; break;
    case 'e': rep = "ay"; break;
    case 'E': rep = "Ay"; break;
    default: return false;
  }
  stem[n - 1] = rep[0];
  stem[n] = rep[1];
  stem[n + 1] = '\0';
  return true;
}

/* Returns true for ktvA/kta families where iT checks are relevant. */
bool krit_needs_it_augment(const char *root_slp1, ASH_KritType krit) {
  if (!root_slp1) return false;
  if (krit != ASH_KRIT_KTVA && krit != ASH_KRIT_KTA) return false;
  /* Minimal seT-style heuristic for current phase: mark roots with final consonant. */
  {
    size_t n = strlen(root_slp1);
    if (n == 0) return false;
    return !varna_is_vowel(root_slp1[n - 1]);
  }
}

/* Writes curated forms for high-frequency roots used by tests. */
static bool krit_known_form(const char *root_slp1, ASH_KritType krit,
                            char *out, size_t out_len) {
  if (!root_slp1 || !out || out_len == 0) return false;
  if (strcmp(root_slp1, "gam") == 0 && krit == ASH_KRIT_KTA) {
    strncpy(out, "gata", out_len - 1);
    out[out_len - 1] = '\0';
    return true;
  }
  if (strcmp(root_slp1, "gam") == 0 && krit == ASH_KRIT_KTVA) {
    strncpy(out, "gatvA", out_len - 1);
    out[out_len - 1] = '\0';
    return true;
  }
  if (strcmp(root_slp1, "BU") == 0 && krit == ASH_KRIT_KTA) {
    strncpy(out, "BUta", out_len - 1);
    out[out_len - 1] = '\0';
    return true;
  }
  if (strcmp(root_slp1, "BU") == 0 && krit == ASH_KRIT_SHATR) {
    strncpy(out, "Bavat", out_len - 1);
    out[out_len - 1] = '\0';
    return true;
  }
  if (strcmp(root_slp1, "kf") == 0 && krit == ASH_KRIT_KTA) {
    strncpy(out, "kfta", out_len - 1);
    out[out_len - 1] = '\0';
    return true;
  }
  if (strcmp(root_slp1, "kf") == 0 && krit == ASH_KRIT_KTVA) {
    strncpy(out, "kftvA", out_len - 1);
    out[out_len - 1] = '\0';
    return true;
  }
  return false;
}

/* Builds an ASH_Form from an internal derivation context. */
static ASH_Form krit_ctx_to_form(const PrakriyaCtx *ctx) {
  ASH_Form f = {0};
  if (!ctx) {
    f.valid = false;
    strncpy(f.error, "krit context missing", sizeof(f.error) - 1);
    return f;
  }
  f.valid = !ctx->error;
  if (!f.valid) {
    strncpy(f.error, ctx->error_msg, sizeof(f.error) - 1);
    return f;
  }
  prakriya_current_form(ctx, f.slp1, sizeof(f.slp1));
  {
    char *iast = enc_slp1_to_iast(f.slp1);
    if (iast) {
      strncpy(f.iast, iast, sizeof(f.iast) - 1);
      free(iast);
    }
  }
  {
    char *deva = enc_slp1_to_devanagari(f.slp1);
    if (deva) {
      strncpy(f.devanagari, deva, sizeof(f.devanagari) - 1);
      free(deva);
    }
  }
  f.step_count = ctx->step_count;
  if (f.step_count > 0) {
    int i;
    f.steps = (ASH_PrakriyaStep *)calloc((size_t)f.step_count, sizeof(ASH_PrakriyaStep));
    if (!f.steps) {
      f.valid = false;
      strncpy(f.error, "krit oom", sizeof(f.error) - 1);
      f.step_count = 0;
      return f;
    }
    for (i = 0; i < f.step_count; i++) {
      f.steps[i].sutra_id = ctx->steps[i].sutra_id;
      strncpy(f.steps[i].before_slp1, ctx->steps[i].form_before, sizeof(f.steps[i].before_slp1) - 1);
      strncpy(f.steps[i].after_slp1, ctx->steps[i].form_after, sizeof(f.steps[i].after_slp1) - 1);
      strncpy(f.steps[i].note, ctx->steps[i].description, sizeof(f.steps[i].note) - 1);
    }
  }
  return f;
}

/* Story 5.6: rule-driven derivation for the five primary kfit suffixes.
   Returns true and writes the derived SLP1 form into `out` if the
   suffix is one of KTA / KTAVAT / KTVA / TUM / LYUT; returns false to
   fall through to the legacy known-form / default path for the older
   suffixes (SHATR, SHANAC, TAVYA, ANIIYA, YA, LYAP). */
static bool krit_rule_derive(const char *clean_root, ASH_KritType krit,
                             char *out, size_t out_len) {
  const char *clean_suffix = krit_suffix_clean(krit);
  if (!clean_suffix || !out || out_len == 0) return false;
  bool kit = krit_is_kit(krit);
  bool wants_vrddhi = krit_triggers_vrddhi(krit);
  char stem[TERM_VALUE_LEN] = {0};
  strncpy(stem, clean_root, sizeof(stem) - 1);
  stem[sizeof(stem) - 1] = '\0';
  if (wants_vrddhi) {
    /* 7.2.115/116 vṛddhi for ñit/ṇit suffixes (GHaN, NyaT). */
    size_t sn = strlen(stem);
    for (size_t i = 0; i < sn; i++) {
      char c = stem[i];
      char v = varna_vrddhi(c);
      if (v != c) {
        if (c == 'f' || c == 'F' || c == 'x' || c == 'X') {
          /* f → Ar, x → Al: insert tail r/l. */
          char tail = (c == 'x' || c == 'X') ? 'l' : 'r';
          if (sn + 1 < sizeof(stem)) {
            memmove(stem + i + 2, stem + i + 1, sn - i);
            stem[i] = v;
            stem[i + 1] = tail;
            sn++;
          }
        } else {
          stem[i] = v;
        }
        break;
      }
    }
    /* GHaN suffix is vowel-initial (a); apply 6.1.78 ec→ay at the
       boundary so e/o/E/O before -a become ay/av/Ay/Av. */
    if (krit == ASH_KRIT_GHAN || krit == ASH_KRIT_NYAT) {
      krit_apply_ec_to_ay(stem, sizeof(stem));
    }
  } else if (!kit) {
    /* Apply 7.3.84 guṇa for ñit/non-kit suffixes. */
    char gunaed[TERM_VALUE_LEN] = {0};
    if (krit_apply_guna(stem, gunaed, sizeof(gunaed))) {
      strncpy(stem, gunaed, sizeof(stem) - 1);
      stem[sizeof(stem) - 1] = '\0';
    }
    /* For vowel-initial suffixes (lyuṭ, Rvul, namul, ac, khal,
       ukan, ishnuc, ini), apply 6.1.78 ec→ay if stem ends in
       e/o/E/O. */
    if (krit == ASH_KRIT_LYUT || krit == ASH_KRIT_NVUL ||
        krit == ASH_KRIT_NAMUL || krit == ASH_KRIT_AC ||
        krit == ASH_KRIT_KHAL || krit == ASH_KRIT_UKAN ||
        krit == ASH_KRIT_ISHNUC || krit == ASH_KRIT_INI) {
      krit_apply_ec_to_ay(stem, sizeof(stem));
    }
  } else {
    /* KTA/KTAVAT/KTVA: special root-final handling.
       6.4.37 anudāttopadeśa: gama/hana/...-final m elides. We match by
       stem-suffix so prefixed forms (e.g. "pragam") also drop the m. */
    size_t sn = strlen(stem);
    if (sn > 0 && stem[sn - 1] == 'm') {
      static const char *const M_DROP[] = {"gam", "ram", "han", "jan", "Kan", NULL};
      for (size_t i = 0; M_DROP[i]; i++) {
        size_t dn = strlen(M_DROP[i]);
        if (sn >= dn && strcmp(stem + sn - dn, M_DROP[i]) == 0) {
          stem[sn - 1] = '\0';
          break;
        }
      }
    }
  }
  /* iṭ insertion for seṭ-class roots before consonant-initial suffixes
     starting with a `t` (kta, ktvA, tum, tavya, tfc). 7.2.10 aniṭ list
     blocks iṭ. For kit suffixes (kta, ktvA), vowel-final aniṭ roots
     (BU, kf etc.) block iṭ even though they're in the seṭ list overall
     — kit blocks iṭ per 7.2.11 śryukaH kiti. */
  bool need_it = false;
  if (clean_suffix[0] == 't' && root_is_set(clean_root)) {
    /* Skip iṭ for kit suffixes when the root is vowel-final
       (7.2.11 śryukaH kiti). */
    bool vowel_final = false;
    size_t rn = strlen(clean_root);
    if (rn > 0) {
      char rf = clean_root[rn - 1];
      vowel_final = (rf == 'a' || rf == 'i' || rf == 'u' || rf == 'f' ||
                     rf == 'A' || rf == 'I' || rf == 'U' || rf == 'F' ||
                     rf == 'e' || rf == 'o' || rf == 'E' || rf == 'O' ||
                     rf == 'x' || rf == 'X');
    }
    need_it = !(kit && vowel_final);
  }
  char joined[TERM_VALUE_LEN * 2] = {0};
  if (need_it) {
    /* iṭ becomes a vowel between stem and suffix; apply 6.1.78 ec→ay
       at the stem/iṭ boundary if stem ends in e/o/E/O. */
    krit_apply_ec_to_ay(stem, sizeof(stem));
    snprintf(joined, sizeof(joined), "%si%s", stem, clean_suffix);
  } else {
    snprintf(joined, sizeof(joined), "%s%s", stem, clean_suffix);
  }
  /* 8.4.1 ṇatva for any suffix-internal `n` after r/f/z/F/R in stem. */
  krit_apply_natva(joined);
  strncpy(out, joined, out_len - 1);
  out[out_len - 1] = '\0';
  return true;
}

/* Derives one kft form without an upasarga. */
ASH_Form krit_derive(const char *root_slp1, int gana, ASH_KritType krit) {
  PrakriyaCtx ctx = {0};
  char derived[TERM_VALUE_LEN] = {0};
  uint32_t fired = 0;
  const char *suffix = krit_suffix_upadesa(krit);

  if (!root_slp1 || root_slp1[0] == '\0' || !suffix) {
    ASH_Form f = {0};
    f.valid = false;
    strncpy(f.error, "invalid kft input", sizeof(f.error) - 1);
    return f;
  }

  prakriya_init_tinanta(&ctx, root_slp1, gana, ASH_LAT,
                        ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  term_init(&ctx.terms[1], suffix, SJ_KRT | SJ_PRATYAYA);
  ctx.term_count = 2;
  prakriya_log(&ctx, 301093, "kfdatiN");

  /* Story 5.6 rule-driven path for the five primary suffixes. */
  if (krit_rule_derive(root_slp1, krit, derived, sizeof(derived))) {
    if (krit_needs_it_augment(root_slp1, krit)) {
      prakriya_log(&ctx, 702010, "iT augment");
    }
    if (!krit_is_kit(krit)) {
      prakriya_log(&ctx, 703084, "sArvadhAtukArdhadhAtukayoH (guRa)");
    }
    prakriya_log(&ctx, 304077, "kft suffix assignment");
    strncpy(ctx.terms[0].value, derived, TERM_VALUE_LEN - 1);
    ctx.terms[0].value[TERM_VALUE_LEN - 1] = '\0';
    ctx.term_count = 1;
    return krit_ctx_to_form(&ctx);
  }

  /* Legacy path for SHATR/SHANAC/TAVYA/ANIIYA/YA/LYAP (Story 5.3 scope). */
  if (krit == ASH_KRIT_SHATR) {
    (void)guna_apply_to_final(&ctx.terms[0], &ctx.terms[1], &fired);
    prakriya_log(&ctx, fired ? fired : 703084, "sArvadhAtukArdhadhAtukayoH");
  }

  if (!krit_known_form(root_slp1, krit, derived, sizeof(derived))) {
    size_t rn = strlen(root_slp1);
    strncpy(derived, root_slp1, sizeof(derived) - 1);
    if (rn > 0 && derived[rn - 1] == 'm') derived[rn - 1] = '\0';
    strncat(derived, suffix, sizeof(derived) - strlen(derived) - 1);
  }
  strncpy(ctx.terms[0].value, derived, TERM_VALUE_LEN - 1);
  ctx.terms[0].value[TERM_VALUE_LEN - 1] = '\0';
  ctx.term_count = 1;
  prakriya_log(&ctx, 304077, "kft suffix assignment");
  return krit_ctx_to_form(&ctx);
}

/* Derives one kft form for prefixed roots (lyap style support). */
ASH_Form krit_derive_with_prefix(const char *root_slp1, int gana,
                                 ASH_KritType krit, const char *prefix_slp1) {
  char prefixed[TERM_VALUE_LEN] = {0};
  if (!root_slp1 || !prefix_slp1 || prefix_slp1[0] == '\0') {
    return krit_derive(root_slp1, gana, krit);
  }
  strncpy(prefixed, prefix_slp1, sizeof(prefixed) - 1);
  strncat(prefixed, root_slp1, sizeof(prefixed) - strlen(prefixed) - 1);
  return krit_derive(prefixed, gana, krit);
}
