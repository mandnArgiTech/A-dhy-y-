/* test_sandhi_vowel.c — Story 1.4 acceptance tests */
#include "unity.h"
#include "sandhi_vowel.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── 6.1.101 savarṇadīrgha ────────────────────────────────────────────── */

void test_savarna_dirgha_a_plus_a(void) {
  SandhiResult r = sandhi_vowel_apply('a', 'a');
  TEST_ASSERT_TRUE(r.changed);
  TEST_ASSERT_EQUAL_STRING("A", r.result_slp1);
  TEST_ASSERT_EQUAL_UINT32(610101, r.rule_applied);
}

void test_savarna_dirgha_A_plus_a(void) {
  SandhiResult r = sandhi_vowel_apply('A', 'a');
  TEST_ASSERT_EQUAL_STRING("A", r.result_slp1);
}

void test_savarna_dirgha_i_plus_i(void) {
  SandhiResult r = sandhi_vowel_apply('i', 'i');
  TEST_ASSERT_EQUAL_STRING("I", r.result_slp1);
}

void test_savarna_dirgha_u_plus_U(void) {
  SandhiResult r = sandhi_vowel_apply('u', 'U');
  TEST_ASSERT_EQUAL_STRING("U", r.result_slp1);
}

/* ── 6.1.87 guṇa (a/ā + ik) ──────────────────────────────────────────── */

void test_guna_a_plus_i(void) {
  SandhiResult r = sandhi_vowel_apply('a', 'i');
  TEST_ASSERT_TRUE(r.changed);
  TEST_ASSERT_EQUAL_STRING("e", r.result_slp1);
  TEST_ASSERT_EQUAL_UINT32(610087, r.rule_applied);
}

void test_guna_a_plus_u(void) {
  SandhiResult r = sandhi_vowel_apply('a', 'u');
  TEST_ASSERT_EQUAL_STRING("o", r.result_slp1);
}

void test_guna_A_plus_i(void) {
  SandhiResult r = sandhi_vowel_apply('A', 'i');
  TEST_ASSERT_EQUAL_STRING("e", r.result_slp1);
}

void test_guna_a_plus_r(void) {
  /* a + ṛ → ar (two chars) */
  SandhiResult r = sandhi_vowel_apply('a', 'f');
  TEST_ASSERT_EQUAL_STRING("ar", r.result_slp1);
}

/* ── 6.1.88 vṛddhi (a/ā + eC) ────────────────────────────────────────── */

void test_vrddhi_a_plus_e(void) {
  /* a + e → ai */
  SandhiResult r = sandhi_vowel_apply('a', 'e');
  TEST_ASSERT_EQUAL_STRING("E", r.result_slp1);
  TEST_ASSERT_EQUAL_UINT32(610088, r.rule_applied);
}

void test_vrddhi_a_plus_o(void) {
  /* a + o → au */
  SandhiResult r = sandhi_vowel_apply('a', 'o');
  TEST_ASSERT_EQUAL_STRING("O", r.result_slp1);
}

void test_vrddhi_A_plus_ai(void) {
  SandhiResult r = sandhi_vowel_apply('A', 'E');
  TEST_ASSERT_EQUAL_STRING("E", r.result_slp1);
}

/* ── 6.1.77 yaṆ (ik + vowel) ─────────────────────────────────────────── */

void test_yan_i_plus_a(void) {
  SandhiResult r = sandhi_vowel_apply('i', 'a');
  TEST_ASSERT_TRUE(r.changed);
  TEST_ASSERT_EQUAL_STRING("ya", r.result_slp1);
  TEST_ASSERT_EQUAL_UINT32(610077, r.rule_applied);
}

void test_yan_u_plus_a(void) {
  SandhiResult r = sandhi_vowel_apply('u', 'a');
  TEST_ASSERT_EQUAL_STRING("va", r.result_slp1);
}

void test_yan_r_plus_a(void) {
  /* ṛ + a → ra */
  SandhiResult r = sandhi_vowel_apply('f', 'a');
  TEST_ASSERT_EQUAL_STRING("ra", r.result_slp1);
}

void test_yan_U_plus_e(void) {
  /* ū + e → ve */
  SandhiResult r = sandhi_vowel_apply('U', 'e');
  TEST_ASSERT_EQUAL_STRING("ve", r.result_slp1);
}

/* ── 6.1.78 ayavādi (eC + vowel) ─────────────────────────────────────── */

