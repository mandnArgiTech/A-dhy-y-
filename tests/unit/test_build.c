/* test_build.c — Story 0.1 acceptance: build compiles, version correct */
#include "unity.h"
#include "ashtadhyayi.h"
#include "samjna.h"
#include "varna.h"
#include "pratyahara.h"
#include "sandhi_vowel.h"
#include <string.h>

void setUp(void)    {}
void tearDown(void) {}

void test_version(void) {
  TEST_ASSERT_NOT_NULL(ash_version());
  TEST_ASSERT_EQUAL_STRING("0.1.0", ash_version());
}

void test_varna_vowel_classification(void) {
  TEST_ASSERT_TRUE(varna_is_vowel('a'));
  TEST_ASSERT_TRUE(varna_is_vowel('A'));
  TEST_ASSERT_TRUE(varna_is_vowel('i'));
  TEST_ASSERT_TRUE(varna_is_vowel('e'));
  TEST_ASSERT_TRUE(varna_is_vowel('O'));
  TEST_ASSERT_FALSE(varna_is_vowel('k'));
  TEST_ASSERT_FALSE(varna_is_vowel('M'));
}

void test_varna_consonant_classification(void) {
  TEST_ASSERT_TRUE(varna_is_consonant('k'));
  TEST_ASSERT_TRUE(varna_is_consonant('y'));
  TEST_ASSERT_TRUE(varna_is_consonant('h'));
  TEST_ASSERT_FALSE(varna_is_consonant('a'));
}

void test_varna_guna(void) {
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('a'));
  TEST_ASSERT_EQUAL_CHAR('e', varna_guna('i'));
  TEST_ASSERT_EQUAL_CHAR('e', varna_guna('I'));
  TEST_ASSERT_EQUAL_CHAR('o', varna_guna('u'));
  TEST_ASSERT_EQUAL_CHAR('o', varna_guna('U'));
}

void test_varna_vrddhi(void) {
  TEST_ASSERT_EQUAL_CHAR('A', varna_vrddhi('a'));
  TEST_ASSERT_EQUAL_CHAR('E', varna_vrddhi('i'));
  TEST_ASSERT_EQUAL_CHAR('O', varna_vrddhi('u'));
}

void test_pratyahara_ac(void) {
  const Pratyahara *p = pratyahara_get("ac");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_GREATER_THAN(0, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'a'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'e'));
  TEST_ASSERT_FALSE(pratyahara_contains("ac", 'k'));
}

void test_pratyahara_hal(void) {
  TEST_ASSERT_TRUE(pratyahara_contains("hal", 'k'));
  TEST_ASSERT_TRUE(pratyahara_contains("hal", 'h'));
  TEST_ASSERT_FALSE(pratyahara_contains("hal", 'a'));
}

void test_pratyahara_ik(void) {
  const Pratyahara *p = pratyahara_get("ik");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(4, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'i'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'u'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'f'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'x'));
  TEST_ASSERT_FALSE(pratyahara_contains("ik", 'a'));
}

void test_samjna_bitmask(void) {
  Samjna s = SJ_DHATU | SJ_PARASMAIPADA;
  TEST_ASSERT_TRUE(samjna_has(s, SJ_DHATU));
  TEST_ASSERT_TRUE(samjna_has(s, SJ_PARASMAIPADA));
  TEST_ASSERT_FALSE(samjna_has(s, SJ_PRATYAYA));
  s = samjna_add(s, SJ_TING);
  TEST_ASSERT_TRUE(samjna_has(s, SJ_TING));
  s = samjna_remove(s, SJ_TING);
  TEST_ASSERT_FALSE(samjna_has(s, SJ_TING));
}

void test_ash_db_load(void) {
  ASH_DB *db = ash_db_load("data/");
  TEST_ASSERT_NOT_NULL(db);
  ash_db_free(db);
}

void test_ash_tinanta_stub(void) {
  ASH_DB *db = ash_db_load("data/");
  ASH_Form f = ash_tinanta(db, "BU", 1, ASH_LAT,
                             ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  ash_db_free(db);
}

void test_ash_subanta_stub(void) {
  ASH_DB *db = ash_db_load("data/");
  ASH_Form f = ash_subanta(db, "rAma", ASH_PUMS,
                             ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  ash_db_free(db);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_version);
  RUN_TEST(test_varna_vowel_classification);
  RUN_TEST(test_varna_consonant_classification);
  RUN_TEST(test_varna_guna);
  RUN_TEST(test_varna_vrddhi);
  RUN_TEST(test_pratyahara_ac);
  RUN_TEST(test_pratyahara_hal);
  RUN_TEST(test_pratyahara_ik);
  RUN_TEST(test_samjna_bitmask);
  RUN_TEST(test_ash_db_load);
  RUN_TEST(test_ash_tinanta_stub);
  RUN_TEST(test_ash_subanta_stub);
  return UNITY_END();
}
