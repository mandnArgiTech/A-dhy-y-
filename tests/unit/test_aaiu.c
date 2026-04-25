#include "unity.h"
#include "subanta/aaiu_stems.h"
#include "context.h"

void setUp(void) {}
void tearDown(void) {}

void test_aa_stem_feminine(void) {
  PrakriyaCtx ctx;
  TEST_ASSERT_TRUE(aa_stem_fem_derive("rAmA", ASH_PRATHAMA_VIB, ASH_BAHUVACANA,
                                      &ctx));
  TEST_ASSERT_EQUAL_STRING("rAmAH", ctx.terms[0].value);
}

void test_i_stem_masculine(void) {
  PrakriyaCtx ctx;
  TEST_ASSERT_TRUE(i_stem_derive("kvi", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA,
                                 &ctx));
  TEST_ASSERT_EQUAL_STRING("kviH", ctx.terms[0].value);
}

void test_u_stem_neuter(void) {
  PrakriyaCtx ctx;
  TEST_ASSERT_TRUE(u_stem_derive("mDu", ASH_NAPUMSAKA, ASH_PRATHAMA_VIB, ASH_EKAVACANA,
                                 &ctx));
  TEST_ASSERT_EQUAL_STRING("mDu", ctx.terms[0].value);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_aa_stem_feminine);
  RUN_TEST(test_i_stem_masculine);
  RUN_TEST(test_u_stem_neuter);
  return UNITY_END();
}
