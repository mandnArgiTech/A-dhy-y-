#include "unity.h"
#include "subanta/consonant_stems.h"
#include "context.h"
#include <string.h>

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

/* Story 4.7 — full an-stem paradigm. */
static void check_form(bool (*fn)(const char *, ASH_Vibhakti, ASH_Vacana, PrakriyaCtx *),
                       const char *stem, ASH_Vibhakti vib, ASH_Vacana vac,
                       const char *expected) {
  PrakriyaCtx ctx = {0};
  TEST_ASSERT_TRUE(fn(stem, vib, vac, &ctx));
  TEST_ASSERT_EQUAL_STRING(expected, ctx.terms[0].value);
}

void test_rajan_full_paradigm(void) {
  /* Selected forms covering all base kinds. */
  check_form(an_stem_masc_full, "rAjan", ASH_PRATHAMA_VIB, ASH_EKAVACANA,  "rAjA");
  check_form(an_stem_masc_full, "rAjan", ASH_PRATHAMA_VIB, ASH_DVIVACANA,  "rAjAnO");
  check_form(an_stem_masc_full, "rAjan", ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "rAjAnaH");
  check_form(an_stem_masc_full, "rAjan", ASH_DVITIYA_VIB,  ASH_EKAVACANA,  "rAjAnam");
  check_form(an_stem_masc_full, "rAjan", ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "rAjYaH");
  check_form(an_stem_masc_full, "rAjan", ASH_TRITIYA_VIB,  ASH_EKAVACANA,  "rAjYA");
  check_form(an_stem_masc_full, "rAjan", ASH_TRITIYA_VIB,  ASH_DVIVACANA,  "rAjaByAm");
  check_form(an_stem_masc_full, "rAjan", ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "rAjaBiH");
  check_form(an_stem_masc_full, "rAjan", ASH_CATURTHI_VIB, ASH_EKAVACANA,  "rAjYe");
  check_form(an_stem_masc_full, "rAjan", ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "rAjYAm");
  check_form(an_stem_masc_full, "rAjan", ASH_SAPTAMI_VIB,  ASH_EKAVACANA,  "rAjYi");
  check_form(an_stem_masc_full, "rAjan", ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "rAjasu");
  check_form(an_stem_masc_full, "rAjan", ASH_SAMBODHANA_VIB, ASH_EKAVACANA, "rAjan");
}

void test_manas_full_paradigm(void) {
  check_form(as_stem_neut_full, "manas", ASH_PRATHAMA_VIB, ASH_EKAVACANA,  "manaH");
  check_form(as_stem_neut_full, "manas", ASH_PRATHAMA_VIB, ASH_DVIVACANA,  "manasI");
  check_form(as_stem_neut_full, "manas", ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "manAMsi");
  check_form(as_stem_neut_full, "manas", ASH_TRITIYA_VIB,  ASH_EKAVACANA,  "manasA");
  check_form(as_stem_neut_full, "manas", ASH_TRITIYA_VIB,  ASH_DVIVACANA,  "manoByAm");
  check_form(as_stem_neut_full, "manas", ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "manoBiH");
  check_form(as_stem_neut_full, "manas", ASH_CATURTHI_VIB, ASH_EKAVACANA,  "manase");
  check_form(as_stem_neut_full, "manas", ASH_SHASTHI_VIB,  ASH_EKAVACANA,  "manasaH");
  check_form(as_stem_neut_full, "manas", ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "manasAm");
  check_form(as_stem_neut_full, "manas", ASH_SAPTAMI_VIB,  ASH_EKAVACANA,  "manasi");
}

/* Story 4.11: in-stem masculine paradigm (guRin). */
void test_guRin_in_stem_paradigm(void) {
  check_form(in_stem_masc_full, "guRin", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "guRI");
  check_form(in_stem_masc_full, "guRin", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "guRinO");
  check_form(in_stem_masc_full, "guRin", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "guRinaH");
  check_form(in_stem_masc_full, "guRin", ASH_DVITIYA_VIB,    ASH_EKAVACANA,  "guRinam");
  check_form(in_stem_masc_full, "guRin", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "guRinA");
  check_form(in_stem_masc_full, "guRin", ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "guRiByAm");
  check_form(in_stem_masc_full, "guRin", ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "guRiBiH");
  check_form(in_stem_masc_full, "guRin", ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "guRine");
  check_form(in_stem_masc_full, "guRin", ASH_SHASTHI_VIB,    ASH_EKAVACANA,  "guRinaH");
  check_form(in_stem_masc_full, "guRin", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "guRinAm");
  check_form(in_stem_masc_full, "guRin", ASH_SAPTAMI_VIB,    ASH_EKAVACANA,  "guRini");
  check_form(in_stem_masc_full, "guRin", ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "guRizu");
  check_form(in_stem_masc_full, "guRin", ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "guRin");
}

