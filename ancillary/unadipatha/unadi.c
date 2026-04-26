/* unadi.c — lightweight Uṇādi lookup database and form adapter. */
#include "unadi.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Parse one TSV row into an entry record. */
static bool parse_entry_line(const char *line, UnadiEntry *entry) {
  unsigned int unadi_id = 0;
  int n = 0;
  if (!line || !entry) return false;
  n = sscanf(line, "%u\t%31[^\t]\t%15[^\t]\t%63[^\t]\t%127[^\t]\t%31[^\n]",
             &unadi_id,
             entry->root_slp1,
             entry->suffix_slp1,
             entry->form_slp1,
             entry->meaning_en,
             entry->sutra_ref);
  if (n < 6) return false;
  entry->unadi_id = unadi_id;
  return true;
}

/* Load a small TSV-backed Uṇādi database into memory. */
int unadi_db_load(UnadiDB *db, const char *tsv_path) {
  FILE *fp = NULL;
  char line[512];
  int capacity = 0;
  if (!db || !tsv_path) return -1;
  memset(db, 0, sizeof(*db));
  fp = fopen(tsv_path, "r");
  if (!fp) return -1;
  if (!fgets(line, sizeof(line), fp)) {
    fclose(fp);
    return -1;
  }
  capacity = 32;
  db->entries = calloc((size_t)capacity, sizeof(UnadiEntry));
  if (!db->entries) {
    fclose(fp);
    return -1;
  }
  while (fgets(line, sizeof(line), fp)) {
    UnadiEntry parsed = {0};
    if (!parse_entry_line(line, &parsed)) continue;
    if (db->count >= capacity) {
      UnadiEntry *grown = NULL;
      capacity *= 2;
      grown = realloc(db->entries, (size_t)capacity * sizeof(UnadiEntry));
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
  return 0;
}

/* Free Uṇādi database allocations. */
void unadi_db_free(UnadiDB *db) {
  if (!db) return;
  free(db->entries);
  db->entries = NULL;
  db->count = 0;
}

/* Find the first entry matching root and optional suffix. */
const UnadiEntry *unadi_lookup(const UnadiDB *db,
                               const char *root_slp1,
                               const char *suffix_slp1) {
  int i;
  if (!db || !root_slp1) return NULL;
  for (i = 0; i < db->count; i++) {
    const UnadiEntry *e = &db->entries[i];
    if (strcmp(e->root_slp1, root_slp1) != 0) continue;
    if (!suffix_slp1 || strcmp(e->suffix_slp1, suffix_slp1) == 0) return e;
  }
  return NULL;
}

/* Check whether a final form exists in the Uṇādi corpus. */
bool unadi_is_attested(const UnadiDB *db, const char *form_slp1) {
  int i;
  if (!db || !form_slp1) return false;
  for (i = 0; i < db->count; i++) {
    if (strcmp(db->entries[i].form_slp1, form_slp1) == 0) return true;
  }
  return false;
}

/* Convert a lookup hit into ASH_Form including provenance trace. */
ASH_Form unadi_form(const UnadiDB *db, const char *root_slp1,
                    const char *suffix_slp1) {
  ASH_Form f = {0};
  const UnadiEntry *entry = unadi_lookup(db, root_slp1, suffix_slp1);
  if (!entry) {
    f.valid = false;
    snprintf(f.error, sizeof(f.error), "unadi form not found");
    return f;
  }
  f.valid = true;
  strncpy(f.slp1, entry->form_slp1, sizeof(f.slp1) - 1);
  {
    char *iast = enc_slp1_to_iast(f.slp1);
    if (iast) {
      strncpy(f.iast, iast, sizeof(f.iast) - 1);
      free(iast);
    }
  }
  {
    char *deva = enc_slp1_to_devanagari(f.slp1);
    if (deva) {
      strncpy(f.devanagari, deva, sizeof(f.devanagari) - 1);
      free(deva);
    }
  }
  f.step_count = 1;
  f.steps = calloc(1, sizeof(ASH_PrakriyaStep));
  if (!f.steps) {
    f.valid = false;
    snprintf(f.error, sizeof(f.error), "oom");
    return f;
  }
  f.steps[0].sutra_id = 0;
  snprintf(f.steps[0].note, sizeof(f.steps[0].note), "unadi:%s", entry->sutra_ref);
  return f;
}
