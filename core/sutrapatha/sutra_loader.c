/* sutra_loader.c — Story 2.1: parse data/sutras.tsv */
#include "sutra.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static ASH_SutraType parse_type(const char *s) {
  if (!s || !*s) return ASH_SUTRA_VIDHI;
  switch (s[0]) {
    case 'S': return ASH_SUTRA_SAMJNA;
    case 'P': return ASH_SUTRA_PARIBHASHA;
    case 'A': return ASH_SUTRA_ADHIKARA;
    case 'V': return ASH_SUTRA_VIDHI;
    case 'N': return ASH_SUTRA_NIYAMA;
    case 'T': return ASH_SUTRA_ATIDESHA;
    case 'I': return ASH_SUTRA_NISHEDHA;
    default:  return ASH_SUTRA_VIDHI;
  }
}

/* Skip a single tab-separated field and return pointer to next field */
static char *next_field(char *p) {
  while (*p && *p != '\t') p++;
  if (*p == '\t') { *p = '\0'; p++; }
  return p;
}

int sutra_db_load(SutraDB *db, const char *tsv_path) {
  if (!db || !tsv_path) return -1;
  memset(db, 0, sizeof(*db));

  FILE *f = fopen(tsv_path, "r");
  if (!f) return -1;

  /* First pass: count lines */
  char buf[2048];
  uint32_t count = 0;
  if (!fgets(buf, sizeof(buf), f)) { fclose(f); return -1; }  /* skip header */
  while (fgets(buf, sizeof(buf), f)) count++;
  rewind(f);
  fgets(buf, sizeof(buf), f);  /* skip header again */

  db->sutras = calloc(count, sizeof(Sutra));
  if (!db->sutras) { fclose(f); return -1; }

  /* Second pass: parse */
  uint32_t idx = 0;
  while (fgets(buf, sizeof(buf), f) && idx < count) {
    /* remove trailing newline */
    size_t n = strlen(buf);
    while (n && (buf[n-1] == '\n' || buf[n-1] == '\r')) buf[--n] = '\0';

    char *p = buf;
    char *field_gid  = p; p = next_field(p);
    char *field_adh  = p; p = next_field(p);
    char *field_pad  = p; p = next_field(p);
    char *field_num  = p; p = next_field(p);
    /* skip text_deva */               p = next_field(p);
    char *field_slp1 = p; p = next_field(p);
    char *field_type = p; p = next_field(p);

    Sutra *s = &db->sutras[idx];
    s->global_id = (uint32_t)strtoul(field_gid, NULL, 10);
    s->adhyaya   = (uint16_t)strtoul(field_adh, NULL, 10);
    s->pada      = (uint8_t) strtoul(field_pad, NULL, 10);
    s->num       = (uint16_t)strtoul(field_num, NULL, 10);
    s->type      = parse_type(field_type);
    strncpy(s->text_slp1, field_slp1, SUTRA_TEXT_LEN - 1);

    /* Build addr_index */
    if (s->adhyaya >= 1 && s->adhyaya <= 8 &&
        s->pada    >= 1 && s->pada    <= 4 &&
        s->num     >= 1 && s->num     <  250) {
      db->addr_index[s->adhyaya][s->pada][s->num] = s->global_id;
    }
    idx++;
  }
  db->count = idx;
  fclose(f);
  return 0;
}

void sutra_db_free(SutraDB *db) {
  if (!db) return;
  free(db->sutras);
  memset(db, 0, sizeof(*db));
}

const Sutra *sutra_get_by_id(const SutraDB *db, uint32_t id) {
  if (!db || !db->sutras || id < 1 || id > db->count) return NULL;
  return &db->sutras[id - 1];
}

const Sutra *sutra_get_by_addr(const SutraDB *db, int a, int p, int n) {
  if (!db || a < 1 || a > 8 || p < 1 || p > 4 || n < 1 || n >= 250) return NULL;
  uint32_t id = db->addr_index[a][p][n];
  return id ? sutra_get_by_id(db, id) : NULL;
}

void sutra_print(const Sutra *s, FILE *f) {
  if (!s || !f) return;
  fprintf(f, "%u.%u.%u  [gid=%u type=%d]  %s\n",
          s->adhyaya, s->pada, s->num, s->global_id, (int)s->type, s->text_slp1);
}

void sutra_foreach_range(const SutraDB *db, uint32_t from, uint32_t to,
                          SutraVisitor fn, void *ud) {
  if (!db || !fn) return;
  for (uint32_t i = from; i <= to && i <= db->count; i++) {
    fn(&db->sutras[i-1], ud);
  }
}

int sutra_filter_by_type(const SutraDB *db, ASH_SutraType type,
                          const Sutra **out, int max) {
  if (!db || !out) return 0;
  int n = 0;
  for (uint32_t i = 0; i < db->count && n < max; i++) {
    if (db->sutras[i].type == type) out[n++] = &db->sutras[i];
  }
  return n;
}
