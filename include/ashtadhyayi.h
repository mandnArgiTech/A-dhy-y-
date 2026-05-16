/**
 * @file ashtadhyayi.h
 * @brief libAshtadhyayi — Complete C implementation of Pāṇini's Aṣṭādhyāyī
 *
 * All symbols are prefixed ASH_ (types/macros) or ash_ (functions).
 * All internal processing uses SLP1 encoding. Conversions happen at
 * output boundaries only.
 *
 * @version 0.1.0
 */

#ifndef ASHTADHYAYI_H
#define ASHTADHYAYI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/* ═══════════════════════════════════════════════════════════════════════════
 * Encoding
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Supported string encodings */
typedef enum {
  ASH_ENC_SLP1 = 0,  /**< ASCII transliteration (internal) */
  ASH_ENC_IAST,      /**< Unicode IAST (display) */
  ASH_ENC_DEVANAGARI, /**< Unicode Devanāgarī (display) */
  ASH_ENC_HK,        /**< Harvard-Kyoto (legacy) */
} ASH_Encoding;

/** Convert between encodings. Caller must free() result. */
char *ash_encode(const char *input, ASH_Encoding from, ASH_Encoding to);

/* ═══════════════════════════════════════════════════════════════════════════
 * Grammar Constants
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Lakaras (verbal tense/mood markers) */
typedef enum {
  ASH_LAT = 0,   /**< Present (laṭ) */
  ASH_LIT,       /**< Perfect (liṭ) */
  ASH_LUT,       /**< Periphrastic future (luṭ) */
  ASH_LRT,       /**< Simple future (lṛṭ) */
  ASH_LOT,       /**< Imperative (loṭ) */
  ASH_LAN,       /**< Imperfect (laṅ) */
  ASH_VIDHILIM,  /**< Optative (vidhiliṅ) */
  ASH_ASHIRLIM,  /**< Benedictive (āśīrliṅ) */
  ASH_LUN,       /**< Aorist (luṅ) */
  ASH_LRN,       /**< Conditional (lṛṅ) */
  ASH_LAKARA_COUNT
} ASH_Lakara;

/** Puruṣa (grammatical person) */
typedef enum {
  ASH_PRATHAMA = 0,  /**< 3rd person */
  ASH_MADHYAMA,      /**< 2nd person */
  ASH_UTTAMA,        /**< 1st person */
} ASH_Purusha;

/** Vacana (number) */
typedef enum {
  ASH_EKAVACANA = 0, /**< Singular */
  ASH_DVIVACANA,     /**< Dual */
  ASH_BAHUVACANA,    /**< Plural */
} ASH_Vacana;

/** Pada (voice category) */
typedef enum {
  ASH_PARASMAI = 0,  /**< Parasmaipada (active-like) */
  ASH_ATMANE,        /**< Ātmanepada (middle-like) */
  ASH_UBHAYA,        /**< Both padas permitted */
} ASH_Pada;

/** Liṅga (grammatical gender) */
typedef enum {
  ASH_PUMS = 0,    /**< Masculine (puṃliṅga) */
  ASH_STRI,        /**< Feminine (strīliṅga) */
  ASH_NAPUMSAKA,   /**< Neuter (napuṃsakaliṅga) */
} ASH_Linga;

/** Vibhakti (grammatical case) */
typedef enum {
  ASH_PRATHAMA_VIB = 0,  /**< Nominative */
  ASH_DVITIYA_VIB,       /**< Accusative */
  ASH_TRITIYA_VIB,       /**< Instrumental */
  ASH_CATURTHI_VIB,      /**< Dative */
  ASH_PANCAMI_VIB,       /**< Ablative */
  ASH_SHASTHI_VIB,       /**< Genitive */
  ASH_SAPTAMI_VIB,       /**< Locative */
  ASH_SAMBODHANA_VIB,    /**< Vocative */
} ASH_Vibhakti;

