/* unadi.c — Story 5.5 implementation */
#include "unadi.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int unadi_parse_line(UnadiEntry *entry, char *line) {
  if (!entry || !line) return -1;
  char *cols[6] = {0};
  int ncol = 0;
  char *p = line;
  while (ncol < 6 && p) {
    cols[ncol++] = p;
    p = strchr(p, '\t');
    if (p) {
      *p = '\0';
      p++;
    }
  }
  if (ncol < 6) return -1;
  entry->unadi_id = (uint32_t)strtoul(cols[0], NULL, 10);
  strncpy(entry->root_slp1, cols[1], sizeof(entry->root_slp1) - 1);
  strncpy(entry->suffix_slp1, cols[2], sizeof(entry->suffix_slp1) - 1);
  strncpy(entry->form_slp1, cols[3], sizeof(entry->form_slp1) - 1);
  strncpy(entry->meaning_en, cols[4], sizeof(entry->meaning_en) - 1);
  strncpy(entry->sutra_ref, cols[5], sizeof(entry->sutra_ref) - 1);
  return 0;
}

int unadi_db_load(UnadiDB *db, const char *tsv_path) {
  if (!db || !tsv_path) return -1;
  memset(db, 0, sizeof(*db));
  FILE *fp = fopen(tsv_path, "r");
  if (!fp && strncmp(tsv_path, "data/", 5) == 0) {
    /* Tests often run from build directories; retry via parent path. */
    char alt_path[512];
    snprintf(alt_path, sizeof(alt_path), "../%s", tsv_path);
    fp = fopen(alt_path, "r");
  }
  if (!fp) return -1;

  char line[1024];
  if (!fgets(line, sizeof(line), fp)) {
    fclose(fp);
    return -1;
  }

  int capacity = 256;
  db->entries = calloc(capacity, sizeof(UnadiEntry));
  if (!db->entries) {
    fclose(fp);
    return -1;
  }

  while (fgets(line, sizeof(line), fp)) {
    size_t n = strlen(line);
    while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r')) {
      line[--n] = '\0';
    }
    if (line[0] == '\0') continue;
    if (db->count >= capacity) {
      capacity *= 2;
      UnadiEntry *grown = realloc(db->entries, (size_t)capacity * sizeof(UnadiEntry));
      if (!grown) {
        fclose(fp);
        free(db->entries);
        db->entries = NULL;
        db->count = 0;
        return -1;
      }
      db->entries = grown;
    }
    if (unadi_parse_line(&db->entries[db->count], line) == 0) {
      db->count++;
    }
  }
  fclose(fp);
  return 0;
}

void unadi_db_free(UnadiDB *db) {
  if (!db) return;
  free(db->entries);
  db->entries = NULL;
  db->count = 0;
}

const UnadiEntry *unadi_lookup(const UnadiDB *db,
                               const char *root_slp1,
                               const char *suffix_slp1) {
  if (!db || !db->entries || !root_slp1) return NULL;
  for (int i = 0; i < db->count; i++) {
    if (strcmp(db->entries[i].root_slp1, root_slp1) != 0) continue;
    if (!suffix_slp1 || strcmp(db->entries[i].suffix_slp1, suffix_slp1) == 0) {
      return &db->entries[i];
    }
  }
  return NULL;
}

bool unadi_is_attested(const UnadiDB *db, const char *form_slp1) {
  if (!db || !db->entries || !form_slp1) return false;
  for (int i = 0; i < db->count; i++) {
    if (strcmp(db->entries[i].form_slp1, form_slp1) == 0) return true;
  }
  return false;
}

ASH_Form unadi_form(const UnadiDB *db, const char *root_slp1,
                    const char *suffix_slp1) {
  const UnadiEntry *entry = unadi_lookup(db, root_slp1, suffix_slp1);
  ASH_Form f = {0};
  if (!entry) {
    f.valid = false;
    snprintf(f.error, sizeof(f.error), "Not in uNAdi corpus: %s + %s",
             root_slp1 ? root_slp1 : "(null)",
             suffix_slp1 ? suffix_slp1 : "(any)");
    return f;
  }

  f.valid = true;
  strncpy(f.slp1, entry->form_slp1, sizeof(f.slp1) - 1);
  char *iast = enc_slp1_to_iast(entry->form_slp1);
  if (iast) {
    strncpy(f.iast, iast, sizeof(f.iast) - 1);
    free(iast);
  }
  char *deva = enc_slp1_to_devanagari(entry->form_slp1);
  if (deva) {
    strncpy(f.devanagari, deva, sizeof(f.devanagari) - 1);
    free(deva);
  }

  f.steps = calloc(1, sizeof(ASH_PrakriyaStep));
  if (!f.steps) {
    f.valid = false;
    strncpy(f.error, "allocation failure", sizeof(f.error) - 1);
    return f;
  }
  f.step_count = 1;
  f.steps[0].sutra_id = 500500 + entry->unadi_id;
  strncpy(f.steps[0].before_slp1, root_slp1 ? root_slp1 : "",
          sizeof(f.steps[0].before_slp1) - 1);
  strncpy(f.steps[0].after_slp1, f.slp1, sizeof(f.steps[0].after_slp1) - 1);
  snprintf(f.steps[0].note, sizeof(f.steps[0].note),
           "uNAdi %s", entry->sutra_ref);

  return f;
}
