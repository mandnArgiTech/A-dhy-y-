/* unadi.c — Story 5.5 baseline implementation */
#include "unadi.h"
#include "encoding.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Fallback lexical core for environments without full unadipatha.tsv yet. */
static const UnadiEntry FALLBACK_ENTRIES[] = {
  {1, "vA",   "yu", "vAyu",  "wind",    "unadi_1.2"},
  {2, "jan",  "u",  "jAnu",  "knee",    "unadi_1.5"},
  {3, "banD", "u",  "banDu", "kinsman", "unadi_1.8"},
  {4, "man",  "as", "manas", "mind",    "unadi_2.1"},
  {5, "tap",  "as", "tapas", "austerity", "unadi_2.2"},
  {6, "smf",  "ti", "smfti", "memory",  "unadi_2.3"},
};

static void unadi_copy_entry(UnadiEntry *dst, const UnadiEntry *src) {
  if (!dst || !src) return;
  memcpy(dst, src, sizeof(*dst));
}

int unadi_db_load(UnadiDB *db, const char *tsv_path) {
  (void)tsv_path;
  if (!db) return -1;
  memset(db, 0, sizeof(*db));
  db->entries = calloc((int)(sizeof(FALLBACK_ENTRIES) / sizeof(FALLBACK_ENTRIES[0])),
                       sizeof(UnadiEntry));
  if (!db->entries) return -1;
  db->count = (int)(sizeof(FALLBACK_ENTRIES) / sizeof(FALLBACK_ENTRIES[0]));
  for (int i = 0; i < db->count; i++) {
    unadi_copy_entry(&db->entries[i], &FALLBACK_ENTRIES[i]);
  }
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
  f.steps[0].sutra_id = 0;
  strncpy(f.steps[0].before_slp1, root_slp1 ? root_slp1 : "",
          sizeof(f.steps[0].before_slp1) - 1);
  strncpy(f.steps[0].after_slp1, f.slp1, sizeof(f.steps[0].after_slp1) - 1);
  snprintf(f.steps[0].note, sizeof(f.steps[0].note),
           "uNAdi %s", entry->sutra_ref);

  return f;
}
