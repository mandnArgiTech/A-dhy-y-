/**
 * demo.c — libAshtadhyayi demonstration program
 *
 * Build: cmake -B build && cmake --build build
 * Run:   ./build/ash_demo
 */

#include "ashtadhyayi.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_separator(void) {
  printf("─────────────────────────────────────────────\n");
}

static void demo_tinanta(ASH_DB *db) {
  printf("\n=== Tiṅanta (Verbal Derivation) ===\n");

  struct {
    const char *root;
    int gana;
    ASH_Lakara lakara;
    ASH_Purusha p;
    ASH_Vacana v;
    ASH_Pada pd;
    const char *desc;
  } cases[] = {
    { "BU",  1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI, "bhū → bhavati" },
    { "gam", 1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI, "gam → gacchati" },
    { "cur", 10, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI, "cur → corayati" },
  };

  for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
    ASH_Form f = ash_tinanta(db, cases[i].root, cases[i].gana,
                               cases[i].lakara, cases[i].p,
                               cases[i].v, cases[i].pd);
    printf("\n%s\n", cases[i].desc);
    if (f.valid) {
      printf("  SLP1:       %s\n", f.slp1);
      printf("  IAST:       %s\n", f.iast);
      printf("  Devanāgarī: %s\n", f.devanagari);
      if (f.step_count > 0) {
        printf("  Steps:      %d rules applied\n", f.step_count);
      }
    } else {
      printf("  [not yet implemented: %s]\n", f.error);
    }
    ash_form_free(&f);
  }
}

static void demo_subanta(ASH_DB *db) {
  printf("\n=== Subanta (Nominal Declension) ===\n");

  struct {
    const char *stem;
    ASH_Linga linga;
    ASH_Vibhakti vib;
    ASH_Vacana vac;
    const char *desc;
  } cases[] = {
    { "rAma", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, "rāma (nom sg) → rāmaḥ" },
    { "rAma", ASH_PUMS, ASH_TRITIYA_VIB,  ASH_EKAVACANA, "rāma (inst sg) → rāmeṇa" },
    { "rAma", ASH_PUMS, ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "rāma (gen pl) → rāmāṇām" },
  };

  for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
    ASH_Form f = ash_subanta(db, cases[i].stem, cases[i].linga,
                               cases[i].vib, cases[i].vac);
    printf("\n%s\n", cases[i].desc);
    if (f.valid) {
      printf("  IAST: %s\n", f.iast);
    } else {
      printf("  [not yet implemented]\n");
    }
    ash_form_free(&f);
  }
}

static void demo_sandhi(ASH_DB *db) {
  printf("\n=== Sandhi ===\n");
  (void)db;

  struct {
    const char *a, *b;
    const char *desc;
  } cases[] = {
    { "rAma",  "asti",  "rāma + asti → rāmāsti" },
    { "ca",    "iti",   "ca + iti → ceti" },
    { "tam",   "ca",    "tam + ca → taṃca" },
  };

  for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
    char out[256] = {0};
    bool ok = ash_sandhi_apply(cases[i].a, cases[i].b,
                                out, sizeof(out), ASH_ENC_IAST);
    printf("\n%s\n", cases[i].desc);
    if (ok) printf("  IAST: %s\n", out);
    else     printf("  [sandhi not yet implemented for this pair]\n");
  }
}

static void demo_compound(ASH_DB *db) {
  printf("\n=== Samāsa (Compound Formation) ===\n");

  ASH_Form f = ash_samasa(db, "rAja", "puruzwa",
                            ASH_SAMASA_TATPURUSHA, ASH_PUMS);
  printf("\nrāja + puruṣa (tatpuruṣa)\n");
  if (f.valid) printf("  IAST: %s\n", f.iast);
  else          printf("  [not yet implemented]\n");
  ash_form_free(&f);
}

static void usage(const char *prog) {
  fprintf(stderr,
    "Usage: %s [command] [args...]\n"
    "  (no args)           — run demo\n"
    "  tinanta ROOT GANA LAKARA PURUSHA VACANA PADA\n"
    "  subanta  STEM LINGA VIBHAKTI VACANA\n",
    prog);
}

int main(int argc, char *argv[]) {
  printf("libAshtadhyayi %s\n", ash_version());
  print_separator();

  ASH_DB *db = ash_db_load("data/");
  if (!db) {
    fprintf(stderr, "ERROR: Failed to load data/. Run ingestion first:\n");
    fprintf(stderr, "  python3 tools/ingest_source.py\n");
    fprintf(stderr, "  python3 tools/ingest_dhatupatha.py\n");
    fprintf(stderr, "  python3 tools/ingest_ganapatha.py\n");
    return 1;
  }

  if (argc == 1) {
    /* Interactive demo */
    demo_tinanta(db);
    demo_subanta(db);
    demo_sandhi(db);
    demo_compound(db);
  } else if (argc >= 8 && strcmp(argv[1], "tinanta") == 0) {
    /* CLI: tinanta ROOT GANA LAKARA PURUSHA VACANA PADA */
    /* (mapping from string to enum left as exercise for Story 6.4) */
    printf("[CLI tinanta mode — implement enum mapping in Story 6.4]\n");
  } else if (argc >= 5 && strcmp(argv[1], "subanta") == 0) {
    printf("[CLI subanta mode — implement enum mapping in Story 6.4]\n");
  } else {
    usage(argv[0]);
    ash_db_free(db);
    return 1;
  }

  print_separator();
  ash_db_free(db);
  return 0;
}