/** Kāraka (semantic role) */
typedef enum {
  ASH_KARTA = 0,    /**< Agent (1.4.54) */
  ASH_KARMAN,       /**< Object (1.4.49) */
  ASH_KARANA,       /**< Instrument (1.4.42) */
  ASH_SAMPRADANA,   /**< Recipient (1.4.32) */
  ASH_APADANA,      /**< Source (1.4.24) */
  ASH_ADHIKARANA,   /**< Locus (1.4.45) */
} ASH_Karaka;

/** Sūtra type classification */
typedef enum {
  ASH_SUTRA_SAMJNA = 0,  /**< Technical term definition */
  ASH_SUTRA_PARIBHASHA,  /**< Metarule */
  ASH_SUTRA_ADHIKARA,    /**< Domain marker */
  ASH_SUTRA_VIDHI,       /**< Operational rule */
  ASH_SUTRA_NIYAMA,      /**< Restrictive rule */
  ASH_SUTRA_ATIDESHA,    /**< Extended application rule */
  ASH_SUTRA_NISHEDHA,    /**< Prohibition rule */
} ASH_SutraType;

/* ═══════════════════════════════════════════════════════════════════════════
 * Sūtra
 * ═══════════════════════════════════════════════════════════════════════════ */

/** A single Pāṇinian sūtra */
typedef struct {
  uint16_t      adhyaya;         /**< Chapter (1–8) */
  uint8_t       pada;            /**< Quarter (1–4) */
  uint16_t      num;             /**< Number within pada */
  uint32_t      global_id;       /**< 1-based canonical ordering */
  char          text_slp1[256];  /**< Sūtra text in SLP1 */
  ASH_SutraType type;
  uint32_t      adhikara_parent; /**< global_id of governing adhikāra */
  uint32_t      anuvrtti_from;   /**< global_id whose terms carry over */
} ASH_Sutra;

/* ═══════════════════════════════════════════════════════════════════════════
 * Database Handle
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Opaque database handle — owns all loaded data */
typedef struct ASH_DB ASH_DB;

/**
 * @brief Load all data from TSV files in data_dir.
 * @param data_dir Path to directory containing *.tsv files.
 * @return Heap-allocated DB handle, or NULL on failure.
 */
ASH_DB *ash_db_load(const char *data_dir);

/** Free all resources owned by db. */
void ash_db_free(ASH_DB *db);

/* ═══════════════════════════════════════════════════════════════════════════
 * Sūtra Lookup
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Look up sūtra by traditional address (adhyāya.pāda.sūtra). */
const ASH_Sutra *ash_sutra_by_addr(const ASH_DB *db, int a, int p, int n);

/** Look up sūtra by canonical global_id (1-based). */
const ASH_Sutra *ash_sutra_by_id(const ASH_DB *db, uint32_t global_id);

/** Print sūtra with anuvṛtti terms to stream. */
void ash_sutra_print(const ASH_Sutra *s, const ASH_DB *db, FILE *stream);

/* ═══════════════════════════════════════════════════════════════════════════
 * Sandhi
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Apply sandhi between two SLP1 strings.
 * @param a   First element (SLP1).
 * @param b   Second element (SLP1).
 * @param out Output buffer (caller provides).
 * @param out_len Size of output buffer.
 * @param enc Output encoding for result.
 * @return true on success.
 */
bool ash_sandhi_apply(const char *a, const char *b,
                      char *out, size_t out_len, ASH_Encoding enc);

/**
 * @brief Attempt to split a sandhied string into component forms.
 * @param joined  Input (SLP1).
 * @param splits  Caller-allocated array of char* to receive results.
 * @param max_splits Maximum number of splits to return.
 * @return Number of splits found.
 */
int ash_sandhi_split(const char *joined, char **splits, int max_splits);

/* ═══════════════════════════════════════════════════════════════════════════
 * Derivation Result
 * ═══════════════════════════════════════════════════════════════════════════ */

