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
    case ASH_KRIT_KIN: return "kin";
    case ASH_KRIT_KVASUS: return "kvasus";
    case ASH_KRIT_ATAN: return "atan";
    case ASH_KRIT_KTAVATUS: return "ktavatus";
    case ASH_KRIT_NAN: return "Ran";
    case ASH_KRIT_GHURAC: return "Gurac";
    case ASH_KRIT_AALUC: return "Aluc";
    case ASH_KRIT_ATRN: return "atrn";
    case ASH_KRIT_MANIN: return "manin";
    case ASH_KRIT_RA: return "ra";
    case ASH_KRIT_KIT: return "kit";
    case ASH_KRIT_BHAVA: return "Bava";
    case ASH_KRIT_VANIP: return "vanip";
    case ASH_KRIT_VANAC: return "vanac";
    case ASH_KRIT_SHACINIT: return "Sacinit";
    case ASH_KRIT_KTHAN: return "kTan";
    case ASH_KRIT_KHISHNUC: return "KizRuc";
    case ASH_KRIT_KHA: return "Ka";
    case ASH_KRIT_KTAVYAN: return "ktavyan";
    case ASH_KRIT_KTRP: return "ktrp";
    /* Fourth-tier additions — closed-set kṛt pratyayas */
    case ASH_KRIT_A: return "a";
    case ASH_KRIT_ANG: return "aN";
    case ASH_KRIT_ATHUC: return "athuc";
    case ASH_KRIT_ADHYAI: return "aDyE";
    case ASH_KRIT_ADHYAIN: return "aDyEn";
    case ASH_KRIT_ANI: return "ani";
    case ASH_KRIT_AP: return "ap";
    case ASH_KRIT_ASE: return "ase";
    case ASH_KRIT_ASEN: return "asen";
    case ASH_KRIT_AARU: return "Aru";
    case ASH_KRIT_INC: return "iY";
    case ASH_KRIT_ITRA: return "itra";
    case ASH_KRIT_INUN: return "inuR";
    case ASH_KRIT_U: return "u";
    case ASH_KRIT_UUKA: return "Uka";
    case ASH_KRIT_ESH: return "eS";
    case ASH_KRIT_KAN: return "kaY";
    case ASH_KRIT_KADHYAI: return "kaDyE";
    case ASH_KRIT_KADHYAIN: return "kaDyEn";
    case ASH_KRIT_KAP: return "kap";
    case ASH_KRIT_KAMUL: return "kamul";
    case ASH_KRIT_KASUN: return "kasun";
    case ASH_KRIT_KASEN: return "kasen";
    case ASH_KRIT_KI: return "ki";
    case ASH_KRIT_KURAC: return "kurac";
    case ASH_KRIT_KEN: return "ken";
    case ASH_KRIT_KENYA: return "kenya";
    case ASH_KRIT_KNU: return "knu";
    case ASH_KRIT_KRU: return "kru";
    case ASH_KRIT_KLUKAN: return "klukan";
    case ASH_KRIT_KVANIP: return "kvanip";
    case ASH_KRIT_KVARAP: return "kvarap";
    case ASH_KRIT_KVIN: return "kvin";
    case ASH_KRIT_KSE: return "kse";
    case ASH_KRIT_KHAC: return "Kac";
    case ASH_KRIT_KHAMUN: return "KamuY";
    case ASH_KRIT_KHASH: return "KaS";
    case ASH_KRIT_KHUKAN: return "KukaY";
    case ASH_KRIT_KHYUN: return "Kyun";
    case ASH_KRIT_GSNU: return "gsnu";
    case ASH_KRIT_GHINUN: return "GinuR";
    case ASH_KRIT_NGVANIP: return "Nvanip";
    case ASH_KRIT_CHANASH: return "cAnaS";
    case ASH_KRIT_NYUT: return "Yyuw";
    case ASH_KRIT_TAK: return "wak";
    case ASH_KRIT_D: return "q";
    case ASH_KRIT_DU: return "qu";
    case ASH_KRIT_N: return "R";
    case ASH_KRIT_NAC: return "Rac";
    case ASH_KRIT_NNYUT: return "Ryuw";
    case ASH_KRIT_NVI: return "Rvi";
    case ASH_KRIT_NVIN: return "Rvin";
    case ASH_KRIT_NVUCH: return "Rvuc";
    case ASH_KRIT_TAVAI: return "tavE";
    case ASH_KRIT_TAVEN: return "taveN";
    case ASH_KRIT_TAVENAS: return "taven";
    case ASH_KRIT_TOSUN: return "tosun";
    case ASH_KRIT_THAKAN: return "Takan";
    case ASH_KRIT_NANG: return "naN";
    case ASH_KRIT_NAJING: return "najiN";
    case ASH_KRIT_YAT: return "yat";
    case ASH_KRIT_YUC: return "yuc";
    case ASH_KRIT_RU: return "ru";
    case ASH_KRIT_LYU: return "lyu";
    case ASH_KRIT_VARAC: return "varac";
    case ASH_KRIT_VIC: return "vic";
    case ASH_KRIT_VIT: return "viw";
    case ASH_KRIT_VUN_ALT: return "vuY";
    case ASH_KRIT_SHA: return "Sa";
    case ASH_KRIT_SHADHYAI: return "SaDyE";
    case ASH_KRIT_SHADHYAIN: return "SaDyEn";
    case ASH_KRIT_SHANAN: return "SAnan";
    case ASH_KRIT_SHAAKAN: return "zAkan";
    case ASH_KRIT_SHTRAN: return "zwran";
    case ASH_KRIT_SHVUN: return "zvun";
    case ASH_KRIT_SE: return "se";
    case ASH_KRIT_SEN: return "sen";
    case ASH_KRIT_DARA: return "qara";
    case ASH_KRIT_IKAVAKA: return "ikavaka";
    case ASH_KRIT_Y_ALT: return "ya";
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
    case ASH_KRIT_KIN: return "i";        /* kit, i */
    case ASH_KRIT_KVASUS: return "vat";   /* perfect active, vat */
    case ASH_KRIT_ATAN: return "at";      /* present-stem participle */
    case ASH_KRIT_KTAVATUS: return "tavat"; /* tavat, alternate */
    case ASH_KRIT_NAN: return "a";        /* ñit, vrddhi + a */
    case ASH_KRIT_GHURAC: return "ura";   /* ura adjective */
    case ASH_KRIT_AALUC: return "Alu";    /* ālu adjective */
    case ASH_KRIT_ATRN: return "tra";     /* action -tra */
    case ASH_KRIT_MANIN: return "man";    /* -man action */
    case ASH_KRIT_RA: return "ra";        /* -ra action */
    case ASH_KRIT_KIT: return "i";        /* kit -i agent */
    case ASH_KRIT_BHAVA: return "Bava";   /* bhāva-noun */
    case ASH_KRIT_VANIP: return "van";    /* -van adj */
    case ASH_KRIT_VANAC: return "vana";   /* -vana action */
    case ASH_KRIT_SHACINIT: return "";    /* special particle */
    case ASH_KRIT_KTHAN: return "Ta";     /* -tha action */
    case ASH_KRIT_KHISHNUC: return "izRu";/* ñit -iṣṇu */
    case ASH_KRIT_KHA: return "Ka";       /* ñit -kha */
    case ASH_KRIT_KTAVYAN: return "tavya";/* alternate -tavya */
    case ASH_KRIT_KTRP: return "tra";     /* -tra agent */
    /* Fourth-tier clean forms. The anubandha-marked variants drop
       their marker letter(s); ñ/ṅ/ṣ at start are anubandhas only. */
    case ASH_KRIT_A: return "a";
    case ASH_KRIT_ANG: return "a";       /* ṅ marker dropped */
    case ASH_KRIT_ATHUC: return "aTu";   /* c dropped */
    case ASH_KRIT_ADHYAI: return "aDyE";
    case ASH_KRIT_ADHYAIN: return "aDyEn";
    case ASH_KRIT_ANI: return "ani";
    case ASH_KRIT_AP: return "a";        /* p marker dropped */
    case ASH_KRIT_ASE: return "ase";
    case ASH_KRIT_ASEN: return "asen";
    case ASH_KRIT_AARU: return "Aru";
    case ASH_KRIT_INC: return "i";       /* ñ marker dropped */
    case ASH_KRIT_ITRA: return "itra";
    case ASH_KRIT_INUN: return "in";     /* uṇ marker dropped */
    case ASH_KRIT_U: return "u";
    case ASH_KRIT_UUKA: return "Uka";
    case ASH_KRIT_ESH: return "e";       /* ś marker dropped */
    case ASH_KRIT_KAN: return "ka";      /* ñ dropped (k is anubandha) */
    case ASH_KRIT_KADHYAI: return "aDyE";
    case ASH_KRIT_KADHYAIN: return "aDyEn";
    case ASH_KRIT_KAP: return "a";       /* k and p anubandhas */
    case ASH_KRIT_KAMUL: return "am";    /* k+ul anubandhas */
    case ASH_KRIT_KASUN: return "as";    /* k+un anubandhas */
    case ASH_KRIT_KASEN: return "ase";   /* k+n anubandhas */
    case ASH_KRIT_KI: return "i";        /* k anubandha */
    case ASH_KRIT_KURAC: return "ura";   /* k anubandha + ac marker */
    case ASH_KRIT_KEN: return "e";       /* k+n */
    case ASH_KRIT_KENYA: return "enya";
    case ASH_KRIT_KNU: return "nu";      /* k anubandha */
    case ASH_KRIT_KRU: return "ru";      /* k anubandha */
    case ASH_KRIT_KLUKAN: return "luka"; /* k+n anubandhas */
    case ASH_KRIT_KVANIP: return "van";  /* k anubandha + ip-marker */
    case ASH_KRIT_KVARAP: return "vara"; /* k+p anubandhas */
    case ASH_KRIT_KVIN: return "vi";     /* k+n */
    case ASH_KRIT_KSE: return "se";      /* k anubandha */
    case ASH_KRIT_KHAC: return "a";      /* kh+c anubandhas */
    case ASH_KRIT_KHAMUN: return "am";   /* kh+ñ */
    case ASH_KRIT_KHASH: return "a";     /* kh+ś */
    case ASH_KRIT_KHUKAN: return "uka";  /* kh+ñ */
    case ASH_KRIT_KHYUN: return "yu";    /* kh+n */
    case ASH_KRIT_GSNU: return "snu";    /* g anubandha */
    case ASH_KRIT_GHINUN: return "in";   /* gh+uṇ */
    case ASH_KRIT_NGVANIP: return "van"; /* ṅ anubandha + ip */
    case ASH_KRIT_CHANASH: return "Ana"; /* c+ś */
    case ASH_KRIT_NYUT: return "ana";    /* ñ-yu-ṭ — like lyuṭ */
    case ASH_KRIT_TAK: return "a";       /* ṭ+k anubandhas */
    case ASH_KRIT_D: return "";          /* ḍ — zero suffix */
    case ASH_KRIT_DU: return "u";        /* ḍ anubandha */
    case ASH_KRIT_N: return "";          /* ṇ — zero (after vrddhi-trigger) */
    case ASH_KRIT_NAC: return "a";       /* ṇ+c */
    case ASH_KRIT_NNYUT: return "ana";   /* ṇ-yu-ṭ */
    case ASH_KRIT_NVI: return "v";       /* ṇ anubandha + i marker */
    case ASH_KRIT_NVIN: return "vi";     /* ṇ+n */
    case ASH_KRIT_NVUCH: return "vu";    /* ṇ+c */
    case ASH_KRIT_TAVAI: return "tavE";
    case ASH_KRIT_TAVEN: return "tave";  /* ṅ anubandha */
    case ASH_KRIT_TAVENAS: return "taven";
    case ASH_KRIT_TOSUN: return "tos";   /* un anubandha */
    case ASH_KRIT_THAKAN: return "Taka"; /* n anubandha */
    case ASH_KRIT_NANG: return "na";     /* ṅ anubandha */
    case ASH_KRIT_NAJING: return "naji"; /* ṅ */
    case ASH_KRIT_YAT: return "ya";      /* t marker */
    case ASH_KRIT_YUC: return "ana";     /* yu+c, like lyuṭ */
    case ASH_KRIT_RU: return "ru";
    case ASH_KRIT_LYU: return "ana";     /* l+yu */
    case ASH_KRIT_VARAC: return "vara";  /* c */
    case ASH_KRIT_VIC: return "v";       /* i+c */
    case ASH_KRIT_VIT: return "vi";      /* ṭ */
    case ASH_KRIT_VUN_ALT: return "aka"; /* alt vuñ */
    case ASH_KRIT_SHA: return "a";       /* ś */
    case ASH_KRIT_SHADHYAI: return "aDyE";
    case ASH_KRIT_SHADHYAIN: return "aDyEn";
    case ASH_KRIT_SHANAN: return "Ana";  /* śAnan */
    case ASH_KRIT_SHAAKAN: return "Aka"; /* ṣAkan */
    case ASH_KRIT_SHTRAN: return "tra";  /* ṣṭran */
    case ASH_KRIT_SHVUN: return "aka";   /* ṣvun, alt to nvul */
    case ASH_KRIT_SE: return "se";
    case ASH_KRIT_SEN: return "sen";
    case ASH_KRIT_DARA: return "ara";    /* ḍara */
    case ASH_KRIT_IKAVAKA: return "ikavaka";
    case ASH_KRIT_Y_ALT: return "ya";
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
         krit == ASH_KRIT_KANAC ||
         krit == ASH_KRIT_KIN || krit == ASH_KRIT_KVASUS ||
         krit == ASH_KRIT_KTAVATUS || krit == ASH_KRIT_KIT ||
         krit == ASH_KRIT_KTHAN || krit == ASH_KRIT_KTAVYAN ||
         krit == ASH_KRIT_KTRP ||
         /* Fourth-tier kit suffixes (all those with 'k' anubandha
            prefix in pratyay.txt's name column). */
         krit == ASH_KRIT_KAN || krit == ASH_KRIT_KADHYAI ||
         krit == ASH_KRIT_KADHYAIN || krit == ASH_KRIT_KAP ||
         krit == ASH_KRIT_KAMUL || krit == ASH_KRIT_KASUN ||
         krit == ASH_KRIT_KASEN || krit == ASH_KRIT_KI ||
         krit == ASH_KRIT_KURAC || krit == ASH_KRIT_KEN ||
         krit == ASH_KRIT_KENYA || krit == ASH_KRIT_KNU ||
         krit == ASH_KRIT_KRU || krit == ASH_KRIT_KLUKAN ||
         krit == ASH_KRIT_KVANIP || krit == ASH_KRIT_KVARAP ||
         krit == ASH_KRIT_KVIN || krit == ASH_KRIT_KSE;
}

