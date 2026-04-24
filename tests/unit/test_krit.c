#include "unity.h"
#include "ashtadhyayi.h"

static ASH_DB *g_db = NULL;

void setUp(void) {
  g_db = ash_db_load("data/");
}

void tearDown(void) {
  ash_db_free(g_db);
  g_db = NULL;
}

void test_kta_gam(void) {
  ASH_Form f = ash_krit(g_db, "gam", 1, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gata", f.slp1);
  TEST_ASSERT_EQUAL_INT(1, f.step_count);
  ash_form_free(&f);
}

void test_ktva_gam(void) {
  ASH_Form f = ash_krit(g_db, "gam", 1, ASH_KRIT_KTVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gatvA", f.slp1);
  ash_form_free(&f);
}

void test_shatr_bhu(void) {
  ASH_Form f = ash_krit(g_db, "BU", 1, ASH_KRIT_SHATR);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("Bavat", f.slp1);
  ash_form_free(&f);
}

void test_kta_kr(void) {
  ASH_Form f = ash_krit(g_db, "kf", 8, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("kfta", f.slp1);
  ash_form_free(&f);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_kta_gam);
  RUN_TEST(test_ktva_gam);
  RUN_TEST(test_shatr_bhu);
  RUN_TEST(test_kta_kr);
  return UNITY_END();
}
