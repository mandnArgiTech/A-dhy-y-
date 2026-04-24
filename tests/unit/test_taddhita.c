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
  TEST_ASSERT_EQUAL_STRING("Danin", f.slp1);
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

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_an_garga);
  RUN_TEST(test_ta_abstract);
  RUN_TEST(test_in_possession);
  RUN_TEST(test_tara_comparative);
  RUN_TEST(test_vat_simile);
  return UNITY_END();
}