/** A single step in a prakriyā (derivation trace) */
typedef struct {
  uint32_t sutra_id;          /**< global_id of rule applied */
  char     before_slp1[128];  /**< Form before rule */
  char     after_slp1[128];   /**< Form after rule */
  char     note[128];         /**< Human-readable note */
} ASH_PrakriyaStep;

/** A derived word form with full provenance */
typedef struct {
  char              slp1[128];         /**< Final form in SLP1 */
  char              iast[256];         /**< Final form in IAST */
  char              devanagari[512];   /**< Final form in Devanāgarī */
  ASH_PrakriyaStep *steps;            /**< Heap-allocated derivation trace */
  int               step_count;
  bool              valid;             /**< false if derivation failed */
  char              error[256];        /**< Error message if !valid */
} ASH_Form;

/** Print the full step-by-step derivation trace to stream. */
void ash_form_print_prakriya(const ASH_Form *f, FILE *stream);

/** Free heap resources inside form (not the form struct itself). */
void ash_form_free(ASH_Form *f);

/* ═══════════════════════════════════════════════════════════════════════════
 * Verbal Derivation (Tiṅanta)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Derive a finite verb form via Pāṇinian prakriyā.
 *
 * @param db        Loaded database handle.
 * @param root_slp1 Dhātu in SLP1 (e.g. "BU" for bhū, "gam" for gam).
 * @param gana      Gaṇa number 1–10.
 * @param lakara    Tense/mood (e.g. ASH_LAT).
 * @param purusha   Person (ASH_PRATHAMA / MADHYAMA / UTTAMA).
 * @param vacana    Number (ASH_EKAVACANA / DVIVACANA / BAHUVACANA).
 * @param pada      Voice (ASH_PARASMAI / ASH_ATMANE).
 * @return ASH_Form — caller must call ash_form_free() when done.
 */
ASH_Form ash_tinanta(const ASH_DB *db, const char *root_slp1, int gana,
                     ASH_Lakara lakara, ASH_Purusha purusha,
                     ASH_Vacana vacana, ASH_Pada pada);

/**
 * @brief Generate full tiṅanta paradigm (18 forms).
 * @param forms  Caller-allocated array of 18 ASH_Form structs.
 * Ordered: prathama eka/dvi/bahu, madhyama eka/dvi/bahu, uttama eka/dvi/bahu
 *          — parasmaipada first (9), then ātmanepada (9).
 */
void ash_tinanta_paradigm(const ASH_DB *db, const char *root_slp1, int gana,
                          ASH_Lakara lakara, ASH_Form forms[18]);

/* ═══════════════════════════════════════════════════════════════════════════
 * Nominal Derivation (Subanta)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Derive a nominal form (subanta).
 *
 * @param db        Loaded database handle.
 * @param stem_slp1 Prātipadika in SLP1 (e.g. "rAma", "nAman", "manas").
 * @param linga     Gender.
 * @param vibhakti  Case.
 * @param vacana    Number.
 * @return ASH_Form — caller must call ash_form_free() when done.
 */
ASH_Form ash_subanta(const ASH_DB *db, const char *stem_slp1,
                     ASH_Linga linga, ASH_Vibhakti vibhakti,
                     ASH_Vacana vacana);

/**
 * @brief Generate full subanta paradigm (24 forms: 8 cases × 3 numbers).
 * @param forms  Caller-allocated array of 24 ASH_Form structs.
 * Ordered: prathama eka/dvi/bahu, dvitīya eka/dvi/bahu, … sambodhana eka/dvi/bahu.
 */
void ash_subanta_paradigm(const ASH_DB *db, const char *stem_slp1,
                          ASH_Linga linga, ASH_Form forms[24]);

