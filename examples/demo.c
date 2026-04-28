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
      printf("  [error: %s]\n", f.error);
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
      printf("  [error: %s]\n", f.error);
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
    else     printf("  IAST: %s\n", out);
  }
}

static void demo_compound(ASH_DB *db) {
  printf("\n=== Samāsa (Compound Formation) ===\n");

  ASH_Form f = ash_samasa(db, "rAja", "puruzwa",
                            ASH_SAMASA_TATPURUSHA, ASH_PUMS);
  printf("\nrāja + puruṣa (tatpuruṣa)\n");
  if (f.valid) printf("  IAST: %s\n", f.iast);
  else          printf("  [error: %s]\n", f.error);
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

static ASH_Lakara parse_lakara(const char *s) {
  if (strcmp(s, "LAT") == 0) return ASH_LAT;
  if (strcmp(s, "LIT") == 0) return ASH_LIT;
  if (strcmp(s, "LUT") == 0) return ASH_LUT;
  if (strcmp(s, "LRT") == 0) return ASH_LRT;
  if (strcmp(s, "LOT") == 0) return ASH_LOT;
  if (strcmp(s, "LAN") == 0) return ASH_LAN;
  if (strcmp(s, "VIDHILIM") == 0) return ASH_VIDHILIM;
  if (strcmp(s, "ASHIRLIM") == 0) return ASH_ASHIRLIM;
  if (strcmp(s, "LUN") == 0) return ASH_LUN;
  if (strcmp(s, "LRN") == 0) return ASH_LRN;
  return ASH_LAKARA_COUNT;
}

static ASH_Purusha parse_purusha(const char *s) {
  if (strcmp(s, "PRATHAMA") == 0) return ASH_PRATHAMA;
  if (strcmp(s, "MADHYAMA") == 0) return ASH_MADHYAMA;
  if (strcmp(s, "UTTAMA") == 0) return ASH_UTTAMA;
  return (ASH_Purusha)-1;
}

static ASH_Vacana parse_vacana(const char *s) {
  if (strcmp(s, "EKAVACANA") == 0 || strcmp(s, "EKA") == 0) return ASH_EKAVACANA;
  if (strcmp(s, "DVIVACANA") == 0 || strcmp(s, "DVI") == 0) return ASH_DVIVACANA;
  if (strcmp(s, "BAHUVACANA") == 0 || strcmp(s, "BAHU") == 0) return ASH_BAHUVACANA;
  return (ASH_Vacana)-1;
}

static ASH_Pada parse_pada(const char *s) {
  if (strcmp(s, "PARASMAI") == 0 || strcmp(s, "P") == 0) return ASH_PARASMAI;
  if (strcmp(s, "ATMANE") == 0 || strcmp(s, "A") == 0) return ASH_ATMANE;
  return (ASH_Pada)-1;
}

static ASH_Linga parse_linga(const char *s) {
  if (strcmp(s, "PUMS") == 0) return ASH_PUMS;
  if (strcmp(s, "STRI") == 0) return ASH_STRI;
  if (strcmp(s, "NAPUMSAKA") == 0) return ASH_NAPUMSAKA;
  return (ASH_Linga)-1;
}

static ASH_Vibhakti parse_vibhakti(const char *s) {
  if (strcmp(s, "PRATHAMA") == 0) return ASH_PRATHAMA_VIB;
  if (strcmp(s, "DVITIYA") == 0) return ASH_DVITIYA_VIB;
  if (strcmp(s, "TRITIYA") == 0) return ASH_TRITIYA_VIB;
  if (strcmp(s, "CATURTHI") == 0) return ASH_CATURTHI_VIB;
  if (strcmp(s, "PANCAMI") == 0) return ASH_PANCAMI_VIB;
  if (strcmp(s, "SHASTHI") == 0) return ASH_SHASTHI_VIB;
  if (strcmp(s, "SAPTAMI") == 0) return ASH_SAPTAMI_VIB;
  if (strcmp(s, "SAMBODHANA") == 0) return ASH_SAMBODHANA_VIB;
  return (ASH_Vibhakti)-1;
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
    ASH_Lakara lakara = parse_lakara(argv[4]);
    ASH_Purusha purusha = parse_purusha(argv[5]);
    ASH_Vacana vacana = parse_vacana(argv[6]);
    ASH_Pada pada = parse_pada(argv[7]);
    ASH_Form f;
    if ((int)lakara < 0 || lakara >= ASH_LAKARA_COUNT ||
        (int)purusha < 0 || (int)purusha > ASH_UTTAMA ||
        (int)vacana < 0 || (int)vacana > ASH_BAHUVACANA ||
        (int)pada < 0 || (int)pada > ASH_ATMANE) {
      usage(argv[0]);
      ash_db_free(db);
      return 2;
    }
    f = ash_tinanta(db, argv[2], atoi(argv[3]), lakara, purusha, vacana, pada);
    if (f.valid) {
      printf("%s\n", f.slp1);
    } else {
      fprintf(stderr, "ERROR: %s\n", f.error);
      ash_form_free(&f);
      ash_db_free(db);
      return 3;
    }
    ash_form_free(&f);
  } else if (argc >= 6 && strcmp(argv[1], "subanta") == 0) {
    ASH_Linga linga = parse_linga(argv[3]);
    ASH_Vibhakti vib = parse_vibhakti(argv[4]);
    ASH_Vacana vacana = parse_vacana(argv[5]);
    ASH_Form f;
    if ((int)linga < 0 || (int)linga > ASH_NAPUMSAKA ||
        (int)vib < 0 || (int)vib > ASH_SAMBODHANA_VIB ||
        (int)vacana < 0 || (int)vacana > ASH_BAHUVACANA) {
      usage(argv[0]);
      ash_db_free(db);
      return 2;
    }
    f = ash_subanta(db, argv[2], linga, vib, vacana);
    if (f.valid) {
      printf("%s\n", f.slp1);
    } else {
      fprintf(stderr, "ERROR: %s\n", f.error);
      ash_form_free(&f);
      ash_db_free(db);
      return 3;
    }
    ash_form_free(&f);
  } else {
    usage(argv[0]);
    ash_db_free(db);
    return 1;
  }

  print_separator();
  ash_db_free(db);
  return 0;
}
