/* unadi.c — Uṇādi-paṭha lookup database backed by data/unadipatha.tsv. */
#include "unadi.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Copy at most `dst_len-1` bytes from src into dst, NUL-terminated. */
static void copy_field(char *dst, size_t dst_len, const char *src) {
  if (!dst || dst_len == 0) return;
  if (!src) { dst[0] = '\0'; return; }
  size_t n = strlen(src);
  if (n >= dst_len) n = dst_len - 1;
  memcpy(dst, src, n);
  dst[n] = '\0';
}

/* Split one TSV row in-place into up to 7 NUL-terminated fields. Returns the
   number of fields produced (>=1 for any non-empty input). */
static int split_tsv(char *line, char *fields[7]) {
  int count = 0;
  char *p = line;
  fields[0] = p;
  while (*p && count < 7) {
    if (*p == '\t') {
      *p = '\0';
      count++;
      if (count < 7) fields[count] = p + 1;
    } else if (*p == '\n' || *p == '\r') {
      *p = '\0';
    }
    p++;
  }
  if (fields[count] && fields[count][0] != '\0') count++;
  return count;
}

/* Parse one TSV row (already stripped of trailing newline) into an entry. */
static bool parse_entry_line(char *line, UnadiEntry *entry) {
  char *fields[7] = {0};
  if (!line || !entry) return false;
  int n = split_tsv(line, fields);
  if (n < 5) return false;
  entry->unadi_id = (uint32_t)strtoul(fields[0], NULL, 10);
  copy_field(entry->sutra_deva,    sizeof(entry->sutra_deva),    fields[1]);
  copy_field(entry->sutra_slp1,    sizeof(entry->sutra_slp1),    fields[2]);
  copy_field(entry->pratyay_deva,  sizeof(entry->pratyay_deva),  fields[3]);
  copy_field(entry->pratyay_slp1,  sizeof(entry->pratyay_slp1),  fields[4]);
  /* fields[5] is sk_commentary — long Devanāgarī text we do not store
     in-memory; keep ingestion lightweight. */
  copy_field(entry->sutra_ref, sizeof(entry->sutra_ref), n >= 7 ? fields[6] : "");
  return entry->sutra_slp1[0] != '\0';
}

int unadi_db_load(UnadiDB *db, const char *tsv_path) {
  if (!db || !tsv_path) return -1;
  memset(db, 0, sizeof(*db));
  FILE *fp = fopen(tsv_path, "r");
  if (!fp) return -1;
  /* Long sk_commentary cells push lines well past 1 KB; size accordingly. */
  char line[8192];
  if (!fgets(line, sizeof(line), fp)) {
    fclose(fp);
    return -1;
  }
  int capacity = 64;
  db->entries = calloc((size_t)capacity, sizeof(UnadiEntry));
  if (!db->entries) {
    fclose(fp);
    return -1;
  }
  while (fgets(line, sizeof(line), fp)) {
    UnadiEntry parsed = {0};
    if (!parse_entry_line(line, &parsed)) continue;
    if (db->count >= capacity) {
      capacity *= 2;
      UnadiEntry *grown = realloc(db->entries, (size_t)capacity * sizeof(UnadiEntry));
      if (!grown) {
        unadi_db_free(db);
        fclose(fp);
        return -1;
      }
      db->entries = grown;
    }
    db->entries[db->count++] = parsed;
  }
  fclose(fp);
  if (db->count == 0) {
    unadi_db_free(db);
    return -1;
  }
  return 0;
}

void unadi_db_free(UnadiDB *db) {
  if (!db) return;
  free(db->entries);
  db->entries = NULL;
  db->count = 0;
}

const UnadiEntry *unadi_lookup_by_id(const UnadiDB *db, uint32_t id) {
  if (!db) return NULL;
  for (int i = 0; i < db->count; i++) {
    if (db->entries[i].unadi_id == id) return &db->entries[i];
  }
  return NULL;
}

const UnadiEntry *unadi_lookup_by_pratyay(const UnadiDB *db,
                                          const char *pratyay_slp1) {
  if (!db || !pratyay_slp1) return NULL;
  for (int i = 0; i < db->count; i++) {
    if (strcmp(db->entries[i].pratyay_slp1, pratyay_slp1) == 0) {
      return &db->entries[i];
    }
  }
  return NULL;
}

ASH_Form unadi_cite_pratyay(const UnadiDB *db, const char *pratyay_slp1) {
  ASH_Form f = {0};
  const UnadiEntry *entry = unadi_lookup_by_pratyay(db, pratyay_slp1);
  if (!entry) {
    f.valid = false;
    snprintf(f.error, sizeof(f.error), "unadi pratyay not found");
    return f;
  }
  f.valid = true;
  strncpy(f.slp1, entry->pratyay_slp1, sizeof(f.slp1) - 1);
  {
    char *iast = enc_slp1_to_iast(f.slp1);
    if (iast) { strncpy(f.iast, iast, sizeof(f.iast) - 1); free(iast); }
  }
  {
    char *deva = enc_slp1_to_devanagari(f.slp1);
    if (deva) { strncpy(f.devanagari, deva, sizeof(f.devanagari) - 1); free(deva); }
  }
  f.step_count = 1;
  f.steps = calloc(1, sizeof(ASH_PrakriyaStep));
  if (!f.steps) {
    f.valid = false;
    snprintf(f.error, sizeof(f.error), "oom");
    return f;
  }
  f.steps[0].sutra_id = entry->unadi_id;
  snprintf(f.steps[0].note, sizeof(f.steps[0].note),
           "unadi:%u %s", entry->unadi_id, entry->pratyay_slp1);
  return f;
}