/* ═══════════════════════════════════════════════════════════════════════════
 * Kṛt Derivatives
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Kṛt suffix types */
typedef enum {
  ASH_KRIT_KTA = 0,    /**< Past passive participle (-ta/-na) */
  ASH_KRIT_KTAVAT,     /**< Past active participle (-tavat) */
  ASH_KRIT_SHATR,      /**< Present active participle (-at) */
  ASH_KRIT_SHANAC,     /**< Present middle participle (-āna) */
  ASH_KRIT_TAVYA,      /**< Gerundive (-tavya) */
  ASH_KRIT_ANIIYA,     /**< Gerundive (-anīya) */
  ASH_KRIT_YA,         /**< Gerundive (-ya) */
  ASH_KRIT_LYAP,       /**< Absolutive with prefix (-ya) */
  ASH_KRIT_KTVA,       /**< Absolutive without prefix (-tvā) */
  ASH_KRIT_TUM,        /**< Infinitive (-tum) */
  ASH_KRIT_LYUT,       /**< Action noun (-ana, also called lyuṭ) */
  /* Phase δ kṛt expansion */
  ASH_KRIT_GHAN,       /**< Action noun (-a / vrddhi-root) per 3.3.18 */
  ASH_KRIT_NVUL,       /**< Agent noun (-aka) per 3.1.133 */
  ASH_KRIT_TRC,        /**< Agent noun (-tṛ) per 3.1.133 */
  ASH_KRIT_KTIN,       /**< Action noun (-ti) per 3.3.94 */
  ASH_KRIT_KYAP,       /**< Gerundive (-ya, kit) per 3.1.106 */
  ASH_KRIT_NYAT,       /**< Gerundive (-ya, vrddhi) per 3.1.124 */
  ASH_KRIT_KVIP,       /**< Agent noun (-φ, zero) per 3.2.61 */
  ASH_KRIT_NAMUL,      /**< Adverbial absolutive (-am) per 3.4.22 */
  /* Phase δ kṛt extended */
  ASH_KRIT_KTRI,       /**< Action noun (-tri) per 3.3.88 */
  ASH_KRIT_KTUM,       /**< Action noun (-tu) per 3.3.10 */
  ASH_KRIT_KMARAC,     /**< Action noun (-mara) per 3.3.74 */
  ASH_KRIT_GHA,        /**< Action noun (-ya, guṇa) per 3.3.118 */
  ASH_KRIT_KA,         /**< Agent noun (-a, kit) per 3.2.3 */
  ASH_KRIT_AC,         /**< Agent noun (-a, ñit) per 3.1.134 */
  ASH_KRIT_KHAL,       /**< Adverbial noun (-a) per 3.3.126 */
  ASH_KRIT_VUN,        /**< Agent noun (-aka, ñit) per 3.1.149 */
  ASH_KRIT_ISHNUC,     /**< Adjective (-iṣṇu) per 3.2.136 */
  ASH_KRIT_UKAN,       /**< Adjective (-uka) per 3.2.154 */
  ASH_KRIT_TAVYAT,     /**< Gerundive (-tavya, with sutra 3.1.96) */
  ASH_KRIT_KELIMAR,    /**< Gerundive (-elima) per 3.3.114 */
  ASH_KRIT_RVU,        /**< Agent (-vu) per 3.3.146 */
  ASH_KRIT_MAN,        /**< Action (-ma) per 3.3.169 */
  ASH_KRIT_TRN,        /**< Action (-tṛ, ñit) per 3.2.135 */
  ASH_KRIT_KAS,        /**< Mid/passive (-asa) per 3.1.96 */
  ASH_KRIT_KVASU,      /**< Past active participle (-vas) per 3.2.107 */
  ASH_KRIT_KANAC,      /**< Past middle participle (-āna) per 3.2.106 */
  ASH_KRIT_INI,        /**< Agent (-in) per 3.2.93 */
  /* Phase δ kṛt third tier — more action / agent / participial nouns */
  ASH_KRIT_KIN,        /**< Action (-i, kit) per 3.3.108 */
  ASH_KRIT_KVASUS,     /**< Past active (-vat, alternate to KVASU) per 3.2.108 */
  ASH_KRIT_ATAN,       /**< Reduplicated present participle (-at) */
  ASH_KRIT_KTAVATUS,   /**< Past active participle (-tavat, alternate) per 3.2.103 */
  ASH_KRIT_NAN,        /**< Action (-a, ñit) per 3.3.107 */
  ASH_KRIT_GHURAC,     /**< Adjective (-ura) per 3.2.161 */
  ASH_KRIT_AALUC,      /**< Adjective (-ālu) per 3.2.158 */
  ASH_KRIT_ATRN,       /**< Action (-tra) per 3.3.110 */
  ASH_KRIT_MANIN,      /**< Action (-man) per 3.3.170 */
  ASH_KRIT_RA,         /**< Action (-ra) per 3.3.57 */
  ASH_KRIT_KIT,        /**< Agent (-i, kit) per 3.3.108 */
  ASH_KRIT_BHAVA,      /**< Bhāva noun (-bhava) per 3.3.18 nominal */
  ASH_KRIT_VANIP,      /**< Adjective (-van) per 3.2.74 */
  ASH_KRIT_VANAC,      /**< Action (-vana) per 3.2.74 */
  ASH_KRIT_SHACINIT,   /**< Tense particles (special) per 3.2.108 */
  ASH_KRIT_KTHAN,      /**< Action (-tha) per 3.3.174 */
  ASH_KRIT_KHISHNUC,   /**< Adjective (-iṣṇu, ñit) per 3.2.137 */
  ASH_KRIT_KHA,        /**< Adjective (-kha) per 3.2.6 */
  ASH_KRIT_KTAVYAN,    /**< Gerundive (alternate to tavya) per 3.3.96 */
  ASH_KRIT_KTRP,       /**< Action (-tra) per 3.2.182 */
} ASH_KritType;

