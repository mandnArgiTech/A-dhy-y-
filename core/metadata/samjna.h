/* samjna.h — saṃjñā bitmask; implemented in Story 2.2 */
#ifndef SAMJNA_H
#define SAMJNA_H
#include <stdint.h>
#include <stdbool.h>
typedef uint64_t Samjna;
#define SJ_NONE         UINT64_C(0)
#define SJ_DHATU        (UINT64_C(1)<<0)
#define SJ_PRATYAYA     (UINT64_C(1)<<1)
#define SJ_ANGA         (UINT64_C(1)<<2)
#define SJ_PADA         (UINT64_C(1)<<3)
#define SJ_SUBANTA      (UINT64_C(1)<<4)
#define SJ_TINANTA      (UINT64_C(1)<<5)
#define SJ_SARVANAMAN   (UINT64_C(1)<<6)
#define SJ_KARAKA       (UINT64_C(1)<<7)
#define SJ_VIBHAKTI     (UINT64_C(1)<<8)
#define SJ_GUNA         (UINT64_C(1)<<9)
#define SJ_VRDDHI       (UINT64_C(1)<<10)
#define SJ_IT           (UINT64_C(1)<<11)
#define SJ_KIT          (UINT64_C(1)<<12)
#define SJ_NGIT         (UINT64_C(1)<<13)
#define SJ_NNIT         (UINT64_C(1)<<14)
#define SJ_PIT          (UINT64_C(1)<<15)
#define SJ_SARVADHATUKA (UINT64_C(1)<<16)
#define SJ_ARDHADHATUKA (UINT64_C(1)<<17)
#define SJ_TING         (UINT64_C(1)<<18)
#define SJ_SUP          (UINT64_C(1)<<19)
#define SJ_KRT          (UINT64_C(1)<<20)
#define SJ_TADDHITA     (UINT64_C(1)<<21)
#define SJ_SAMASA       (UINT64_C(1)<<22)
#define SJ_NIPATA       (UINT64_C(1)<<23)
#define SJ_UPASARGA     (UINT64_C(1)<<24)
#define SJ_AVYAYA       (UINT64_C(1)<<25)
#define SJ_SAT          (UINT64_C(1)<<26)
#define SJ_NISTHA       (UINT64_C(1)<<27)
#define SJ_KRDANTA      (UINT64_C(1)<<28)
#define SJ_TADDHITANTA  (UINT64_C(1)<<29)
#define SJ_PARASMAIPADA (UINT64_C(1)<<30)
#define SJ_ATMANEPADA   (UINT64_C(1)<<31)
#define SJ_PRATIPADIKA  (UINT64_C(1)<<32)
#define SJ_LAGHU        (UINT64_C(1)<<33)
#define SJ_GURU         (UINT64_C(1)<<34)
#define samjna_has(s,f)    (((s)&(f))!=0)
#define samjna_add(s,f)    ((s)|(f))
#define samjna_remove(s,f) ((s)&~(f))
const char *samjna_describe(Samjna s);
Samjna samjna_of_sutra(uint32_t sutra_global_id);
#endif
