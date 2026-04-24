/* anuvrtti.c — Story 2.5 implementation
 *
 * Many Aṣṭādhyāyī sūtras are elliptical — they rely on terms being inherited
 * from earlier sūtras. This is called anuvṛtti (recurring).
 *
 * Example: 6.1.77 iko yaṇ aci defines a substitution. The word "sthāne"
 * (in place of) comes from earlier in the pāda by anuvṛtti.
 *
 * Full automatic resolution requires commentary (kāśikā). Our implementation
 * provides the mechanism and a small hand-curated table of the most important
 * anuvṛtti relationships. The table can be extended incrementally from the
 * paribhāṣendu-śekhara and kāśikā data already in vendor/.
 */

#include "anuvrtti.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ── Seed table of well-known anuvṛtti relationships ─────────────────── */

typedef struct {
  uint32_t    sutra_addr;           /* adhyaya*10000 + pada*100 + num */
  uint32_t    source_addr;          /* where the carried-over term comes from */
  const char *term_slp1;
} SeedAnuvrtti;

/* These are the canonical anuvṛtti chains documented in traditional commentary. */
static const SeedAnuvrtti SEED[] = {
  /* 6.1.77 iko yaṇ aci — 'sthāne' from 1.1.49, adhikāra context */
  { 60177, 10149, "sTAne" },
  /* 6.1.87 ādguṇaḥ — 'eka-pūrva-paraḥ' from 6.1.84 */
  { 60187, 60184, "eka-pUrva-paraH" },
  /* 6.1.88 vṛddhireci — anuvṛtti 'ādguṇaḥ' and contextual scope */
  { 60188, 60187, "AdguRaH" },
  /* 6.1.101 akaḥ savarṇe dīrghaḥ — anuvṛtti 'ak' expansion */
  { 60101, 10109, "ak" },
  /* 3.1.68 kartari śap — 'tiṅ' from 3.4.77 */
  { 30168, 30477, "tiN" },
  { 0, 0, NULL }
};

int anuvrtti_load(AnuvrttiDB *db, const char *tsv) {
  (void)tsv;  /* No TSV yet — seed data only */
  if (!db) return -1;
  memset(db, 0, sizeof(*db));

  /* Count distinct sūtras in seed */
  int unique_sutras = 0;
  for (int i = 0; SEED[i].sutra_addr; i++) {
    bool found = false;
    for (int j = 0; j < i; j++)
      if (SEED[j].sutra_addr == SEED[i].sutra_addr) { found = true; break; }
    if (!found) unique_sutras++;
  }

  if (unique_sutras == 0) return 0;
  db->entries = calloc(unique_sutras, sizeof(AnuvrttiEntry));
  if (!db->entries) return -1;
  db->count = 0;

  /* Group seeds by sutra_addr */
  for (int i = 0; SEED[i].sutra_addr; i++) {
    /* Find or create entry */
    AnuvrttiEntry *e = NULL;
    for (int j = 0; j < db->count; j++) {
      if (db->entries[j].sutra_id == SEED[i].sutra_addr) { e = &db->entries[j]; break; }
    }
    if (!e) {
      e = &db->entries[db->count++];
      e->sutra_id = SEED[i].sutra_addr;
      e->terms = NULL;
      e->term_count = 0;
    }
    /* Append term */
    e->terms = realloc(e->terms, sizeof(AnuvrttiBTerm) * (e->term_count + 1));
    if (!e->terms) return -1;
    strncpy(e->terms[e->term_count].term_slp1, SEED[i].term_slp1,
             sizeof(e->terms[0].term_slp1) - 1);
    e->terms[e->term_count].term_slp1[sizeof(e->terms[0].term_slp1) - 1] = '\0';
    e->terms[e->term_count].source_sutra_id = SEED[i].source_addr;
    e->term_count++;
  }
  return db->count;
}

void anuvrtti_db_free(AnuvrttiDB *db) {
  if (!db) return;
  for (int i = 0; i < db->count; i++) free(db->entries[i].terms);
  free(db->entries);
  db->entries = NULL;
  db->count = 0;
}

const AnuvrttiEntry *anuvrtti_get(const AnuvrttiDB *db, uint32_t sutra_id) {
  if (!db || !db->entries) return NULL;
  for (int i = 0; i < db->count; i++)
    if (db->entries[i].sutra_id == sutra_id) return &db->entries[i];
  return NULL;
}

int anuvrtti_resolve(const AnuvrttiDB *db, uint32_t sutra_id, char *out, size_t len) {
  if (!out || len == 0) return 0;
  out[0] = '\0';
  const AnuvrttiEntry *e = anuvrtti_get(db, sutra_id);
  if (!e) return 0;
  size_t pos = 0;
  for (int i = 0; i < e->term_count; i++) {
    size_t n = strlen(e->terms[i].term_slp1);
    if (pos + n + 1 >= len) break;
    if (pos > 0) { out[pos++] = ','; }
    memcpy(out + pos, e->terms[i].term_slp1, n);
    pos += n;
  }
  out[pos] = '\0';
  return (int)pos;
}