void test_ayavadi_e_plus_a(void) {
  /* e + a → aya */
  SandhiResult r = sandhi_vowel_apply('e', 'a');
  TEST_ASSERT_EQUAL_STRING("aya", r.result_slp1);
  TEST_ASSERT_EQUAL_UINT32(610078, r.rule_applied);
}

void test_ayavadi_o_plus_a(void) {
  /* o + a → ava */
  SandhiResult r = sandhi_vowel_apply('o', 'a');
  TEST_ASSERT_EQUAL_STRING("ava", r.result_slp1);
}

void test_ayavadi_ai_plus_a(void) {
  /* ai + a → āya */
  SandhiResult r = sandhi_vowel_apply('E', 'a');
  TEST_ASSERT_EQUAL_STRING("Aya", r.result_slp1);
}

/* ── Full-word join ──────────────────────────────────────────────────── */

void test_join_rama_asti(void) {
  char out[64];
  bool changed = sandhi_vowel_join("rAma", "asti", out, sizeof(out));
  TEST_ASSERT_TRUE(changed);
  /* rāma + asti: final ā + initial a = savarṇadīrgha → ā
     Result: rām + ā + sti = rāmāsti */
  TEST_ASSERT_EQUAL_STRING("rAmAsti", out);
}

void test_join_ca_iti(void) {
  char out[64];
  bool changed = sandhi_vowel_join("ca", "iti", out, sizeof(out));
  TEST_ASSERT_TRUE(changed);
  /* ca + iti: a + i → e (guṇa) → ceti */
  TEST_ASSERT_EQUAL_STRING("ceti", out);
}

void test_join_ca_eva(void) {
  char out[64];
  bool changed = sandhi_vowel_join("ca", "eva", out, sizeof(out));
  TEST_ASSERT_TRUE(changed);
  /* ca + eva: a + e → ai (vṛddhi) → caiva */
  TEST_ASSERT_EQUAL_STRING("cEva", out);
}

void test_join_iti_api(void) {
  char out[64];
  bool changed = sandhi_vowel_join("iti", "api", out, sizeof(out));
  TEST_ASSERT_TRUE(changed);
  /* iti + api: i + a → ya (yaṆ) → ityapi */
  TEST_ASSERT_EQUAL_STRING("ityapi", out);
}

void test_join_ne_ati(void) {
  char out[64];
  bool changed = sandhi_vowel_join("ne", "ati", out, sizeof(out));
  TEST_ASSERT_TRUE(changed);
  /* ne + ati: e + a → aya → nayati */
  TEST_ASSERT_EQUAL_STRING("nayati", out);
}

void test_join_no_sandhi_consonant_initial(void) {
  char out[64];
  bool changed = sandhi_vowel_join("rAma", "kfta", out, sizeof(out));
  /* Vowel + consonant — no vowel sandhi triggered */
  TEST_ASSERT_FALSE(changed);
  TEST_ASSERT_EQUAL_STRING("rAmakfta", out);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_savarna_dirgha_a_plus_a);
  RUN_TEST(test_savarna_dirgha_A_plus_a);
  RUN_TEST(test_savarna_dirgha_i_plus_i);
  RUN_TEST(test_savarna_dirgha_u_plus_U);
  RUN_TEST(test_guna_a_plus_i);
  RUN_TEST(test_guna_a_plus_u);
  RUN_TEST(test_guna_A_plus_i);
  RUN_TEST(test_guna_a_plus_r);
  RUN_TEST(test_vrddhi_a_plus_e);
  RUN_TEST(test_vrddhi_a_plus_o);
  RUN_TEST(test_vrddhi_A_plus_ai);
  RUN_TEST(test_yan_i_plus_a);
  RUN_TEST(test_yan_u_plus_a);
  RUN_TEST(test_yan_r_plus_a);
  RUN_TEST(test_yan_U_plus_e);
  RUN_TEST(test_ayavadi_e_plus_a);
  RUN_TEST(test_ayavadi_o_plus_a);
  RUN_TEST(test_ayavadi_ai_plus_a);
  RUN_TEST(test_join_rama_asti);
  RUN_TEST(test_join_ca_iti);
  RUN_TEST(test_join_ca_eva);
  RUN_TEST(test_join_iti_api);
  RUN_TEST(test_join_ne_ati);
  RUN_TEST(test_join_no_sandhi_consonant_initial);
  return UNITY_END();
}
