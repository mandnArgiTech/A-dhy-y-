#include "unity.h"
#include "guna_vrddhi.h"

void setUp(void) {}
void tearDown(void) {}

void test_guna_apply_to_final_bhu(void) {
  Term anga;
  Term pratyaya;
  uint32_t rule = 0;
  term_init(&anga, "BU", SJ_DHATU | SJ_ANGA);
  term_init(&pratyaya, "Sap", SJ_PRATYAYA | SJ_SARVADHATUKA);
  TEST_ASSERT_TRUE(guna_apply_to_final(&anga, &pratyaya, &rule));
  TEST_ASSERT_EQUAL_STRING("Bo", anga.value);
  TEST_ASSERT_EQUAL_UINT32(703084u, rule);
}

void test_guna_blocked_by_kit(void) {
  Term anga;
  Term pratyaya;
  uint32_t rule = 0;
  term_init(&anga, "BU", SJ_DHATU | SJ_ANGA);
  term_init(&pratyaya, "ktvA", SJ_PRATYAYA | SJ_KIT);
  TEST_ASSERT_FALSE(guna_apply_to_final(&anga, &pratyaya, &rule));
  TEST_ASSERT_EQUAL_STRING("BU", anga.value);
  TEST_ASSERT_EQUAL_UINT32(100105u, rule);
}

void test_vrddhi_apply_to_final(void) {
  Term anga;
  Term pratyaya;
  uint32_t rule = 0;
  term_init(&anga, "kavi", SJ_PRATIPADIKA | SJ_ANGA);
  term_init(&pratyaya, "aR", SJ_TADDHITA | SJ_PRATYAYA);
  TEST_ASSERT_TRUE(vrddhi_apply_to_final(&anga, &pratyaya, &rule));
  TEST_ASSERT_EQUAL_STRING("kavE", anga.value);
  TEST_ASSERT_EQUAL_UINT32(702115u, rule);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_guna_apply_to_final_bhu);
  RUN_TEST(test_guna_blocked_by_kit);
  RUN_TEST(test_vrddhi_apply_to_final);
  return UNITY_END();
}
