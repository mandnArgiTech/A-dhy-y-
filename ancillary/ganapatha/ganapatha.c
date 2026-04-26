/* ganapatha.c — gaNapATha loading and lookup helpers */
#include "ganapatha.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Trim whitespace from both ends of a mutable C string. */
static char *trim(char *s) {
  char *end;
  while (*s && isspace((unsigned char)*s)) s++;
  end = s + strlen(s);
  while (end > s && isspace((unsigned char)end[-1])) end--;
  *end = '\0';
  return s;
}

/* Copy text safely into a fixed-size destination with guaranteed NUL. */
static void safe_copy(char *dst, size_t dst_len, const char *src) {
  if (!dst || dst_len == 0) return;
  if (!src) src = "";
  strncpy(dst, src, dst_len - 1);
  dst[dst_len - 1] = '\0';
}

/* Parse a TSV line into columns using in-place splitting. */
static int split_tsv(char *line, char *cols[], int max_cols) {
  int n = 0;
  char *p = line;
  if (!line || !cols || max_cols <= 0) return 0;
  cols[n++] = p;
  while (*p && n < max_cols) {
    if (*p == '\t') {
      *p = '\0';
      cols[n++] = p + 1;
    }
    p++;
  }
  return n;
}

/* Load gaNapATha entries from the generated TSV into memory. */
int ganapatha_db_load(GanapathaDB *db, const char *tsv_path) {
  FILE *fp;
  char line[2048];
  int capacity = 0;
  if (!db || !tsv_path) return -1;
  memset(db, 0, sizeof(*db));
  fp = fopen(tsv_path, "r");
  if (!fp) return -1;
  if (!fgets(line, sizeof(line), fp)) {
    fclose(fp);
    return -1;
  }
  while (fgets(line, sizeof(line), fp)) {
    char *cols[8] = {0};
    GanapathaEntry *entry;
    int count = split_tsv(line, cols, 8);
    if (count < 7) continue;
    if (db->count >= capacity) {
      int new_cap = (capacity == 0) ? 256 : capacity * 2;
      GanapathaEntry *new_entries = (GanapathaEntry *)realloc(
          db->entries, (size_t)new_cap * sizeof(GanapathaEntry));
      if (!new_entries) {
        fclose(fp);
        ganapatha_db_free(db);
        return -1;
      }
      db->entries = new_entries;
      capacity = new_cap;
    }
    entry = &db->entries[db->count];
    entry->gana_id = (int)strtol(trim(cols[0]), NULL, 10);
    safe_copy(entry->gana_name_slp1, sizeof(entry->gana_name_slp1), trim(cols[1]));
    safe_copy(entry->sutra_ref, sizeof(entry->sutra_ref), trim(cols[3]));
    entry->serial = (int)strtol(trim(cols[4]), NULL, 10);
    safe_copy(entry->member_slp1, sizeof(entry->member_slp1), trim(cols[5]));
    db->count++;
  }
  fclose(fp);
  return db->count > 0 ? 0 : -1;
}

/* Release all heap memory owned by the gaNapATha DB. */
void ganapatha_db_free(GanapathaDB *db) {
  if (!db) return;
  free(db->entries);
  db->entries = NULL;
  db->count = 0;
}

/* Return the first entry matching a given group name in SLP1. */
const GanapathaEntry *ganapatha_find_group(const GanapathaDB *db, const char *gana_name_slp1) {
  int i;
  if (!db || !gana_name_slp1) return NULL;
  for (i = 0; i < db->count; i++) {
    if (strcmp(db->entries[i].gana_name_slp1, gana_name_slp1) == 0) {
      return &db->entries[i];
    }
  }
  return NULL;
}

/* Return true if the requested member occurs in the named gaNa group. */
bool ganapatha_has_member(const GanapathaDB *db,
                          const char *gana_name_slp1,
                          const char *member_slp1) {
  int i;
  if (!db || !gana_name_slp1 || !member_slp1) return false;
  for (i = 0; i < db->count; i++) {
    if (strcmp(db->entries[i].gana_name_slp1, gana_name_slp1) == 0 &&
        strcmp(db->entries[i].member_slp1, member_slp1) == 0) {
      return true;
    }
  }
  return false;
}
