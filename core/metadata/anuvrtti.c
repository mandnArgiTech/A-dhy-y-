/* anuvrtti.c — stub; Story 2.5 */
#include "anuvrtti.h"
#include <string.h>
int anuvrtti_load(AnuvrttiDB *db, const char *tsv) { (void)tsv; if(db){db->entries=NULL;db->count=0;} return 0; }
void anuvrtti_db_free(AnuvrttiDB *db) { if(db){db->entries=NULL;db->count=0;} }
const AnuvrttiEntry *anuvrtti_get(const AnuvrttiDB *db, uint32_t id) {
  (void)id; if(!db) return NULL;
  for(int i=0;i<db->count;i++) if(db->entries[i].sutra_id==id) return &db->entries[i];
  return NULL;
}
int anuvrtti_resolve(const AnuvrttiDB *db, uint32_t id, char *out, size_t len) {
  (void)db;(void)id; if(out&&len) *out='\0'; return 0;
}
