/* unadi.h — Uṇādi-paṭha lookup helpers */
#ifndef UNADI_H
#define UNADI_H

#include "ashtadhyayi.h"
#include <stdbool.h>
#include <stdint.h>

/* One Uṇādi sūtra entry as ingested from data/unadipatha.tsv. */
typedef struct {
  uint32_t unadi_id;            /* upstream serial id, e.g. 1001 */
  char     sutra_deva[256];     /* Devanāgarī sūtra text */
  char     sutra_slp1[256];     /* SLP1 sūtra text */
  char     pratyay_deva[64];    /* Devanāgarī suffix introduced */
  char     pratyay_slp1[64];    /* SLP1 suffix introduced */
  char     sutra_ref[32];       /* optional cross-reference (e.g. "unadi_1.1") */
} UnadiEntry;

/* In-memory Uṇādi database loaded from TSV. */
typedef struct {
  UnadiEntry *entries;
  int         count;
} UnadiDB;

/* Load Uṇādi entries from a TSV path into `db`. */
int unadi_db_load(UnadiDB *db, const char *tsv_path);

/* Free all resources held by a loaded Uṇādi DB. */
void unadi_db_free(UnadiDB *db);

/* Lookup a sūtra by its upstream id. Returns NULL if not present. */
const UnadiEntry *unadi_lookup_by_id(const UnadiDB *db, uint32_t id);

/* Lookup the first sūtra whose pratyay matches `pratyay_slp1`. */
const UnadiEntry *unadi_lookup_by_pratyay(const UnadiDB *db,
                                          const char *pratyay_slp1);

/* Build an informational ASH_Form citing the sūtra that introduces a pratyay. */
ASH_Form unadi_cite_pratyay(const UnadiDB *db, const char *pratyay_slp1);

#endif
