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

void test_r_stem_nom_and_gen(void) {
  PrakriyaCtx nom = {0};
  PrakriyaCtx gen = {0};
  TEST_ASSERT_TRUE(r_stem_can_handle("pitf"));
  TEST_ASSERT_TRUE(r_stem_derive("pitf", ASH_PRATHAMA_VIB, ASH_EKAVACANA, &nom));
  TEST_ASSERT_EQUAL_STRING("pitA", nom.terms[0].value);
  TEST_ASSERT_TRUE(r_stem_derive("pitf", ASH_SHASTHI_VIB, ASH_EKAVACANA, &gen));
  TEST_ASSERT_EQUAL_STRING("pituH", gen.terms[0].value);
}

void test_consonant_dispatch_and_reject(void) {
  char out[64] = {0};
  TEST_ASSERT_TRUE(consonant_stem_derive("mns", ASH_NAPUMSAKA,
                                         ASH_TRITIYA_VIB, ASH_EKAVACANA,
                                         out, sizeof(out)));
  TEST_ASSERT_EQUAL_STRING("mnsA", out);
  TEST_ASSERT_FALSE(consonant_stem_derive("unknown", ASH_PUMS,
                                          ASH_PRATHAMA_VIB, ASH_EKAVACANA,
                                          out, sizeof(out)));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_rajan_nom_sg);
  RUN_TEST(test_manas_nom_sg);
  RUN_TEST(test_r_stem_nom_and_gen);
  RUN_TEST(test_consonant_dispatch_and_reject);
  return UNITY_END();
}
