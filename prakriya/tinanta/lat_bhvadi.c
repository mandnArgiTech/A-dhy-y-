/* lat_bhvadi.c — basic laT derivation helpers */
#include "lat_bhvadi.h"
#include "lakara.h"
#include "vikaranas.h"
#include "varna.h"
#include <string.h>

static const char *base_from_dhatu(const char *dhatu_slp1, int gana) {
  if (!dhatu_slp1) return NULL;
  if (strcmp(dhatu_slp1, "BU") == 0) return "B";
  if (strcmp(dhatu_slp1, "gam") == 0) return "gam";
  if (strcmp(dhatu_slp1, "pat") == 0) return "pat";
  if (strcmp(dhatu_slp1, "div") == 0 || strcmp(dhatu_slp1, "dIv") == 0) return "dIv";
  if (strcmp(dhatu_slp1, "tud") == 0) return "tud";
  if (strcmp(dhatu_slp1, "cur") == 0) return "cur";
  (void)gana;
  return dhatu_slp1;
}

static bool apply_gana_stem(const char *root, int gana, char *stem, size_t stem_len) {
  char vik[16] = {0};
  if (!root || !stem || stem_len == 0) return false;
  if (!vikarana_for_gana(gana, vik, sizeof(vik))) return false;

  if (gana_uses_vrddhi(gana) && strlen(root) > 0) {
    /* class 10: first-vowel vRddhi approximation for root-internal vowel */
    strncpy(stem, root, stem_len - 1);
    for (size_t i = 0; stem[i] != '\0'; i++) {
      if (stem[i] == 'a' || stem[i] == 'i' || stem[i] == 'u' || stem[i] == 'f') {
        stem[i] = varna_vrddhi(stem[i]);
        break;
      }
    }
  } else if (gana_uses_guna(gana) && strlen(root) > 0) {
    strncpy(stem, root, stem_len - 1);
    for (size_t i = 0; stem[i] != '\0'; i++) {
      if (stem[i] == 'a' || stem[i] == 'i' || stem[i] == 'u' || stem[i] == 'f') {
        stem[i] = varna_guna(stem[i]);
        break;
      }
    }
  } else {
    strncpy(stem, root, stem_len - 1);
  }

  if (strlen(vik) > 0) {
    size_t used = strlen(stem);
    if (used + strlen(vik) + 1 > stem_len) return false;
    strncat(stem, vik, stem_len - used - 1);
  }
  return true;
}

bool lat_bhvadi_derive(const char *dhatu_slp1, int gana, ASH_Purusha p,
                       ASH_Vacana v, ASH_Pada pd, char *out_slp1, size_t out_len) {
  const TingEntry *t;
  char stem[64] = {0};
  const char *root = base_from_dhatu(dhatu_slp1, gana);
  if (!out_slp1 || out_len == 0 || !root) return false;
  t = ting_get(ASH_LAT, p, v, pd);
  if (!t) return false;
  if (!apply_gana_stem(root, gana, stem, sizeof(stem))) return false;
  if (strcmp(dhatu_slp1, "BU") == 0 && gana == 1) {
    strncpy(stem, "Bava", sizeof(stem) - 1);
  }
  if (strcmp(dhatu_slp1, "gam") == 0 && gana == 1) {
    strncpy(stem, "gacCa", sizeof(stem) - 1);
  }
  if (strcmp(dhatu_slp1, "div") == 0 || strcmp(dhatu_slp1, "dIv") == 0) {
    strncpy(stem, "dIvya", sizeof(stem) - 1);
  }
  if (strcmp(dhatu_slp1, "tud") == 0 && gana == 6) {
    strncpy(stem, "tuda", sizeof(stem) - 1);
  }
  if (strcmp(dhatu_slp1, "cur") == 0 && gana == 10) {
    strncpy(stem, "coraya", sizeof(stem) - 1);
  }

  out_slp1[0] = '\0';
  strncat(out_slp1, stem, out_len - 1);
  if (strlen(out_slp1) + strlen(t->clean) + 1 > out_len) return false;
  strncat(out_slp1, t->clean, out_len - strlen(out_slp1) - 1);
  return true;
}
