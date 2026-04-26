/* ganapatha.h — gaNa and mAheSvara member loader API */
#ifndef GANAPATHA_H
#define GANAPATHA_H

#include <stdbool.h>

typedef struct {
  int  gana_id;
  char gana_name_slp1[64];
  char sutra_ref[32];
  int  serial;
  char member_slp1[64];
} GanapathaEntry;

typedef struct {
  GanapathaEntry *entries;
  int             count;
} GanapathaDB;

/* Loads gaNapATha TSV members from disk. */
int ganapatha_db_load(GanapathaDB *db, const char *tsv_path);

/* Releases all memory held by a gaNapATha database. */
void ganapatha_db_free(GanapathaDB *db);

/* Returns true if a member occurs in the given gaNa-name bucket. */
bool ganapatha_has_member(const GanapathaDB *db, const char *gana_name_slp1,
                          const char *member_slp1);

/* Returns the first member row for a gaNa name in SLP1. */
const GanapathaEntry *ganapatha_find_group(const GanapathaDB *db,
                                           const char *gana_name_slp1);

/* Returns true when the entry belongs to mAheSvara-sUtra rows. */
bool ganapatha_is_maheshvara(const GanapathaEntry *entry);

#endif
