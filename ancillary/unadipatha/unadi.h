/* unadi.h — uNadi lexical lookup; Story 5.5 baseline */
#ifndef UNADI_H
#define UNADI_H

#include "ashtadhyayi.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint32_t unadi_id;
  char     root_slp1[32];
  char     suffix_slp1[16];
  char     form_slp1[64];
  char     meaning_en[128];
  char     sutra_ref[32];
} UnadiEntry;

typedef struct {
  UnadiEntry *entries;
  int         count;
} UnadiDB;

int  unadi_db_load(UnadiDB *db, const char *tsv_path);
void unadi_db_free(UnadiDB *db);
const UnadiEntry *unadi_lookup(const UnadiDB *db,
                               const char *root_slp1,
                               const char *suffix_slp1);
bool unadi_is_attested(const UnadiDB *db, const char *form_slp1);
ASH_Form unadi_form(const UnadiDB *db, const char *root_slp1,
                    const char *suffix_slp1);

#endif