/* Story 4.11: vat-stem masculine paradigm (SfRvat). */
void test_SfRvat_paradigm(void) {
  check_form(vat_stem_masc_full, "SfRvat", ASH_PRATHAMA_VIB,   ASH_EKAVACANA,  "SfRvan");
  check_form(vat_stem_masc_full, "SfRvat", ASH_PRATHAMA_VIB,   ASH_DVIVACANA,  "SfRvantO");
  check_form(vat_stem_masc_full, "SfRvat", ASH_PRATHAMA_VIB,   ASH_BAHUVACANA, "SfRvantaH");
  check_form(vat_stem_masc_full, "SfRvat", ASH_DVITIYA_VIB,    ASH_BAHUVACANA, "SfRvataH");
  check_form(vat_stem_masc_full, "SfRvat", ASH_TRITIYA_VIB,    ASH_EKAVACANA,  "SfRvatA");
  check_form(vat_stem_masc_full, "SfRvat", ASH_TRITIYA_VIB,    ASH_DVIVACANA,  "SfRvadByAm");
  check_form(vat_stem_masc_full, "SfRvat", ASH_TRITIYA_VIB,    ASH_BAHUVACANA, "SfRvadBiH");
  check_form(vat_stem_masc_full, "SfRvat", ASH_CATURTHI_VIB,   ASH_EKAVACANA,  "SfRvate");
  check_form(vat_stem_masc_full, "SfRvat", ASH_SHASTHI_VIB,    ASH_BAHUVACANA, "SfRvatAm");
  check_form(vat_stem_masc_full, "SfRvat", ASH_SAPTAMI_VIB,    ASH_BAHUVACANA, "SfRvatsu");
  check_form(vat_stem_masc_full, "SfRvat", ASH_SAMBODHANA_VIB, ASH_EKAVACANA,  "SfRvan");
}

void test_pitf_full_paradigm(void) {
  check_form(r_stem_masc_full, "pitf", ASH_PRATHAMA_VIB, ASH_EKAVACANA,  "pitA");
  check_form(r_stem_masc_full, "pitf", ASH_PRATHAMA_VIB, ASH_DVIVACANA,  "pitarO");
  check_form(r_stem_masc_full, "pitf", ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "pitaraH");
  check_form(r_stem_masc_full, "pitf", ASH_DVITIYA_VIB,  ASH_EKAVACANA,  "pitaram");
  check_form(r_stem_masc_full, "pitf", ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "pitFn");
  check_form(r_stem_masc_full, "pitf", ASH_TRITIYA_VIB,  ASH_EKAVACANA,  "pitrA");
  check_form(r_stem_masc_full, "pitf", ASH_CATURTHI_VIB, ASH_EKAVACANA,  "pitre");
  check_form(r_stem_masc_full, "pitf", ASH_PANCAMI_VIB,  ASH_EKAVACANA,  "pituH");
  check_form(r_stem_masc_full, "pitf", ASH_SHASTHI_VIB,  ASH_EKAVACANA,  "pituH");
  check_form(r_stem_masc_full, "pitf", ASH_SHASTHI_VIB,  ASH_DVIVACANA,  "pitroH");
  check_form(r_stem_masc_full, "pitf", ASH_SHASTHI_VIB,  ASH_BAHUVACANA, "pitFRAm");
  check_form(r_stem_masc_full, "pitf", ASH_SAPTAMI_VIB,  ASH_EKAVACANA,  "pitari");
  check_form(r_stem_masc_full, "pitf", ASH_SAPTAMI_VIB,  ASH_BAHUVACANA, "pitfzu");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_rajan_nom_sg);
  RUN_TEST(test_manas_nom_sg);
  RUN_TEST(test_r_stem_nom_and_gen);
  RUN_TEST(test_consonant_dispatch_and_reject);
  RUN_TEST(test_rajan_full_paradigm);
  RUN_TEST(test_manas_full_paradigm);
  RUN_TEST(test_pitf_full_paradigm);
  RUN_TEST(test_guRin_in_stem_paradigm);
  RUN_TEST(test_SfRvat_paradigm);
  return UNITY_END();
}
