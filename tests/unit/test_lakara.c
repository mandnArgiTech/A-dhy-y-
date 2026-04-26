#include "unity.h"
#include "tinanta/lakara.h"
#include <string.h>

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

void test_ting_get_rejects_invalid_lakara(void) {
  const TingEntry *t = ting_get(ASH_LIT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_NULL(t);
}

void test_ting_assign_rejects_null_context(void) {
  TEST_ASSERT_EQUAL_INT(-1, ting_assign(NULL, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI));
}

void test_ting_assign_success_and_context_updates(void) {
  PrakriyaCtx ctx;
  int rc;

  memset(&ctx, 0, sizeof(ctx));
  rc = ting_assign(&ctx, ASH_LAT, ASH_MADHYAMA, ASH_DVIVACANA, ASH_PARASMAI);
  TEST_ASSERT_EQUAL_INT(0, rc);
  TEST_ASSERT_EQUAL_INT(1, ctx.term_count);
  TEST_ASSERT_EQUAL_STRING("Tas", ctx.terms[0].value);
  TEST_ASSERT_TRUE(samjna_has(ctx.terms[0].samjna, SJ_TING));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lat_parasmai_prathama_eka);
  RUN_TEST(test_lat_atmane_prathama_eka);
  RUN_TEST(test_lat_all_18_forms_available);
  RUN_TEST(test_lakara_name_and_sarvadhatuka);
  RUN_TEST(test_ting_get_rejects_invalid_lakara);
  RUN_TEST(test_ting_assign_rejects_null_context);
  RUN_TEST(test_ting_assign_success_and_context_updates);
  return UNITY_END();
}
