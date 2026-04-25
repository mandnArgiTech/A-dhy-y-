#ifndef TEST_PATHS_H
#define TEST_PATHS_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Resolve a readable data directory for both direct and ctest execution. */
static inline const char *test_resolve_data_dir(char *out, size_t out_len) {
  static const char *const CANDIDATES[] = {
      "data",
      "../data",
      "../../data",
      "/workspace/data",
  };
  size_t i;

  if (!out || out_len == 0) return NULL;
  for (i = 0; i < sizeof(CANDIDATES) / sizeof(CANDIDATES[0]); i++) {
    if (access(CANDIDATES[i], R_OK) == 0) {
      snprintf(out, out_len, "%s", CANDIDATES[i]);
      return out;
    }
  }
  snprintf(out, out_len, "data");
  return out;
}

/* Resolve a readable data file path for both direct and ctest execution. */
static inline const char *test_resolve_data_file(const char *name, char *out,
                                                 size_t out_len) {
  static const char *const PREFIXES[] = {
      "data",
      "../data",
      "../../data",
      "/workspace/data",
  };
  size_t i;

  if (!name || !out || out_len == 0) return NULL;
  for (i = 0; i < sizeof(PREFIXES) / sizeof(PREFIXES[0]); i++) {
    snprintf(out, out_len, "%s/%s", PREFIXES[i], name);
    if (access(out, R_OK) == 0) return out;
  }
  snprintf(out, out_len, "data/%s", name);
  return out;
}

#endif
