/* unadi.h — Uṇādi lexical lookup helpers */
#ifndef UNADI_H
#define UNADI_H

#include "ashtadhyayi.h"
#include <stdbool.h>
#include <stdint.h>

/* One Uṇādi lexical derivation entry. */
typedef struct {
  uint32_t unadi_id;
  char root_slp1[32];
  char suffix_slp1[16];
  char form_slp1[64];
  char meaning_en[128];
  char sutra_ref[32];
} UnadiEntry;

/* In-memory Uṇādi database loaded from TSV. */
typedef struct {
  UnadiEntry *entries;
  int count;
} UnadiDB;

/* Load Uṇādi entries from a TSV path into `db`. */
int unadi_db_load(UnadiDB *db, const char *tsv_path);

/* Free all resources held by a loaded Uṇādi DB. */
void unadi_db_free(UnadiDB *db);

/* Lookup an attested Uṇādi entry by root and optional suffix. */
const UnadiEntry *unadi_lookup(const UnadiDB *db,
                               const char *root_slp1,
                               const char *suffix_slp1);

/* Return whether a final SLP1 form is attested in the Uṇādi corpus. */
bool unadi_is_attested(const UnadiDB *db, const char *form_slp1);

/* Build an ASH_Form from an attested Uṇādi lookup. */
ASH_Form unadi_form(const UnadiDB *db, const char *root_slp1,
                    const char *suffix_slp1);

#endif
