/* adhikara.h — stub; Story 2.4 */
#ifndef ADHIKARA_H
#define ADHIKARA_H
#include "sutra.h"
int  adhikara_build(SutraDB *db, const char *tsv);
bool adhikara_governs(const SutraDB *db, uint32_t adhikara_id, uint32_t child_id);
uint32_t adhikara_parent_of(const SutraDB *db, uint32_t sutra_id);
int  adhikara_parents_of(const SutraDB *db, uint32_t sutra_id, uint32_t *out, int max);
#endif
