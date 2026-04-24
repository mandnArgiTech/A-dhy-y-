#include "unity.h"
#include "pipeline.h"
#include "taddhita/taddhita.h"
#include "ashtadhyayi.h"
#include <string.h>

static ASH_DB *g_db = NULL;

void setUp(void) { g_db = ash_db_load("data/"); }

void tearDown(void) {
  if (g_db) {
    ash_db_free(g_db);
    g_db = NULL;
  }
}

void test_an_garga(void) {
  ASH_Form f = taddhita_derive(NULL, "garga", TD_AN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gArgya", f.slp1);
  ash_form_free(&f);
}

void test_ta_abstract(void) {
  ASH_Form f = taddhita_derive(NULL, "Sukla", TD_TA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("SuklatA", f.slp1);
  ash_form_free(&f);
}

void test_in_possession(void) {
  ASH_Form f = taddhita_derive(NULL, "Dana", TD_IN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("DanIn", f.slp1);
  ash_form_free(&f);
}

void test_tara_comparative(void) {
  ASH_Form f = taddhita_derive(NULL, "laghu", TD_TARA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("laGutara", f.slp1);
  ash_form_free(&f);
}

void test_vat_simile(void) {
  ASH_Form f = taddhita_derive(NULL, "siMha", TD_VAT);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("siMhavat", f.slp1);
  ash_form_free(&f);
}

void test_all_15_types_have_coverage(void) {
  struct {
    const char *base;
    TaddhitaType td;
    const char *expected;
  } rows[] = {
    {"garga",   TD_AN,   "gArgya"},
    {"vidyA",   TD_YA,   "vEdya"},
    {"Dana",    TD_IN,   "DanIn"},
    {"Dana",    TD_MAT,  "Danamat"},
    {"Sukla",   TD_TA,   "SuklatA"},
    {"Sukla",   TD_TVA,  "Suklatva"},
    {"putra",   TD_KA,   "putraka"},
    {"grAma",   TD_IKA,  "grAmIRa"},
    {"rAjan",   TD_IYA,  "rAjanIya"},
    {"Sreyas",  TD_TAMA, "Sreyastama"},
    {"laghu",   TD_TARA, "laGutara"},
    {"siMha",   TD_VAT,  "siMhavat"},
    {"suvarna", TD_MAYA, "suvarRamaya"},
    {"grIzma",  TD_AANA, "grIzmARa"},
    {"yajYa",   TD_VYA,  "yajYIya"},
  };

  for (size_t i = 0; i < sizeof(rows) / sizeof(rows[0]); i++) {
    ASH_Form f = taddhita_derive(NULL, rows[i].base, rows[i].td);
    TEST_ASSERT_TRUE_MESSAGE(f.valid, "all 15 taddhita rows should derive");
    TEST_ASSERT_EQUAL_STRING(rows[i].expected, f.slp1);
    TEST_ASSERT_TRUE(f.step_count == 1);
    TEST_ASSERT_TRUE(f.steps[0].sutra_id > 0);
    ash_form_free(&f);
  }
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_an_garga);
  RUN_TEST(test_ta_abstract);
  RUN_TEST(test_in_possession);
  RUN_TEST(test_tara_comparative);
  RUN_TEST(test_vat_simile);
  RUN_TEST(test_all_15_types_have_coverage);
  return UNITY_END();
}
