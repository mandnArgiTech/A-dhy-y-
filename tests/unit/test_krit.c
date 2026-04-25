#include "unity.h"
#include "krit/krit_primary.h"

void setUp(void) {}
void tearDown(void) {}

void test_kta_gam(void) {
  ASH_Form f = krit_derive("gam", 1, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gata", f.slp1);
  ash_form_free(&f);
}

void test_ktva_gam(void) {
  ASH_Form f = krit_derive("gam", 1, ASH_KRIT_KTVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gatvA", f.slp1);
  ash_form_free(&f);
}

void test_shatr_bhu(void) {
  ASH_Form f = krit_derive("BU", 1, ASH_KRIT_SHATR);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("Bavat", f.slp1);
  ash_form_free(&f);
}

void test_kta_kr(void) {
  ASH_Form f = krit_derive("kf", 8, ASH_KRIT_KTA);
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
