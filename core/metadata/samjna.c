/* samjna.c — Story 2.2 implementation
 *
 * Human-readable description of bitmasks + sūtra_id → Samjna mapping.
 */

#include "samjna.h"
#include <stdio.h>
#include <string.h>

/* ── Mapping table: bit position → name ──────────────────────────────── */

typedef struct {
  Samjna      flag;
  const char *name;
} SamjnaName;

static const SamjnaName NAMES[] = {
  { SJ_DHATU,         "DHATU" },
  { SJ_PRATYAYA,      "PRATYAYA" },
  { SJ_ANGA,          "ANGA" },
  { SJ_PADA,          "PADA" },
  { SJ_SUBANTA,       "SUBANTA" },
  { SJ_TINANTA,       "TINANTA" },
  { SJ_SARVANAMAN,    "SARVANAMAN" },
  { SJ_KARAKA,        "KARAKA" },
  { SJ_VIBHAKTI,      "VIBHAKTI" },
  { SJ_GUNA,          "GUNA" },
  { SJ_VRDDHI,        "VRDDHI" },
  { SJ_IT,            "IT" },
  { SJ_KIT,           "KIT" },
  { SJ_NGIT,          "NGIT" },
  { SJ_NNIT,          "NNIT" },
  { SJ_PIT,           "PIT" },
  { SJ_SARVADHATUKA,  "SARVADHATUKA" },
  { SJ_ARDHADHATUKA,  "ARDHADHATUKA" },
  { SJ_TING,          "TING" },
  { SJ_SUP,           "SUP" },
  { SJ_KRT,           "KRT" },
  { SJ_TADDHITA,      "TADDHITA" },
  { SJ_SAMASA,        "SAMASA" },
  { SJ_NIPATA,        "NIPATA" },
  { SJ_UPASARGA,      "UPASARGA" },
  { SJ_AVYAYA,        "AVYAYA" },
  { SJ_SAT,           "SAT" },
  { SJ_NISTHA,        "NISTHA" },
  { SJ_KRDANTA,       "KRDANTA" },
  { SJ_TADDHITANTA,   "TADDHITANTA" },
  { SJ_PARASMAIPADA,  "PARASMAIPADA" },
  { SJ_ATMANEPADA,    "ATMANEPADA" },
  { SJ_PRATIPADIKA,   "PRATIPADIKA" },
  { SJ_LAGHU,         "LAGHU" },
  { SJ_GURU,          "GURU" },
  { SJ_ANUDATTA,      "ANUDATTA" },
  { SJ_UDATTA,        "UDATTA" },
  { SJ_SVARITA,       "SVARITA" },
  { 0, NULL }
};

/* ── samjna_describe ──────────────────────────────────────────────────── */

const char *samjna_describe(Samjna s) {
  static char buf[512];
  if (s == SJ_NONE) { strcpy(buf, "NONE"); return buf; }

  size_t pos = 0;
  buf[0] = '\0';
  for (int i = 0; NAMES[i].name; i++) {
    if ((s & NAMES[i].flag) != 0) {
      size_t n = strlen(NAMES[i].name);
      if (pos > 0 && pos + 1 < sizeof(buf)) { buf[pos++] = '|'; buf[pos] = '\0'; }
      if (pos + n + 1 < sizeof(buf)) {
        memcpy(buf + pos, NAMES[i].name, n);
        pos += n;
        buf[pos] = '\0';
      }
    }
  }
  if (pos == 0) strcpy(buf, "NONE");
  return buf;
}

/* ── samjna_of_sutra ──────────────────────────────────────────────────────
 *
 * Maps the global_id of key saṃjñā-defining sūtras to their flag.
 * We can't look up by (adhyaya,pada,num) tuple without a db pointer, so
 * this uses the traditional address encoded as uint32:
 *   adhyaya*10000 + pada*100 + num
 */

typedef struct {
  uint32_t addr;   /* adhyaya*10000 + pada*100 + num */
  Samjna   flag;
} SutraSamjna;

static const SutraSamjna SUTRA_SAMJNA[] = {
  { 10101, SJ_VRDDHI       },  /* 1.1.1 vṛddhirādaic */
  { 10102, SJ_GUNA         },  /* 1.1.2 adeṅ guṇaḥ */
  { 10126, SJ_NISTHA       },  /* 1.1.26 ktaktavatū niṣṭhā */
  { 10127, SJ_SARVANAMAN   },  /* 1.1.27 sarvādīni sarvanāmāni */
  { 10137, SJ_AVYAYA       },  /* 1.1.37 svarādi-nipātam avyayam */
  { 10245, SJ_PRATIPADIKA  },  /* 1.2.45 arthavad adhātur apratyayaḥ prātipadikam */
  { 10301, SJ_DHATU        },  /* 1.3.1 bhūvādayo dhātavaḥ */
  { 10302, SJ_IT           },  /* 1.3.2 upadeśe 'j-anunāsika it */
  { 10414, SJ_PADA         },  /* 1.4.14 suptiṅantaṃ padam */
  { 10423, SJ_KARAKA       },  /* 1.4.23 kārake */
  { 10456, SJ_NIPATA       },  /* 1.4.56 prāg rīśvarān nipātāḥ */
  { 10459, SJ_UPASARGA     },  /* 1.4.59 upasargāḥ kriyāyoge */
  { 20103, SJ_SAMASA       },  /* 2.1.3 prāk kaḍārāt samāsaḥ */
  { 20482, SJ_SUBANTA      },  /* 2.4.82 avyayād āpsupaḥ — defines subanta/tinanta */
  { 30101, SJ_PRATYAYA     },  /* 3.1.1 pratyayaḥ */
  { 30193, SJ_KRT          },  /* 3.1.93 kṛdatiṅ */
  { 30377, SJ_TING         },  /* 3.4.77 laḥ parasmaipadam */
  { 30404, SJ_ATMANEPADA   },  /* 3.4.78 tiṅstrīṇi trīṇi prathama... */
  { 30413, SJ_SARVADHATUKA },  /* 3.4.113 tiṅśit sārvadhātukam */
  { 30414, SJ_ARDHADHATUKA },  /* 3.4.114 ārdhadhātukaṃ śeṣaḥ */
  { 40102, SJ_SUP          },  /* 4.1.2 svaujasamauṭchaṣṭā... */
  { 40176, SJ_TADDHITA     },  /* 4.1.76 taddhitāḥ */
  { 60401, SJ_ANGA         },  /* 6.4.1 aṅgasya */
  { 0, 0 }
};

Samjna samjna_of_sutra(uint32_t sutra_global_id) {
  /* Caller typically passes a traditional address — attempt direct lookup */
  for (int i = 0; SUTRA_SAMJNA[i].addr; i++) {
    if (SUTRA_SAMJNA[i].addr == sutra_global_id) return SUTRA_SAMJNA[i].flag;
  }
  return SJ_NONE;
}
