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

void test_aa_stem_special_cases(void) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_TRUE(aa_stem_fem_derive("rAmA", ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("rAmA", ctx.terms[0].value);
  TEST_ASSERT_TRUE(aa_stem_fem_derive("rAmA", ASH_TRITIYA_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("rAmyA", ctx.terms[0].value);
}

void test_i_stem_additional_and_fallback(void) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_TRUE(i_stem_derive("kvi", ASH_PUMS, ASH_DVITIYA_VIB, ASH_BAHUVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("kvIn", ctx.terms[0].value);
  TEST_ASSERT_TRUE(i_stem_derive("agni", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("agni", ctx.terms[0].value);
}

void test_u_stem_non_supported_linga_fails(void) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_FALSE(u_stem_derive("mDu", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_aa_stem_feminine);
  RUN_TEST(test_i_stem_masculine);
  RUN_TEST(test_u_stem_neuter);
  RUN_TEST(test_aa_stem_special_cases);
  RUN_TEST(test_i_stem_additional_and_fallback);
  RUN_TEST(test_u_stem_non_supported_linga_fails);
  return UNITY_END();
}
