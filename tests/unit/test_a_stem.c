#include "unity.h"
#include "subanta/a_stem.h"
#include "context.h"

void setUp(void) {}
void tearDown(void) {}

void test_rama_nom_sg(void) {
  PrakriyaCtx ctx = {0};
  bool ok = a_stem_masc_derive("rAm", ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx);
  char form[64] = {0};
  TEST_ASSERT_TRUE(ok);
  prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmH", form);
}

void test_rama_acc_pl(void) {
  PrakriyaCtx ctx = {0};
  bool ok = a_stem_masc_derive("rAm", ASH_DVITIYA_VIB, ASH_BAHUVACANA, &ctx);
  char form[64] = {0};
  TEST_ASSERT_TRUE(ok);
  prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmAn", form);
}

void test_rama_inst_sg(void) {
  PrakriyaCtx ctx = {0};
  bool ok = a_stem_masc_derive("rAm", ASH_TRITIYA_VIB, ASH_EKAVACANA, &ctx);
  char form[64] = {0};
  TEST_ASSERT_TRUE(ok);
  prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmeR", form);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_rama_nom_sg);
  RUN_TEST(test_rama_acc_pl);
  RUN_TEST(test_rama_inst_sg);
  return UNITY_END();
}
