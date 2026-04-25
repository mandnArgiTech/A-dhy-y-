#include "unity.h"
#include "taddhita/taddhita.h"

void setUp(void) {}
void tearDown(void) {}

void test_taddhita_an(void) {
  ASH_Form f = taddhita_derive(NULL, "garga", TD_AN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gArgya", f.slp1);
  ash_form_free(&f);
}

void test_taddhita_ta(void) {
  ASH_Form f = taddhita_derive(NULL, "Sukla", TD_TA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("SuklatA", f.slp1);
  ash_form_free(&f);
}

void test_taddhita_vat(void) {
  ASH_Form f = taddhita_derive(NULL, "siMha", TD_VAT);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("siMhavat", f.slp1);
  ash_form_free(&f);
}

void test_taddhita_vrddhi_predicate(void) {
  TEST_ASSERT_TRUE(taddhita_causes_vrddhi(TD_AN));
  TEST_ASSERT_FALSE(taddhita_causes_vrddhi(TD_TA));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_taddhita_an);
  RUN_TEST(test_taddhita_ta);
  RUN_TEST(test_taddhita_vat);
  RUN_TEST(test_taddhita_vrddhi_predicate);
  return UNITY_END();
}
