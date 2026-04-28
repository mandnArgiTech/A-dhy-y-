#include "unity.h"
#include "subanta/vibhakti.h"
#include "samjna.h"

void setUp(void) {}
void tearDown(void) {}

/* Ensure the basic lookup table works for canonical masculine rows. */
void test_sup_lookup_masculine_core_cells(void) {
  const SupEntry *e1 = sup_get(ASH_PRATHAMA_VIB, ASH_EKAVACANA, ASH_PUMS);
  const SupEntry *e2 = sup_get(ASH_DVITIYA_VIB, ASH_BAHUVACANA, ASH_PUMS);
  TEST_ASSERT_NOT_NULL(e1);
  TEST_ASSERT_NOT_NULL(e2);
  TEST_ASSERT_EQUAL_STRING("su", e1->upadesa_slp1);
  TEST_ASSERT_EQUAL_STRING("Sas", e2->upadesa_slp1);
}

/* Ensure assigning a suffix adds a second term with sUP saMjYA. */
void test_sup_assign_updates_context(void) {
  PrakriyaCtx ctx = {0};
  term_init(&ctx.terms[0], "rAm", SJ_PRATIPADIKA);
  ctx.term_count = 1;
  TEST_ASSERT_EQUAL_INT(0, sup_assign(&ctx, ASH_PRATHAMA_VIB, ASH_EKAVACANA, ASH_PUMS));
  TEST_ASSERT_EQUAL_INT(2, ctx.term_count);
  TEST_ASSERT_TRUE(samjna_has(ctx.terms[1].samjna, SJ_SUP));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_sup_lookup_masculine_core_cells);
  RUN_TEST(test_sup_assign_updates_context);
  return UNITY_END();
}
