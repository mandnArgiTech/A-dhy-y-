/* reduplication.h — abhyāsa (root-reduplication) helpers shared by
   LIT (perfect), gaṇa-3 (juhotyādi), and the desiderative/intensive
   future stories. Implements 6.1.8/6.1.10 + 7.4.59–66. */
#ifndef REDUPLICATION_H
#define REDUPLICATION_H

#include <stddef.h>
#include <stdbool.h>

/* Produce the abhyāsa-prefixed form of a dhātu's first syllable.
   `clean_root` is the post-anubandha-strip SLP1 root (e.g. "BU",
   "kf", "gam"). `out` receives the abhyāsa + root sequence; caller
   must size `out_len >= strlen(clean_root)+4`.
   Returns true on success. */
bool reduplicate(const char *clean_root, char *out, size_t out_len);

#endif
