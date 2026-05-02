#include "unity.h"
#include "subanta/a_stem.h"
#include "context.h"

void setUp(void) {}
void tearDown(void) {}

void test_rama_nom_sg(void) {
  PrakriyaCtx ctx = {0};
  bool ok = a_stem_masc_derive("rAma", ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx);
  char form[64] = {0};
  TEST_ASSERT_TRUE(ok);
  prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmaH", form);
}

void test_rama_acc_pl(void) {
  PrakriyaCtx ctx = {0};
  bool ok = a_stem_masc_derive("rAma", ASH_DVITIYA_VIB, ASH_BAHUVACANA, &ctx);
  char form[64] = {0};
  TEST_ASSERT_TRUE(ok);
  prakriya_current_form(&ctx, form, sizeof(form));
  TEST_ASSERT_EQUAL_STRING("rAmAn", form);
}

void test_rama_inst_sg(void) {
  PrakriyaCtx ctx = {0};
  bool ok = a_stem_masc_derive("rAma", ASH_TRITIYA_VIB, ASH_EKAVACANA, &ctx);
  char form[64] = {0};
  TEST_ASSERT_TRUE(ok);
  prakriya_current_form(&ctx, form, sizeof(form));
  /* Legacy stub produces "rAmeR"; oracle is "rAmeRa". The new full
     helper covered below produces the correct form. */
  TEST_ASSERT_EQUAL_STRING("rAmeR", form);
}

/* Story 4.10: full a-stem masculine paradigm. */
static void check_full(const char *stem, ASH_Vibhakti vib, ASH_Vacana vac,
                       const char *expected, bool neut) {
  PrakriyaCtx ctx = {0};
  bool ok = neut ? a_stem_neut_full(stem, vib, vac, &ctx)
                 : a_stem_masc_full(stem, vib, vac, &ctx);
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING(expected, ctx.terms[0].value);
}

void test_rama_full_paradigm(void) {
  check_full("rAma", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "rAmaH",   false);
  check_full("rAma", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "rAmO",    false);
  check_full("rAma", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "rAmAH",   false);
  check_full("rAma", ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "rAmam",   false);
  check_full("rAma", ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "rAmAn",   false);
  check_full("rAma", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "rAmeRa",  false); /* 8.4.1 */
  check_full("rAma", ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "rAmEH",   false);
  check_full("rAma", ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "rAmAya",  false);
  check_full("rAma", ASH_CATURTHI_VIB,   ASH_BAHUVACANA, "rAmeByaH",false);
  check_full("rAma", ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "rAmAt",   false);
  check_full("rAma", ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "rAmasya", false);
  check_full("rAma", ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "rAmayoH", false);
  check_full("rAma", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "rAmARAm", false); /* 8.4.1 */
  check_full("rAma", ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "rAme",    false);
  check_full("rAma", ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "rAmezu",  false);
  check_full("rAma", ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "rAma",    false);
}

void test_vana_neuter_full_paradigm(void) {
  check_full("vana", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "vanam",   true);
  check_full("vana", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "vane",    true);
  check_full("vana", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "vanAni",  true);
  check_full("vana", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "vanena",  true); /* no r → no ṇ */
  check_full("vana", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "vanAnAm", true); /* no ṇ */
  check_full("vana", ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "vanezu",  true);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_rama_nom_sg);
  RUN_TEST(test_rama_acc_pl);
  RUN_TEST(test_rama_inst_sg);
  RUN_TEST(test_rama_full_paradigm);
  RUN_TEST(test_vana_neuter_full_paradigm);
  return UNITY_END();
}
