/* test_pratyahara.c — Story 1.2 acceptance tests */
#include "unity.h"
#include "pratyahara.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

void test_ac_all_vowels(void) {
  /* ac = all 9 short-form vowels: a i u ṛ ḷ e o ai au */
  const Pratyahara *p = pratyahara_get("ac");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(9, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'a'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'i'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'u'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'f'));  /* ṛ */
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'x'));  /* ḷ */
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'e'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'o'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'E'));  /* ai */
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'O'));  /* au */
  TEST_ASSERT_FALSE(pratyahara_contains("ac", 'k'));
}

void test_ik_short_vowels(void) {
  /* ik = i u ṛ ḷ */
  const Pratyahara *p = pratyahara_get("ik");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(4, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'i'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'u'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'f'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'x'));
  TEST_ASSERT_FALSE(pratyahara_contains("ik", 'a'));
}

void test_eN_e_and_o(void) {
  /* eN = e o */
  const Pratyahara *p = pratyahara_get("eN");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(2, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("eN", 'e'));
  TEST_ASSERT_TRUE(pratyahara_contains("eN", 'o'));
}

void test_Ec_ai_and_au(void) {
  /* aiC = ai au */
  const Pratyahara *p = pratyahara_get("Ec");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(2, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("Ec", 'E'));
  TEST_ASSERT_TRUE(pratyahara_contains("Ec", 'O'));
}

void test_ec_four_sandhya_vowels(void) {
  /* ec = e o ai au */
  const Pratyahara *p = pratyahara_get("ec");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(4, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ec", 'e'));
  TEST_ASSERT_TRUE(pratyahara_contains("ec", 'O'));
  TEST_ASSERT_FALSE(pratyahara_contains("ec", 'a'));
}

void test_yaR_semivowels(void) {
  /* yaṆ = y v r l — four semivowels (note: our table uses label "yR") */
  const Pratyahara *p = pratyahara_get("yR");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(4, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("yR", 'y'));
  TEST_ASSERT_TRUE(pratyahara_contains("yR", 'v'));
  TEST_ASSERT_TRUE(pratyahara_contains("yR", 'r'));
  TEST_ASSERT_TRUE(pratyahara_contains("yR", 'l'));
}

void test_jaS_voiced_stops(void) {
  /* jaŚ = j b g ḍ d — 5 voiced unaspirated stops */
  const Pratyahara *p = pratyahara_get("jS");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(5, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("jS", 'j'));
  TEST_ASSERT_TRUE(pratyahara_contains("jS", 'b'));
  TEST_ASSERT_TRUE(pratyahara_contains("jS", 'g'));
  TEST_ASSERT_TRUE(pratyahara_contains("jS", 'q'));  /* ḍ */
  TEST_ASSERT_TRUE(pratyahara_contains("jS", 'd'));
  TEST_ASSERT_FALSE(pratyahara_contains("jS", 'p'));
}

void test_Sr_three_sibilants(void) {
  /* śar = ś ṣ s */
  const Pratyahara *p = pratyahara_get("Sr");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(3, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("Sr", 'S'));
  TEST_ASSERT_TRUE(pratyahara_contains("Sr", 'z'));
  TEST_ASSERT_TRUE(pratyahara_contains("Sr", 's'));
}

void test_Ym_five_nasals(void) {
  /* ñaM = ñ m ṅ ṇ n — 5 nasal stops */
  const Pratyahara *p = pratyahara_get("Ym");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(5, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("Ym", 'Y'));
  TEST_ASSERT_TRUE(pratyahara_contains("Ym", 'm'));
  TEST_ASSERT_TRUE(pratyahara_contains("Ym", 'N'));
  TEST_ASSERT_TRUE(pratyahara_contains("Ym", 'R'));
  TEST_ASSERT_TRUE(pratyahara_contains("Ym", 'n'));
}

void test_hal_all_consonants(void) {
  /* hal = all consonants — 34 total (no length variants counted) */
  const Pratyahara *p = pratyahara_get("hl");
  TEST_ASSERT_NOT_NULL(p);
  /* 33 phonemes in REAL_SEQUENCE from h (sutra 5) to end = semivowels + nasals + stops + sibilants + h
     That's: 4 semivowels + 5 nasals + 2 (jh,bh) + 3 (gh,ḍh,dh) + 5 (j,b,g,ḍ,d)
           + 8 (kh,ph,ch,ṭh,th,c,ṭ,t) + 2 (k,p) + 3 (ś,ṣ,s) + 1 (h) = 33 */
  TEST_ASSERT_EQUAL_INT(34, p->count);  /* includes both h occurrences */
  TEST_ASSERT_TRUE(pratyahara_contains("hl", 'k'));
  TEST_ASSERT_TRUE(pratyahara_contains("hl", 'h'));
  TEST_ASSERT_TRUE(pratyahara_contains("hl", 'y'));
  TEST_ASSERT_FALSE(pratyahara_contains("hl", 'a'));
}

void test_ak_vowels_except_sandhya(void) {
  /* aK = a i u ṛ ḷ — 5 primary vowels */
  const Pratyahara *p = pratyahara_get("ak");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_EQUAL_INT(5, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ak", 'a'));
  TEST_ASSERT_TRUE(pratyahara_contains("ak", 'x'));
  TEST_ASSERT_FALSE(pratyahara_contains("ak", 'e'));
}

void test_unknown_label_returns_null(void) {
  TEST_ASSERT_NULL(pratyahara_get("xyz"));
  TEST_ASSERT_NULL(pratyahara_get(""));
  TEST_ASSERT_NULL(pratyahara_get(NULL));
}

void test_all_defined_pratyaharas_nonempty(void) {
  int n;
  const Pratyahara *all = pratyahara_all(&n);
  TEST_ASSERT_NOT_NULL(all);
  TEST_ASSERT_GREATER_THAN(30, n);  /* we define ≥30 pratyāhāras */
  for (int i = 0; i < n; i++) {
    TEST_ASSERT_GREATER_THAN_MESSAGE(0, all[i].count, "every pratyāhāra must have ≥1 member");
    TEST_ASSERT_NOT_NULL(all[i].label);
  }
}

void test_cross_reference_with_varna(void) {
  /* Every member of "ac" must also return true from varna_is_vowel */
  extern bool varna_is_vowel(char);
  const Pratyahara *p = pratyahara_get("ac");
  TEST_ASSERT_NOT_NULL(p);
  for (int i = 0; i < p->count; i++) {
    TEST_ASSERT_TRUE_MESSAGE(varna_is_vowel(p->members[i]),
                              "every ac member must be a vowel");
  }
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_ac_all_vowels);
  RUN_TEST(test_ik_short_vowels);
  RUN_TEST(test_eN_e_and_o);
  RUN_TEST(test_Ec_ai_and_au);
  RUN_TEST(test_ec_four_sandhya_vowels);
  RUN_TEST(test_yaR_semivowels);
  RUN_TEST(test_jaS_voiced_stops);
  RUN_TEST(test_Sr_three_sibilants);
  RUN_TEST(test_Ym_five_nasals);
  RUN_TEST(test_hal_all_consonants);
  RUN_TEST(test_ak_vowels_except_sandhya);
  RUN_TEST(test_unknown_label_returns_null);
  RUN_TEST(test_all_defined_pratyaharas_nonempty);
  RUN_TEST(test_cross_reference_with_varna);
  return UNITY_END();
}