/** Derive a kṛt form. */
ASH_Form ash_krit(const ASH_DB *db, const char *root_slp1, int gana,
                  ASH_KritType krit);

/* ═══════════════════════════════════════════════════════════════════════════
 * Compound Formation (Samāsa)
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Samāsa types */
typedef enum {
  ASH_SAMASA_TATPURUSHA = 0,
  ASH_SAMASA_KARMADHAARAYA,
  ASH_SAMASA_BAHUVRIHI,
  ASH_SAMASA_DVANDVA,
  ASH_SAMASA_AVYAYIBHAVA,
  ASH_SAMASA_DVIGU,
} ASH_SamasaType;

/**
 * @brief Form a compound from two prātipadika stems.
 * @param purva_slp1   First member (SLP1).
 * @param uttara_slp1  Second member (SLP1).
 * @param type         Compound type.
 * @param linga        Gender of compound (for bahuvṛīhi etc.).
 */
ASH_Form ash_samasa(const ASH_DB *db,
                    const char *purva_slp1, const char *uttara_slp1,
                    ASH_SamasaType type, ASH_Linga linga);

/* ═══════════════════════════════════════════════════════════════════════════
 * Phonology Utilities
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Expand a pratyāhāra label to NULL-terminated SLP1 phoneme array. */
const char **ash_pratyahara_expand(const char *label);

/** Check if SLP1 phoneme belongs to a pratyāhāra. */
bool ash_in_pratyahara(char phoneme, const char *pratyahara_label);

/** Apply guṇa substitution to a vowel (SLP1). Returns substituted char. */
char ash_guna(char vowel_slp1);

/** Apply vṛddhi substitution to a vowel (SLP1). Returns substituted char. */
char ash_vrddhi(char vowel_slp1);

/* ═══════════════════════════════════════════════════════════════════════════
 * Version
 * ═══════════════════════════════════════════════════════════════════════════ */

#define ASH_VERSION_MAJOR 0
#define ASH_VERSION_MINOR 1
#define ASH_VERSION_PATCH 0
#define ASH_VERSION_STR   "0.1.0"

const char *ash_version(void);

#ifdef __cplusplus
}
#endif

#endif /* ASHTADHYAYI_H */
