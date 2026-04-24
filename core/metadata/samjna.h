/* samjna.h — saṃjñā (technical term) bitmask registry
 * Story 2.2 implementation
 *
 * Pāṇini defines ~38 technical terms throughout the Aṣṭādhyāyī. A Term in
 * the derivation engine carries a Samjna bitmask marking every category
 * that applies to it, enabling O(1) membership tests.
 */

#ifndef SAMJNA_H
#define SAMJNA_H

#include <stdint.h>
#include <stdbool.h>

/* 64-bit bitmask — supports up to 64 simultaneous saṃjñās */
typedef uint64_t Samjna;

/* ── Saṃjñā constants (38 categories) ────────────────────────────────── */
#define SJ_NONE          UINT64_C(0)
#define SJ_DHATU         (UINT64_C(1) <<  0)  /* 1.3.1  — verbal root */
#define SJ_PRATYAYA      (UINT64_C(1) <<  1)  /* 3.1.1  — suffix */
#define SJ_ANGA          (UINT64_C(1) <<  2)  /* 6.4.1  — stem before suffix */
#define SJ_PADA          (UINT64_C(1) <<  3)  /* 1.4.14 — word-form */
#define SJ_SUBANTA       (UINT64_C(1) <<  4)  /* 2.4.82 — nominal word */
#define SJ_TINANTA       (UINT64_C(1) <<  5)  /* 2.4.82 — verbal word */
#define SJ_SARVANAMAN    (UINT64_C(1) <<  6)  /* 1.1.27 — pronoun */
#define SJ_KARAKA        (UINT64_C(1) <<  7)  /* 1.4.23 — semantic role */
#define SJ_VIBHAKTI      (UINT64_C(1) <<  8)  /* case suffix */
#define SJ_GUNA          (UINT64_C(1) <<  9)  /* 1.1.2  — medium vowel */
#define SJ_VRDDHI        (UINT64_C(1) << 10)  /* 1.1.1  — high vowel */
#define SJ_IT            (UINT64_C(1) << 11)  /* 1.3.2  — marker letter */
#define SJ_KIT           (UINT64_C(1) << 12)  /* k-it suffix */
#define SJ_NGIT          (UINT64_C(1) << 13)  /* ṅ-it suffix */
#define SJ_NNIT          (UINT64_C(1) << 14)  /* ṇ-it suffix */
#define SJ_PIT           (UINT64_C(1) << 15)  /* p-it suffix */
#define SJ_SARVADHATUKA  (UINT64_C(1) << 16)  /* 3.4.113 */
#define SJ_ARDHADHATUKA  (UINT64_C(1) << 17)  /* 3.4.114 */
#define SJ_TING          (UINT64_C(1) << 18)  /* 3.4.77  — personal ending */
#define SJ_SUP           (UINT64_C(1) << 19)  /* 4.1.2   — nominal suffix */
#define SJ_KRT           (UINT64_C(1) << 20)  /* 3.1.93  — primary suffix */
#define SJ_TADDHITA      (UINT64_C(1) << 21)  /* 4.1.76  — secondary suffix */
#define SJ_SAMASA        (UINT64_C(1) << 22)  /* compound member */
#define SJ_NIPATA        (UINT64_C(1) << 23)  /* 1.4.56  — particle */
#define SJ_UPASARGA      (UINT64_C(1) << 24)  /* 1.4.59  — verbal prefix */
#define SJ_AVYAYA        (UINT64_C(1) << 25)  /* 1.1.37  — indeclinable */
#define SJ_SAT           (UINT64_C(1) << 26)  /* 3.2.127 — pres participle */
#define SJ_NISTHA        (UINT64_C(1) << 27)  /* 1.1.26  — kta/ktavat */
#define SJ_KRDANTA       (UINT64_C(1) << 28)  /* kṛt-ending stem */
#define SJ_TADDHITANTA   (UINT64_C(1) << 29)  /* taddhita-ending stem */
#define SJ_PARASMAIPADA  (UINT64_C(1) << 30)
#define SJ_ATMANEPADA    (UINT64_C(1) << 31)
#define SJ_PRATIPADIKA   (UINT64_C(1) << 32)  /* 1.2.45 */
#define SJ_LAGHU         (UINT64_C(1) << 33)  /* light syllable */
#define SJ_GURU          (UINT64_C(1) << 34)  /* heavy syllable */
#define SJ_ANUDATTA      (UINT64_C(1) << 35)  /* grave accent */
#define SJ_UDATTA        (UINT64_C(1) << 36)  /* acute accent */
#define SJ_SVARITA       (UINT64_C(1) << 37)  /* circumflex accent */

/* ── Operations ──────────────────────────────────────────────────────── */
#define samjna_has(s, flag)     (((s) & (flag)) != 0)
#define samjna_add(s, flag)     ((s) | (flag))
#define samjna_remove(s, flag)  ((s) & ~(flag))

/** Return human-readable names for a saṃjñā bitmask (static buffer). */
const char *samjna_describe(Samjna s);

/** Get the saṃjñā defined by a specific sūtra global_id, or SJ_NONE. */
Samjna samjna_of_sutra(uint32_t sutra_global_id);

/** Convenience: bitmask of all structural (non-accent) saṃjñās. */
#define SJ_STRUCTURAL_MASK \
  (~(SJ_ANUDATTA | SJ_UDATTA | SJ_SVARITA | SJ_LAGHU | SJ_GURU))

#endif /* SAMJNA_H */
