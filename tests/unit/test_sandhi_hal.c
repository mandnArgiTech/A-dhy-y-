/* test_sandhi_hal.c — Story 1.5 acceptance tests (consonant + visarga sandhi) */
#include "unity.h"
#include "sandhi_hal.h"
#include "sandhi_visarga.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── 8.4.53 jaś voicing assimilation ─────────────────────────────────── */

void test_jash_k_before_a(void) {
  /* k + vowel = voiced environment → k→g */
  SandhiResult r = sandhi_hal_apply('k', 'a');
  TEST_ASSERT_EQUAL_STRING("ga", r.result_slp1);
  TEST_ASSERT_TRUE(r.changed);
  TEST_ASSERT_EQUAL_UINT32(840053, r.rule_applied);
}

void test_jash_t_before_d(void) {
  SandhiResult r = sandhi_hal_apply('t', 'd');
  TEST_ASSERT_EQUAL_STRING("dd", r.result_slp1);
}

void test_jash_p_before_a(void) {
  /* p before voiced vowel — should become b (jaś assimilation) */
  SandhiResult r = sandhi_hal_apply('p', 'a');
  TEST_ASSERT_EQUAL_STRING("ba", r.result_slp1);
}

/* ── 8.4.55 car devoicing ─────────────────────────────────────────────── */

void test_car_g_before_t(void) {
  /* g + voiceless t → k + t */
  SandhiResult r = sandhi_hal_apply('g', 't');
  TEST_ASSERT_EQUAL_STRING("kt", r.result_slp1);
  TEST_ASSERT_EQUAL_UINT32(840055, r.rule_applied);
}

void test_car_d_before_k(void) {
  SandhiResult r = sandhi_hal_apply('d', 'k');
  TEST_ASSERT_EQUAL_STRING("tk", r.result_slp1);
}

void test_car_b_before_s(void) {
  SandhiResult r = sandhi_hal_apply('b', 's');
  TEST_ASSERT_EQUAL_STRING("ps", r.result_slp1);
}

void test_jash_additional_branches(void) {
  TEST_ASSERT_EQUAL_STRING("ja", sandhi_hal_apply('c', 'a').result_slp1);
  TEST_ASSERT_EQUAL_STRING("qa", sandhi_hal_apply('w', 'a').result_slp1);
  TEST_ASSERT_EQUAL_STRING("qa", sandhi_hal_apply('q', 'a').result_slp1);
  TEST_ASSERT_EQUAL_STRING("da", sandhi_hal_apply('d', 'a').result_slp1);
  TEST_ASSERT_EQUAL_STRING("ba", sandhi_hal_apply('b', 'a').result_slp1);
}

void test_car_additional_branches(void) {
  TEST_ASSERT_EQUAL_STRING("ck", sandhi_hal_apply('j', 'k').result_slp1);
  TEST_ASSERT_EQUAL_STRING("wk", sandhi_hal_apply('q', 'k').result_slp1);
  TEST_ASSERT_EQUAL_STRING("ck", sandhi_hal_apply('c', 'k').result_slp1);
  TEST_ASSERT_EQUAL_STRING("wk", sandhi_hal_apply('w', 'k').result_slp1);
  TEST_ASSERT_EQUAL_STRING("tk", sandhi_hal_apply('t', 'k').result_slp1);
  TEST_ASSERT_EQUAL_STRING("pk", sandhi_hal_apply('p', 'k').result_slp1);
  TEST_ASSERT_EQUAL_STRING("rk", sandhi_hal_apply('r', 'k').result_slp1);
}

/* ── 8.3.23 anusvāra: final m + consonant ───────────────────────────── */

void test_anusvara_m_before_k(void) {
  SandhiResult r = sandhi_hal_apply('m', 'k');
  TEST_ASSERT_EQUAL_STRING("Mk", r.result_slp1);
  TEST_ASSERT_TRUE(r.changed);
  TEST_ASSERT_EQUAL_UINT32(830023, r.rule_applied);
}

void test_anusvara_m_before_c(void) {
  SandhiResult r = sandhi_hal_apply('m', 'c');
  TEST_ASSERT_EQUAL_STRING("Mc", r.result_slp1);
}

void test_m_before_vowel_no_change(void) {
  /* m + vowel — 8.3.23 only applies before consonants */
  SandhiResult r = sandhi_hal_apply('m', 'a');
  TEST_ASSERT_EQUAL_STRING("ma", r.result_slp1);
}

/* ── 8.4.45 nasal assimilation ─────────────────────────────────────── */

