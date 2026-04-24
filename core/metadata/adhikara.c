/* adhikara.c — Story 2.4 implementation
 *
 * An adhikāra-sūtra sets a scope that applies to all following rules until
 * its scope is overridden. Example: 6.4.1 aṅgasya governs all rules in
 * Aṣṭādhyāyī 6.4-7.4 that operate on the aṅga (stem).
 *
 * Our approach:
 *   1. Scan the loaded SutraDB for sūtras of type ADHIKARA
 *   2. For each non-adhikāra sūtra, record the most recent adhikāra
 *      that still governs it (stored in Sutra.adhikara_parent)
 *
 * Simplified model: adhikāra scope = all sūtras from adhikāra's global_id
 * up to the next adhikāra at the same or higher pāda level.
 */

#include "adhikara.h"
#include "ashtadhyayi.h"
#include <string.h>

int adhikara_build(SutraDB *db, const char *tsv) {
  (void)tsv;  /* No extra TSV — info already in sutras.tsv via 'sutra_type' */
  if (!db || !db->sutras) return 0;

  /* First pass: walk sūtras, tracking the most recent adhikāra */
  uint32_t current_adhikara = 0;
  int built = 0;

  for (uint32_t i = 0; i < db->count; i++) {
    Sutra *s = &db->sutras[i];
    if (s->type == ASH_SUTRA_ADHIKARA) {
      /* This sūtra IS an adhikāra. It governs itself and following rules. */
      current_adhikara = s->global_id;
      s->adhikara_parent = 0;  /* adhikāras have no parent (self-scoped) */
    } else {
      /* Non-adhikāra inherits the current scope */
      s->adhikara_parent = current_adhikara;
      if (current_adhikara > 0) built++;
    }
  }
  return built;
}

bool adhikara_governs(const SutraDB *db, uint32_t adhikara_id, uint32_t child_id) {
  if (!db) return false;
  const Sutra *child = sutra_get_by_id(db, child_id);
  if (!child) return false;
  return child->adhikara_parent == adhikara_id;
}

uint32_t adhikara_parent_of(const SutraDB *db, uint32_t sutra_id) {
  if (!db) return 0;
  const Sutra *s = sutra_get_by_id(db, sutra_id);
  return s ? s->adhikara_parent : 0;
}

int adhikara_parents_of(const SutraDB *db, uint32_t sutra_id,
                        uint32_t *out, int max) {
  if (!db || !out || max <= 0) return 0;
  int n = 0;
  uint32_t cur = adhikara_parent_of(db, sutra_id);
  while (cur > 0 && n < max) {
    out[n++] = cur;
    /* Walk up: adhikāras can nest (e.g. aṅgasya within dhātoḥ) */
    cur = adhikara_parent_of(db, cur);
  }
  return n;
}
