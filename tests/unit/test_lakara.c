#include "unity.h"
#include "tinanta/lakara.h"

void setUp(void) {}
void tearDown(void) {}

void test_lat_parasmai_prathama_eka(void) {
  const TingEntry *t = ting_get(ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_NOT_NULL(t);
  TEST_ASSERT_EQUAL_STRING("tip", t->upadesa);
  TEST_ASSERT_EQUAL_STRING("ti", t->clean);
  TEST_ASSERT_TRUE(samjna_has(t->samjna, SJ_SARVADHATUKA));
}

void test_lat_atmane_prathama_eka(void) {
  const TingEntry *t = ting_get(ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_ATMANE);
  TEST_ASSERT_NOT_NULL(t);
  TEST_ASSERT_EQUAL_STRING("ta", t->upadesa);
  TEST_ASSERT_EQUAL_STRING("te", t->clean);
}

void test_lat_all_18_forms_available(void) {
  for (int p = ASH_PRATHAMA; p <= ASH_UTTAMA; p++) {
    for (int v = ASH_EKAVACANA; v <= ASH_BAHUVACANA; v++) {
      const TingEntry *tp = ting_get(ASH_LAT, (ASH_Purusha)p, (ASH_Vacana)v, ASH_PARASMAI);
      const TingEntry *ta = ting_get(ASH_LAT, (ASH_Purusha)p, (ASH_Vacana)v, ASH_ATMANE);
      TEST_ASSERT_NOT_NULL(tp);
      TEST_ASSERT_NOT_NULL(ta);
      TEST_ASSERT_NOT_NULL(tp->clean);
      TEST_ASSERT_NOT_NULL(ta->clean);
    }
  }
}

void test_lakara_name_and_sarvadhatuka(void) {
  TEST_ASSERT_EQUAL_STRING("laT", lakara_name(ASH_LAT));
  TEST_ASSERT_TRUE(lakara_is_sarvadhatuka(ASH_LAT));
  TEST_ASSERT_FALSE(lakara_is_sarvadhatuka(ASH_LIT));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lat_parasmai_prathama_eka);
  RUN_TEST(test_lat_atmane_prathama_eka);
  RUN_TEST(test_lat_all_18_forms_available);
  RUN_TEST(test_lakara_name_and_sarvadhatuka);
  return UNITY_END();
}
