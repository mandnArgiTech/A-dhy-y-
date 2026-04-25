#include "unity.h"
#include "subanta/consonant_stems.h"

void setUp(void) {}
void tearDown(void) {}

void test_rajan_nom_sg(void) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_TRUE(n_stem_derive("rAjn", ASH_PUMS,
                                 ASH_PRATHAMA_VIB, ASH_EKAVACANA,
                                 &ctx));
  TEST_ASSERT_EQUAL_STRING("rAjA", ctx.terms[0].value);
}

void test_manas_nom_sg(void) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_TRUE(as_stem_derive("mns", ASH_NAPUMSAKA,
                                  ASH_PRATHAMA_VIB, ASH_EKAVACANA,
                                  &ctx));
  TEST_ASSERT_EQUAL_STRING("mnH", ctx.terms[0].value);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_rajan_nom_sg);
  RUN_TEST(test_manas_nom_sg);
  return UNITY_END();
}
