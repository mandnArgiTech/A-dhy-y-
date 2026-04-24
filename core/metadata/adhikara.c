/* adhikara.c — stub; Story 2.4 */
#include "adhikara.h"
int  adhikara_build(SutraDB *db, const char *tsv) { (void)db;(void)tsv; return 0; }
bool adhikara_governs(const SutraDB *db, uint32_t a, uint32_t c) {
  (void)db; return a < c; /* stub */
}
uint32_t adhikara_parent_of(const SutraDB *db, uint32_t id) { (void)db;(void)id; return 0; }
int adhikara_parents_of(const SutraDB *db, uint32_t id, uint32_t *out, int max) {
  (void)db;(void)id;(void)out;(void)max; return 0;
}
