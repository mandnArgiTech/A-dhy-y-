/* accent.h — svara (accent) computation per Pāṇinian rules.
 * Phase ζ initial scaffold: defines the per-syllable accent constants
 * already declared in <ashtadhyayi.h> plus the engine entry points.
 *
 * Accent codes (one char per vowel):
 *   'U' = udātta (high)
 *   'A' = anudātta (low)
 *   'S' = svarita (falling)
 *   'P' = pracaya (Vedic non-cadential anudātta)
 */
#ifndef ASHTADHYAYI_CORE_ACCENT_H
#define ASHTADHYAYI_CORE_ACCENT_H

#include "ashtadhyayi.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the accent registry from a phit-sūtras data file.
 * Returns the number of phiṭ-sūtras loaded, or 0 on failure. */
int accent_load_phit(const char *path);

/* Compute the accent string for a given SLP1 form, treating it as a
 * subanta (nominal). Applies phiṭ-sūtra 1.1 ("phiṣaḥ ante udāttaḥ" —
 * last vowel is udātta) as the default and overrides with specific
 * sūtras when the stem matches a known exception.
 *
 * Returns true on success and writes a per-syllable accent string into
 * `out` (one char per vowel, terminated). On failure returns false.
 */
bool accent_compute_subanta(const char *form_slp1, ASH_AccentMode mode,
                            char *out, size_t out_len);

/* Compute accent for a tinanta (verbal) form. Per 6.1.158 anudāttaṃ
 * padam ekavarjam, a finite verb is fully anudātta (= no udātta) EXCEPT
 * when it is sentence-initial / negated / interrogative. We default to
 * the standard rule: all syllables anudātta. */
bool accent_compute_tinanta(const char *form_slp1, ASH_AccentMode mode,
                            char *out, size_t out_len);

/* Convenience: write 'A' for every vowel in `form_slp1`. */
bool accent_all_anudatta(const char *form_slp1, char *out, size_t out_len);

#ifdef __cplusplus
}
#endif

#endif
