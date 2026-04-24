/* sutra_loader.c — stub; implemented in Story 2.1 */
#include "sutra.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
int sutra_db_load(SutraDB *db, const char *tsv_path) {
  if (!db || !tsv_path) return -1;
  memset(db, 0, sizeof(*db));
  FILE *f = fopen(tsv_path, "r");
  if (!f) return -1;  /* TSV not yet generated — that's fine */
  fclose(f);
  return 0;  /* stub: load nothing, just succeed */
}
void sutra_db_free(SutraDB *db) {
  if (!db) return;
  free(db->sutras);
  db->sutras = NULL;
  db->count = 0;
}
const Sutra *sutra_get_by_id(const SutraDB *db, uint32_t id) {
  if (!db || !db->sutras || id < 1 || id > db->count) return NULL;
  return &db->sutras[id - 1];
}
const Sutra *sutra_get_by_addr(const SutraDB *db, int a, int p, int n) {
  if (!db || a<1||a>8||p<1||p>4||n<1||n>249) return NULL;
  uint32_t id = db->addr_index[a][p][n];
  return id ? sutra_get_by_id(db, id) : NULL;
}
void sutra_print(const Sutra *s, FILE *f) {
  if (!s || !f) return;
  fprintf(f, "%u.%u.%u  %s\n", s->adhyaya, s->pada, s->num, s->text_slp1);
}
void sutra_foreach_range(const SutraDB *db, uint32_t from, uint32_t to,
                          SutraVisitor fn, void *ud) {
  if (!db || !fn) return;
  for (uint32_t i = from; i <= to && i <= db->count; i++)
    fn(&db->sutras[i-1], ud);
}
int sutra_filter_by_type(const SutraDB *db, ASH_SutraType type,
                          const Sutra **out, int max) {
  if (!db || !out) return 0;
  int n = 0;
  for (uint32_t i = 0; i < db->count && n < max; i++)
    if (db->sutras[i].type == type) out[n++] = &db->sutras[i];
  return n;
}
