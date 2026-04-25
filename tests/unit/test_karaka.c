#include "unity.h"
#include "subanta/karaka.h"

void setUp(void) {}
void tearDown(void) {}

void test_karaka_default_mappings(void) {
  TEST_ASSERT_EQUAL_INT(ASH_PRATHAMA_VIB, karaka_default_vibhakti(ASH_KARTA));
  TEST_ASSERT_EQUAL_INT(ASH_DVITIYA_VIB, karaka_default_vibhakti(ASH_KARMAN));
  TEST_ASSERT_EQUAL_INT(ASH_TRITIYA_VIB, karaka_default_vibhakti(ASH_KARANA));
  TEST_ASSERT_EQUAL_INT(ASH_CATURTHI_VIB, karaka_default_vibhakti(ASH_SAMPRADANA));
  TEST_ASSERT_EQUAL_INT(ASH_PANCAMI_VIB, karaka_default_vibhakti(ASH_APADANA));
  TEST_ASSERT_EQUAL_INT(ASH_SAPTAMI_VIB, karaka_default_vibhakti(ASH_ADHIKARANA));
}

void test_karaka_and_vibhakti_names(void) {
  TEST_ASSERT_EQUAL_STRING("kartr", karaka_name(ASH_KARTA));
  TEST_ASSERT_EQUAL_STRING("karman", karaka_name(ASH_KARMAN));
  TEST_ASSERT_EQUAL_STRING("tritiya", vibhakti_name(ASH_TRITIYA_VIB));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_karaka_default_mappings);
  RUN_TEST(test_karaka_and_vibhakti_names);
  return UNITY_END();
}