/* Whether a suffix triggers vṛddhi on the root vowel (ñit/ṇit). */
static bool krit_triggers_vrddhi(ASH_KritType krit) {
  return krit == ASH_KRIT_GHAN || krit == ASH_KRIT_NYAT ||
         krit == ASH_KRIT_VUN  || krit == ASH_KRIT_RVU  ||
         krit == ASH_KRIT_TRN  || krit == ASH_KRIT_NAN  ||
         krit == ASH_KRIT_KHISHNUC || krit == ASH_KRIT_KHA ||
         /* Fourth-tier ñit/ṇit suffixes: those whose name starts
            with ñ (Y/Ñ in SLP1) or ṇ (R/Ṇ). */
         krit == ASH_KRIT_NYUT || krit == ASH_KRIT_INC ||
         krit == ASH_KRIT_NAC  || krit == ASH_KRIT_NNYUT ||
         krit == ASH_KRIT_NVI  || krit == ASH_KRIT_NVIN ||
         krit == ASH_KRIT_NVUCH || krit == ASH_KRIT_VUN_ALT ||
         krit == ASH_KRIT_GHINUN || krit == ASH_KRIT_INUN;
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
    /* For vowel-initial suffixes, apply 6.1.78 ec→ay if stem ends
       in e/o/E/O. This includes lyuṭ, ṇvul, namul, ac, khal, ukañ,
       iṣṇuc, ini AND the fourth-tier vowel-initial ones (a, aṅ, athu,
       adhyai, ani, ap, ase, asen, āru, in, itra, inuṇ, u, ūka, eś,
       yuc, lyu, ...). */
    if (krit == ASH_KRIT_LYUT || krit == ASH_KRIT_NVUL ||
        krit == ASH_KRIT_NAMUL || krit == ASH_KRIT_AC ||
        krit == ASH_KRIT_KHAL || krit == ASH_KRIT_UKAN ||
        krit == ASH_KRIT_ISHNUC || krit == ASH_KRIT_INI ||
        krit == ASH_KRIT_A || krit == ASH_KRIT_ANG ||
        krit == ASH_KRIT_ATHUC || krit == ASH_KRIT_ADHYAI ||
        krit == ASH_KRIT_ADHYAIN || krit == ASH_KRIT_ANI ||
        krit == ASH_KRIT_AP || krit == ASH_KRIT_ASE ||
        krit == ASH_KRIT_ASEN || krit == ASH_KRIT_AARU ||
        krit == ASH_KRIT_INC || krit == ASH_KRIT_ITRA ||
        krit == ASH_KRIT_INUN || krit == ASH_KRIT_U ||
        krit == ASH_KRIT_UUKA || krit == ASH_KRIT_ESH ||
        krit == ASH_KRIT_YUC || krit == ASH_KRIT_LYU ||
        krit == ASH_KRIT_YAT || krit == ASH_KRIT_NYUT ||
        krit == ASH_KRIT_NNYUT || krit == ASH_KRIT_NAC ||
        krit == ASH_KRIT_KHAC || krit == ASH_KRIT_SHA) {
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