void test_nasal_k_plus_n(void) {
  /* k + n → N (velar nasal) + n  by 8.4.45 */
  SandhiResult r = sandhi_hal_apply('k', 'n');
  TEST_ASSERT_EQUAL_STRING("Nn", r.result_slp1);
  TEST_ASSERT_EQUAL_UINT32(840045, r.rule_applied);
}

void test_nasal_t_plus_m(void) {
  /* t + m → n + m */
  SandhiResult r = sandhi_hal_apply('t', 'm');
  TEST_ASSERT_EQUAL_STRING("nm", r.result_slp1);
}

void test_nasal_additional_branches(void) {
  TEST_ASSERT_EQUAL_STRING("Yn", sandhi_hal_apply('c', 'n').result_slp1);
  TEST_ASSERT_EQUAL_STRING("Rn", sandhi_hal_apply('w', 'n').result_slp1);
  TEST_ASSERT_EQUAL_STRING("mn", sandhi_hal_apply('p', 'n').result_slp1);
}

/* ── Full-word join ──────────────────────────────────────────────────── */

void test_join_tam_ca(void) {
  char out[64];
  bool changed = sandhi_hal_join("tam", "ca", out, sizeof(out));
  TEST_ASSERT_TRUE(changed);
  TEST_ASSERT_EQUAL_STRING("taMca", out);
}

void test_join_vak_asti(void) {
  char out[64];
  /* vāk + asti: k → g (before voiced vowel) */
  bool changed = sandhi_hal_join("vAk", "asti", out, sizeof(out));
  TEST_ASSERT_TRUE(changed);
  TEST_ASSERT_EQUAL_STRING("vAgasti", out);
}

void test_join_no_change_consonant_match(void) {
  /* g + g — already voiced; no change */
  char out[64];
  bool changed = sandhi_hal_join("g", "g", out, sizeof(out));
  TEST_ASSERT_FALSE(changed);
}

/* ── Visarga sandhi ──────────────────────────────────────────────────── */

void test_visarga_a_before_voiced(void) {
  /* aḥ + voiced → o */
  SandhiResult r = sandhi_visarga_apply('a', 'g');
  TEST_ASSERT_EQUAL_STRING("og", r.result_slp1);
  TEST_ASSERT_TRUE(r.changed);
}

void test_visarga_a_before_vowel(void) {
  /* aḥ + a → o (+ a) */
  SandhiResult r = sandhi_visarga_apply('a', 'a');
  TEST_ASSERT_EQUAL_STRING("oa", r.result_slp1);
  TEST_ASSERT_TRUE(r.changed);
}

void test_visarga_A_before_voiced(void) {
  /* āḥ + voiced → ā (visarga drops) */
  SandhiResult r = sandhi_visarga_apply('A', 'b');
  TEST_ASSERT_EQUAL_STRING("b", r.result_slp1);
  TEST_ASSERT_TRUE(r.changed);
}

void test_visarga_i_before_voiced(void) {
  /* iḥ + voiced → ir */
  SandhiResult r = sandhi_visarga_apply('i', 'g');
  TEST_ASSERT_EQUAL_STRING("rg", r.result_slp1);
  TEST_ASSERT_TRUE(r.changed);
}

void test_visarga_before_voiceless(void) {
  /* iḥ + k → iḥk (no change) */
  SandhiResult r = sandhi_visarga_apply('i', 'k');
  TEST_ASSERT_EQUAL_STRING("Hk", r.result_slp1);
  TEST_ASSERT_FALSE(r.changed);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_jash_k_before_a);
  RUN_TEST(test_jash_t_before_d);
  RUN_TEST(test_jash_p_before_a);
  RUN_TEST(test_car_g_before_t);
  RUN_TEST(test_car_d_before_k);
  RUN_TEST(test_car_b_before_s);
  RUN_TEST(test_jash_additional_branches);
  RUN_TEST(test_car_additional_branches);
  RUN_TEST(test_anusvara_m_before_k);
  RUN_TEST(test_anusvara_m_before_c);
  RUN_TEST(test_m_before_vowel_no_change);
  RUN_TEST(test_nasal_k_plus_n);
  RUN_TEST(test_nasal_t_plus_m);
  RUN_TEST(test_nasal_additional_branches);
  RUN_TEST(test_join_tam_ca);
  RUN_TEST(test_join_vak_asti);
  RUN_TEST(test_join_no_change_consonant_match);
  RUN_TEST(test_visarga_a_before_voiced);
  RUN_TEST(test_visarga_a_before_vowel);
  RUN_TEST(test_visarga_A_before_voiced);
  RUN_TEST(test_visarga_i_before_voiced);
  RUN_TEST(test_visarga_before_voiceless);
  return UNITY_END();
}
