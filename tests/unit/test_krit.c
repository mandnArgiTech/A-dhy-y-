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

void test_ktavat_default_suffix_path(void) {
  ASH_Form f = krit_derive("pat", 1, ASH_KRIT_KTAVAT);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("patktavat", f.slp1);
  TEST_ASSERT_GREATER_THAN(1, f.step_count);
  ash_form_free(&f);
}

void test_ktva_kr_known_form(void) {
  ASH_Form f = krit_derive("kf", 8, ASH_KRIT_KTVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("kftvA", f.slp1);
  ash_form_free(&f);
}

void test_krit_needs_it_augment_predicate(void) {
  TEST_ASSERT_TRUE(krit_needs_it_augment("gam", ASH_KRIT_KTVA));
  TEST_ASSERT_FALSE(krit_needs_it_augment("a", ASH_KRIT_KTVA));
  TEST_ASSERT_FALSE(krit_needs_it_augment("gam", ASH_KRIT_TAVYA));
  TEST_ASSERT_FALSE(krit_needs_it_augment(NULL, ASH_KRIT_KTVA));
}

void test_krit_with_prefix(void) {
  ASH_Form f = krit_derive_with_prefix("gam", 1, ASH_KRIT_KTA, "pra");
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("pragakta", f.slp1);
  ash_form_free(&f);
}

void test_krit_with_empty_prefix_falls_back(void) {
  ASH_Form f = krit_derive_with_prefix("gam", 1, ASH_KRIT_KTA, "");
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gata", f.slp1);
  ash_form_free(&f);
}

void test_krit_invalid_inputs(void) {
  ASH_Form f = krit_derive("", 1, ASH_KRIT_KTA);
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_TRUE(f.error[0] != '\0');
  ash_form_free(&f);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_kta_gam);
  RUN_TEST(test_ktva_gam);
  RUN_TEST(test_shatr_bhu);
  RUN_TEST(test_kta_kr);
  RUN_TEST(test_ktavat_default_suffix_path);
  RUN_TEST(test_ktva_kr_known_form);
  RUN_TEST(test_krit_needs_it_augment_predicate);
  RUN_TEST(test_krit_with_prefix);
  RUN_TEST(test_krit_with_empty_prefix_falls_back);
  RUN_TEST(test_krit_invalid_inputs);
  return UNITY_END();
}
