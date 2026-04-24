/* karaka.c — kāraka → vibhakti mapping; Story 4.5 */
#include "karaka.h"
#include "ashtadhyayi.h"
ASH_Vibhakti karaka_default_vibhakti(ASH_Karaka k) {
  switch(k) {
    case ASH_KARTA:      return ASH_PRATHAMA_VIB;
    case ASH_KARMAN:     return ASH_DVITIYA_VIB;
    case ASH_KARANA:     return ASH_TRITIYA_VIB;
    case ASH_SAMPRADANA: return ASH_CATURTHI_VIB;
    case ASH_APADANA:    return ASH_PANCAMI_VIB;
    case ASH_ADHIKARANA: return ASH_SAPTAMI_VIB;
    default:             return ASH_PRATHAMA_VIB;
  }
}
const char *karaka_name(ASH_Karaka k) {
  static const char *names[] = {"kartṛ","karman","karaṇa","sampradāna","apādāna","adhikaraṇa"};
  return (k >= 0 && k <= ASH_ADHIKARANA) ? names[k] : "unknown";
}
const char *vibhakti_name(ASH_Vibhakti v) {
  static const char *names[] = {"prathama","dvitīya","tṛtīyā","caturthī",
                                  "pañcamī","ṣaṣṭhī","saptamī","sambodhana"};
  return (v >= 0 && v <= ASH_SAMBODHANA_VIB) ? names[v] : "unknown";
}
