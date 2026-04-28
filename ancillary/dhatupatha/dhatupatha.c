/* dhatupatha.c — dhatupatha TSV loader and lookup helpers. */
#include "dhatupatha.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Parse one TSV line into fields by replacing tabs with NUL terminators. */
static int split_tabs(char *line, char *cols[], int max_cols) {
  int count = 0;
  char *p = line;
  if (!line || !cols || max_cols <= 0) return 0;
  cols[count++] = p;
  while (*p && count < max_cols) {
    if (*p == '\t') {
      *p = '\0';
      cols[count++] = p + 1;
    }
    p++;
  }
  return count;
}

/* Trim trailing newline/carriage return from a mutable line buffer. */
static void trim_eol(char *line) {
  size_t n;
  if (!line) return;
  n = strlen(line);
  while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r')) {
    line[n - 1] = '\0';
    n--;
  }
}

/* Load dhatu entries from TSV generated in phase-0 ingestion. */
int dhatupatha_db_load(DhatupathaDB *db, const char *tsv_path) {
  FILE *fp;
  char line[1024];
  size_t cap = 0;
  if (!db || !tsv_path) return -1;
  memset(db, 0, sizeof(*db));

  fp = fopen(tsv_path, "r");
  if (!fp) return -1;

  /* Skip header. */
  if (!fgets(line, sizeof(line), fp)) {
    fclose(fp);
    return -1;
  }

  while (fgets(line, sizeof(line), fp)) {
    char *cols[16] = {0};
    int ncols;
    DhatupathaEntry *e;
    trim_eol(line);
    ncols = split_tabs(line, cols, 16);
    if (ncols < 8) continue;
    if ((size_t)db->count >= cap) {
      size_t new_cap = cap == 0 ? 256u : cap * 2u;
      DhatupathaEntry *new_entries =
          (DhatupathaEntry *)realloc(db->entries, new_cap * sizeof(DhatupathaEntry));
      if (!new_entries) {
        fclose(fp);
        dhatupatha_db_free(db);
        return -1;
      }
      db->entries = new_entries;
      cap = new_cap;
    }
    e = &db->entries[db->count];
    memset(e, 0, sizeof(*e));
    e->global_id = (uint32_t)strtoul(cols[0], NULL, 10);
    e->gana = (int)strtol(cols[1], NULL, 10);
    e->serial_in_gana = (int)strtol(cols[2], NULL, 10);
    strncpy(e->upadesa_slp1, cols[4], sizeof(e->upadesa_slp1) - 1);
    strncpy(e->meaning_en, cols[6], sizeof(e->meaning_en) - 1);
    e->pada_flag = cols[7][0] ? cols[7][0] : 'U';
    e->settva[0] = cols[8][0] ? cols[8][0] : '\0';
    e->settva[1] = '\0';
    db->count++;
  }

  fclose(fp);
  return db->count > 0 ? 0 : -1;
}

/* Release all heap allocations associated with a dhatupatha database. */
void dhatupatha_db_free(DhatupathaDB *db) {
  if (!db) return;
  free(db->entries);
  db->entries = NULL;
  db->count = 0;
}

/* Find a dhatu by normalized SLP1 root and optional gana hint. */
const DhatupathaEntry *dhatupatha_find(const DhatupathaDB *db,
                                       const char *root_slp1, int gana_hint) {
  int i;
  if (!db || !root_slp1) return NULL;
  for (i = 0; i < db->count; i++) {
    const DhatupathaEntry *e = &db->entries[i];
    if (strcmp(e->upadesa_slp1, root_slp1) != 0) continue;
    if (gana_hint != 0 && e->gana != gana_hint) continue;
    return e;
  }
  return NULL;
}

/* Return true when the requested root is present in the loaded corpus. */
bool dhatupatha_contains(const DhatupathaDB *db, const char *clean_slp1) {
  return dhatupatha_find(db, clean_slp1, 0) != NULL;
}
