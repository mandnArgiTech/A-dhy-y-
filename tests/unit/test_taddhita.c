#include "unity.h"
#include "taddhita/taddhita.h"
#include <string.h>

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

void test_taddhita_extra_suffixes_and_names(void) {
  ASH_Form f;

  f = taddhita_derive(NULL, "deva", TD_MAT);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("devamat", f.slp1);
  ash_form_free(&f);

  f = taddhita_derive(NULL, "deva", TD_IN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("devain", f.slp1);
  ash_form_free(&f);

  f = taddhita_derive(NULL, "deva", TD_TARA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("devatara", f.slp1);
  ash_form_free(&f);

  TEST_ASSERT_EQUAL_STRING("aR", taddhita_type_name(TD_AN));
  TEST_ASSERT_EQUAL_STRING("unknown", taddhita_type_name((TaddhitaType)999));
}

void test_taddhita_error_paths(void) {
  ASH_Form f;

  f = taddhita_derive(NULL, "", TD_AN);
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_TRUE(strstr(f.error, "empty") != NULL);
  ash_form_free(&f);

  f = taddhita_derive(NULL, "deva", (TaddhitaType)0);
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_TRUE(strstr(f.error, "unsupported") != NULL);
  ash_form_free(&f);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_taddhita_an);
  RUN_TEST(test_taddhita_ta);
  RUN_TEST(test_taddhita_vat);
  RUN_TEST(test_taddhita_vrddhi_predicate);
  RUN_TEST(test_taddhita_extra_suffixes_and_names);
  RUN_TEST(test_taddhita_error_paths);
  return UNITY_END();
}
