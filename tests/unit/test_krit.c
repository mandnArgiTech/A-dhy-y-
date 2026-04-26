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

void test_shanac_labh(void) {
  ASH_Form f = ash_krit(g_db, "laB", 1, ASH_KRIT_SHANAC);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("laBamAna", f.slp1);
  ash_form_free(&f);
}

void test_ktavat_gam(void) {
  ASH_Form f = ash_krit(g_db, "gam", 1, ASH_KRIT_KTAVAT);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gatavat", f.slp1);
  ash_form_free(&f);
}

void test_tavya_bhu(void) {
  ASH_Form f = ash_krit(g_db, "BU", 1, ASH_KRIT_TAVYA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("Bavitavya", f.slp1);
  ash_form_free(&f);
}

void test_aniiya_bhu(void) {
  ASH_Form f = ash_krit(g_db, "BU", 1, ASH_KRIT_ANIIYA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("BavanIya", f.slp1);
  ash_form_free(&f);
}

void test_ya_root(void) {
  ASH_Form f = ash_krit(g_db, "nI", 1, ASH_KRIT_YA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("nIya", f.slp1);
  ash_form_free(&f);
}

void test_lyap_prefixed(void) {
  ASH_Form f = ash_krit(g_db, "Agam", 1, ASH_KRIT_LYAP);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("Agamya", f.slp1);
  ash_form_free(&f);
}

void test_kta_kr(void) {
  ASH_Form f = ash_krit(g_db, "kf", 8, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("kfta", f.slp1);
  ash_form_free(&f);
}

void test_ktva_bhu_it_logic(void) {
  ASH_Form f = ash_krit(g_db, "BU", 1, ASH_KRIT_KTVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("BUtvA", f.slp1);
  ash_form_free(&f);
}

void test_ktva_set_root_it_augment(void) {
  ASH_Form f = ash_krit(g_db, "lab", 1, ASH_KRIT_KTVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("labitvA", f.slp1);
  ash_form_free(&f);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_kta_gam);
  RUN_TEST(test_ktva_gam);
  RUN_TEST(test_shatr_bhu);
  RUN_TEST(test_shanac_labh);
  RUN_TEST(test_ktavat_gam);
  RUN_TEST(test_tavya_bhu);
  RUN_TEST(test_aniiya_bhu);
  RUN_TEST(test_ya_root);
  RUN_TEST(test_lyap_prefixed);
  RUN_TEST(test_kta_kr);
  RUN_TEST(test_ktva_bhu_it_logic);
  RUN_TEST(test_ktva_set_root_it_augment);
  return UNITY_END();
}
