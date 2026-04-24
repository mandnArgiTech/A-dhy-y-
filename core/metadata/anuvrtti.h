/* anuvrtti.h — stub; Story 2.5 */
#ifndef ANUVRTTI_H
#include <stddef.h>
#define ANUVRTTI_H
#include <stdint.h>
typedef struct { char term_slp1[64]; uint32_t source_sutra_id; } AnuvrttiBTerm;
typedef struct { uint32_t sutra_id; AnuvrttiBTerm *terms; int term_count; } AnuvrttiEntry;
typedef struct { AnuvrttiEntry *entries; int count; } AnuvrttiDB;
int  anuvrtti_load(AnuvrttiDB *db, const char *tsv_path);
void anuvrtti_db_free(AnuvrttiDB *db);
const AnuvrttiEntry *anuvrtti_get(const AnuvrttiDB *db, uint32_t sutra_id);
int  anuvrtti_resolve(const AnuvrttiDB *db, uint32_t sutra_id, char *out, size_t len);
#endif
