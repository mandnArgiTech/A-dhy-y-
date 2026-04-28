/* dhatupatha.h — DhAtupATha TSV loader helpers */
#ifndef DHATUPATHA_H
#define DHATUPATHA_H

#include "ashtadhyayi.h"
#include <stdbool.h>

/* One dhAtupATha row projected into C fields used by pipeline code. */
typedef struct {
  uint32_t global_id;
  int gana;
  int serial_in_gana;
  char upadesa_slp1[64];
  char meaning_en[128];
  char pada_flag;
  char settva[4];
} DhatupathaEntry;

/* Loaded dhAtupATha corpus. */
typedef struct {
  DhatupathaEntry *entries;
  int count;
} DhatupathaDB;

/* Load dhAtupATha rows from a TSV file path. */
int dhatupatha_db_load(DhatupathaDB *db, const char *tsv_path);

/* Free memory allocated for the loaded dhAtupATha rows. */
void dhatupatha_db_free(DhatupathaDB *db);

/* Find a dhAtu by cleaned SLP1 with optional gaNa hint (0 = any). */
const DhatupathaEntry *dhatupatha_find(const DhatupathaDB *db,
                                       const char *clean_slp1, int gana_hint);

/* Return true when the database contains a matching dhAtu. */
bool dhatupatha_contains(const DhatupathaDB *db, const char *clean_slp1);

#endif
