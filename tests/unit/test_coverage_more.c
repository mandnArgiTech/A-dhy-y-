#include "unity.h"
#include "ashtadhyayi.h"
#include "encoding.h"
#include "pipeline.h"
#include "sandhi_visarga.h"
#include "krit/krit_primary.h"
#include "subanta/aaiu_stems.h"
#include "subanta/consonant_stems.h"
#include "unadipatha/unadi.h"
#include "ganapatha/ganapatha.h"
#include "varna.h"
#include "test_paths.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void setUp(void) {}
void tearDown(void) {}

static const char *data_dir(void) {
  static char dir[256];
  return test_resolve_data_dir(dir, sizeof(dir));
}

static void write_file(const char *path, const char *content) {
  FILE *f = fopen(path, "w");
  TEST_ASSERT_NOT_NULL(f);
  TEST_ASSERT_EQUAL_INT((int)strlen(content), (int)fwrite(content, 1, strlen(content), f));
  fclose(f);
}

void test_pipeline_api_and_dispatch_branches(void) {
  ASH_DB *db = ash_db_load(data_dir());
  ASH_Form f;
  FILE *tmp;
  char line[256] = {0};
  char out[64] = {0};
  Pipeline p = {0};

  TEST_ASSERT_NOT_NULL(db);

  f = ash_tinanta(db, "BU", 1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_TRUE(f.valid);
  tmp = tmpfile();
  TEST_ASSERT_NOT_NULL(tmp);
  ash_form_print_prakriya(&f, tmp);
  rewind(tmp);
  TEST_ASSERT_NOT_NULL(fgets(line, sizeof(line), tmp));
  TEST_ASSERT_NOT_NULL(strstr(line, "Derivation"));
  fclose(tmp);
  ash_form_free(&f);

  TEST_ASSERT_TRUE(ash_sandhi_apply("ca", "iti", out, sizeof(out), ASH_ENC_SLP1));
  TEST_ASSERT_EQUAL_STRING("ceti", out);
  TEST_ASSERT_EQUAL_INT(0, ash_sandhi_split("ceti", NULL, 0));

  f = ash_krit(db, "gam", 1, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = ash_samasa(db, "rAja", "puruza", ASH_SAMASA_TATPURUSHA, ASH_PUMS);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = pipeline_subanta(&p, "rAmA", ASH_STRI, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = pipeline_subanta(&p, "kvi", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = pipeline_subanta(&p, "mDu", ASH_NAPUMSAKA, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = pipeline_subanta(&p, "rAjn", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = pipeline_subanta(&p, "mns", ASH_NAPUMSAKA, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = pipeline_subanta(&p, "pitf", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  {
    DhatuEntry entries[2];
    memset(entries, 0, sizeof(entries));
    strncpy(entries[0].clean_slp1, "BU", sizeof(entries[0].clean_slp1) - 1);
    entries[0].gana = 1;
    strncpy(entries[1].clean_slp1, "gam", sizeof(entries[1].clean_slp1) - 1);
    entries[1].gana = 1;
    p.dhatus = entries;
    p.dhatu_count = 2;
    TEST_ASSERT_NULL(pipeline_find_dhatu(NULL, "BU", 0));
    TEST_ASSERT_NULL(pipeline_find_dhatu(&p, NULL, 0));
    TEST_ASSERT_NOT_NULL(pipeline_find_dhatu(&p, "BU", 0));
    TEST_ASSERT_NULL(pipeline_find_dhatu(&p, "BU", 4));
  }

  ash_db_free(db);
}

void test_unadi_growth_and_error_loader_branches(void) {
  char empty_path[] = "/tmp/unadi-empty-XXXXXX";
  char many_path[] = "/tmp/unadi-many-XXXXXX";
  char line[256];
  int fd_empty = mkstemp(empty_path);
  int fd_many = mkstemp(many_path);
  UnadiDB db = {0};
  FILE *f;

  TEST_ASSERT_TRUE(fd_empty >= 0);
  TEST_ASSERT_TRUE(fd_many >= 0);
  close(fd_empty);
  close(fd_many);

  {
    char empty_tsv[128];
    snprintf(empty_tsv, sizeof(empty_tsv), "%s.tsv", empty_path);
    rename(empty_path, empty_tsv);
    strncpy(empty_path, empty_tsv, sizeof(empty_path) - 1);
    empty_path[sizeof(empty_path) - 1] = '\0';
  }
  {
    char many_tsv[128];
    snprintf(many_tsv, sizeof(many_tsv), "%s.tsv", many_path);
    rename(many_path, many_tsv);
    strncpy(many_path, many_tsv, sizeof(many_path) - 1);
    many_path[sizeof(many_path) - 1] = '\0';
  }

  write_file(empty_path, "");
  TEST_ASSERT_EQUAL_INT(-1, unadi_db_load(&db, empty_path));

  f = fopen(many_path, "w");
  TEST_ASSERT_NOT_NULL(f);
  fprintf(f, "unadi_id\troot_slp1\tsuffix_slp1\tform_slp1\tmeaning_en\tsutra_ref\n");
  for (int i = 0; i < 40; i++) {
    snprintf(line, sizeof(line), "%d\tr%d\ts\tf%d\tm\tu.%d\n", i + 1, i, i, i + 1);
    fputs(line, f);
  }
  fclose(f);
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, many_path));
  TEST_ASSERT_EQUAL_INT(40, db.count);
  unadi_db_free(&db);

  unlink(empty_path);
  unlink(many_path);
}

void test_encoding_and_misc_remaining_branches(void) {
  char *r;
  char out[64];
  PrakriyaCtx ctx = {0};

  r = enc_slp1_to_devanagari("ka?");
  TEST_ASSERT_NOT_NULL(r);
  free(r);

  r = enc_hk_to_slp1("RR");
  TEST_ASSERT_EQUAL_STRING("F", r);
  free(r);

  r = ash_encode("rāma", ASH_ENC_IAST, ASH_ENC_SLP1);
  TEST_ASSERT_EQUAL_STRING("rAma", r);
  free(r);

  r = ash_encode("राम", ASH_ENC_DEVANAGARI, ASH_ENC_SLP1);
  TEST_ASSERT_NOT_NULL(r);
  free(r);

  r = encoding_convert("rAma", ASH_ENC_SLP1, ASH_ENC_HK);
  TEST_ASSERT_EQUAL_STRING("rAma", r);
  free(r);

  TEST_ASSERT_EQUAL_CHAR('f', varna_shorten('F'));
  TEST_ASSERT_EQUAL_CHAR('x', varna_shorten('X'));
  TEST_ASSERT_EQUAL_CHAR('X', varna_lengthen('x'));
  TEST_ASSERT_EQUAL_CHAR('A', varna_vrddhi('x'));

  TEST_ASSERT_TRUE(aa_stem_fem_derive("rAmA", ASH_DVITIYA_VIB, ASH_DVIVACANA, &ctx));
  TEST_ASSERT_TRUE(aa_stem_fem_derive("rAmA", (ASH_Vibhakti)99, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_TRUE(u_stem_derive("guru", ASH_NAPUMSAKA, ASH_SAPTAMI_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("guru", ctx.terms[0].value);

  TEST_ASSERT_TRUE(n_stem_derive("rAjn", ASH_PUMS, ASH_TRITIYA_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("rAjYA", ctx.terms[0].value);
  TEST_ASSERT_TRUE(n_stem_derive("rAjn", ASH_PUMS, ASH_SAPTAMI_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("rAjn", ctx.terms[0].value);
  TEST_ASSERT_TRUE(consonant_stem_derive("rAjn", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, out, sizeof(out)));
  TEST_ASSERT_TRUE(consonant_stem_derive("pitf", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, out, sizeof(out)));

  {
    FILE *tmp = tmpfile();
    TEST_ASSERT_NOT_NULL(tmp);
    term_print(&ctx.terms[0], tmp);
    fclose(tmp);
  }

  {
    SandhiResult sv = sandhi_visarga_apply('A', 'k');
    TEST_ASSERT_FALSE(sv.changed);
    TEST_ASSERT_EQUAL_STRING("Hk", sv.result_slp1);
  }

  {
    ASH_Form f = krit_derive("pat", 1, ASH_KRIT_TAVYA);
    TEST_ASSERT_TRUE(f.valid);
    ash_form_free(&f);
    f = krit_derive("pat", 1, (ASH_KritType)999);
    TEST_ASSERT_FALSE(f.valid);
    ash_form_free(&f);
  }

  {
    GanapathaDB db = {0};
    static char gp_path[256];
    TEST_ASSERT_EQUAL_INT(0, ganapatha_db_load(&db,
                                               test_resolve_data_file("ganapatha.tsv",
                                                                      gp_path, sizeof(gp_path))));
    ganapatha_db_free(&db);
  }
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_pipeline_api_and_dispatch_branches);
  RUN_TEST(test_unadi_growth_and_error_loader_branches);
  RUN_TEST(test_encoding_and_misc_remaining_branches);
  return UNITY_END();
}
