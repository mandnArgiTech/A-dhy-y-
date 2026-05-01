#include "unity.h"
#include "subanta/aaiu_stems.h"
#include "context.h"
#include <string.h>

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
  TEST_ASSERT_EQUAL_STRING("agniH", ctx.terms[0].value);
}

void test_u_stem_non_supported_linga_fails(void) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_FALSE(u_stem_derive("mDu", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx));
}

/* Story 4.6 — full agni (i-stem masc) paradigm. */
static void check_form(bool (*fn)(const char *, ASH_Vibhakti, ASH_Vacana, PrakriyaCtx *),
                       const char *stem, ASH_Vibhakti vib, ASH_Vacana vac,
                       const char *expected) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_TRUE(fn(stem, vib, vac, &ctx));
  TEST_ASSERT_EQUAL_STRING(expected, ctx.terms[0].value);
}

void test_i_masc_agni_full_paradigm(void) {
  check_form(i_stem_masc_full, "agni", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "agniH");
  check_form(i_stem_masc_full, "agni", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "agnI");
  check_form(i_stem_masc_full, "agni", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "agnayaH");
  check_form(i_stem_masc_full, "agni", ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "agnim");
  check_form(i_stem_masc_full, "agni", ASH_DVITIYA_VIB,    ASH_DVIVACANA,  "agnI");
  check_form(i_stem_masc_full, "agni", ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "agnIn");
  check_form(i_stem_masc_full, "agni", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "agninA");
  check_form(i_stem_masc_full, "agni", ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "agniByAm");
  check_form(i_stem_masc_full, "agni", ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "agniBiH");
  check_form(i_stem_masc_full, "agni", ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "agnaye");
  check_form(i_stem_masc_full, "agni", ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "agneH");
  check_form(i_stem_masc_full, "agni", ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "agneH");
  check_form(i_stem_masc_full, "agni", ASH_SHASTHI_VIB,    ASH_DVIVACANA,  "agnyoH");
  check_form(i_stem_masc_full, "agni", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "agnInAm");
  check_form(i_stem_masc_full, "agni", ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "agnO");
  check_form(i_stem_masc_full, "agni", ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "agnizu");
  check_form(i_stem_masc_full, "agni", ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "agne");
}

void test_u_masc_guru_full_paradigm(void) {
  /* guru contains `r`, so 8.4.1 ṇatva fires for n+vowel suffixes. */
  check_form(u_stem_masc_full, "guru", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "guruH");
  check_form(u_stem_masc_full, "guru", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "gurU");
  check_form(u_stem_masc_full, "guru", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "guravaH");
  check_form(u_stem_masc_full, "guru", ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "gurUn");
  check_form(u_stem_masc_full, "guru", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "guruRA");
  check_form(u_stem_masc_full, "guru", ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "guruByAm");
  check_form(u_stem_masc_full, "guru", ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "gurave");
  check_form(u_stem_masc_full, "guru", ASH_PANCAMI_VIB,    ASH_EKAVACANA,  "guroH");
  check_form(u_stem_masc_full, "guru", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "gurURAm");
  check_form(u_stem_masc_full, "guru", ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "gurO");
  check_form(u_stem_masc_full, "guru", ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "guruzu");
  check_form(u_stem_masc_full, "guru", ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "guro");
}

void test_u_neut_madhu_full_paradigm(void) {
  /* maDu has no r/f/z/F/R, so 8.4.1 does NOT fire. n stays as `n`. */
  check_form(u_stem_neut_full, "maDu", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "maDu");
  check_form(u_stem_neut_full, "maDu", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "maDunI");
  check_form(u_stem_neut_full, "maDu", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "maDUni");
  check_form(u_stem_neut_full, "maDu", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "maDunA");
  check_form(u_stem_neut_full, "maDu", ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "maDune");
  check_form(u_stem_neut_full, "maDu", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "maDUnAm");
  check_form(u_stem_neut_full, "maDu", ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "maDuzu");
}

void test_i_neut_vari_full_paradigm(void) {
  /* vAri has `r`, so n→ṇ fires. */
  check_form(i_stem_neut_full, "vAri", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "vAri");
  check_form(i_stem_neut_full, "vAri", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "vAriRI");
  check_form(i_stem_neut_full, "vAri", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "vArIRi");
  check_form(i_stem_neut_full, "vAri", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "vAriRA");
  check_form(i_stem_neut_full, "vAri", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "vArIRAm");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_aa_stem_feminine);
  RUN_TEST(test_i_stem_masculine);
  RUN_TEST(test_u_stem_neuter);
  RUN_TEST(test_aa_stem_special_cases);
  RUN_TEST(test_i_stem_additional_and_fallback);
  RUN_TEST(test_u_stem_non_supported_linga_fails);
  RUN_TEST(test_i_masc_agni_full_paradigm);
  RUN_TEST(test_u_masc_guru_full_paradigm);
  RUN_TEST(test_u_neut_madhu_full_paradigm);
  RUN_TEST(test_i_neut_vari_full_paradigm);
  return UNITY_END();
}
