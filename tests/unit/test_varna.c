/* test_varna.c — Story 1.1 acceptance tests */
#include "unity.h"
#include "varna.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── Vowel classification ─────────────────────────────────────────────── */

void test_all_14_vowels_classified_as_vowels(void) {
  const char *vowels = "aAiIuUfFxXeEoO";
  for (const char *p = vowels; *p; p++) {
    TEST_ASSERT_TRUE_MESSAGE(varna_is_vowel(*p), "should be vowel");
    TEST_ASSERT_FALSE_MESSAGE(varna_is_consonant(*p), "vowel is not consonant");
  }
}

void test_sandhya_vowels_are_long(void) {
  /* e, ai, o, au are always long — sūtra 1.1.1 classification */
  TEST_ASSERT_TRUE(varna_is_long('e'));
  TEST_ASSERT_TRUE(varna_is_long('E'));
  TEST_ASSERT_TRUE(varna_is_long('o'));
  TEST_ASSERT_TRUE(varna_is_long('O'));
  TEST_ASSERT_FALSE(varna_is_short('e'));
}

void test_short_long_vowel_pairs(void) {
  TEST_ASSERT_TRUE(varna_is_short('a') && varna_is_long('A'));
  TEST_ASSERT_TRUE(varna_is_short('i') && varna_is_long('I'));
  TEST_ASSERT_TRUE(varna_is_short('u') && varna_is_long('U'));
  TEST_ASSERT_TRUE(varna_is_short('f') && varna_is_long('F'));
  TEST_ASSERT_TRUE(varna_is_short('x') && varna_is_long('X'));
}

/* ── Consonant classification ─────────────────────────────────────────── */

void test_all_33_consonants(void) {
  /* 25 stops + 4 semivowels + 4 sibilants = 33 */
  const char *cons = "kKgGNcCjJYwWqQRtTdDnpPbBmyrlvSzsh";
  for (const char *p = cons; *p; p++) {
    TEST_ASSERT_TRUE_MESSAGE(varna_is_consonant(*p), "should be consonant");
    TEST_ASSERT_FALSE_MESSAGE(varna_is_vowel(*p), "consonant is not vowel");
  }
}

void test_special_markers_neither_vowel_nor_consonant(void) {
  /* anusvāra ṃ and visarga ḥ are in a class of their own */
  TEST_ASSERT_FALSE(varna_is_vowel('M'));
  TEST_ASSERT_FALSE(varna_is_consonant('M'));
  TEST_ASSERT_FALSE(varna_is_vowel('H'));
  TEST_ASSERT_FALSE(varna_is_consonant('H'));
}

void test_voicing(void) {
  /* Voiceless: k, c, ṭ, t, p + aspirates + sibilants ś/ṣ/s */
  TEST_ASSERT_TRUE(varna_is_voiceless('k'));
  TEST_ASSERT_TRUE(varna_is_voiceless('K'));
  TEST_ASSERT_TRUE(varna_is_voiceless('c'));
  TEST_ASSERT_TRUE(varna_is_voiceless('S'));
  TEST_ASSERT_TRUE(varna_is_voiceless('s'));

  /* Voiced: g, j, ḍ, d, b + aspirates + nasals + semivowels + h */
  TEST_ASSERT_TRUE(varna_is_voiced('g'));
  TEST_ASSERT_TRUE(varna_is_voiced('G'));
  TEST_ASSERT_TRUE(varna_is_voiced('n'));
  TEST_ASSERT_TRUE(varna_is_voiced('r'));
  TEST_ASSERT_TRUE(varna_is_voiced('h'));
  TEST_ASSERT_TRUE(varna_is_voiced('a'));  /* vowels are voiced */
}

void test_aspiration(void) {
  /* 10 aspirate stops: K G C J W Q T D P B */
  const char *aspirates = "KGCJWQTDPB";
  for (const char *p = aspirates; *p; p++) {
    TEST_ASSERT_TRUE_MESSAGE(varna_is_aspirate(*p), "should be aspirate");
  }
  /* Non-aspirates: k g c j ṭ ḍ t d p b */
  const char *non = "kgcjwqtdpb";
  for (const char *p = non; *p; p++) {
    TEST_ASSERT_FALSE_MESSAGE(varna_is_aspirate(*p), "should not be aspirate");
  }
}

void test_nasals(void) {
  /* 5 nasal stops: ṅ ñ ṇ n m */
  TEST_ASSERT_TRUE(varna_is_nasal('N'));
  TEST_ASSERT_TRUE(varna_is_nasal('Y'));
  TEST_ASSERT_TRUE(varna_is_nasal('R'));
  TEST_ASSERT_TRUE(varna_is_nasal('n'));
  TEST_ASSERT_TRUE(varna_is_nasal('m'));
  TEST_ASSERT_FALSE(varna_is_nasal('k'));
  TEST_ASSERT_FALSE(varna_is_nasal('a'));
}

void test_semivowels(void) {
  /* 4 semivowels (antastha): y r l v */
  TEST_ASSERT_TRUE(varna_is_semivowel('y'));
  TEST_ASSERT_TRUE(varna_is_semivowel('r'));
  TEST_ASSERT_TRUE(varna_is_semivowel('l'));
  TEST_ASSERT_TRUE(varna_is_semivowel('v'));
  TEST_ASSERT_FALSE(varna_is_semivowel('s'));
  TEST_ASSERT_FALSE(varna_is_semivowel('k'));
}

void test_sibilants(void) {
  /* 4 ūṣman: ś ṣ s h */
  TEST_ASSERT_TRUE(varna_is_sibilant('S'));
  TEST_ASSERT_TRUE(varna_is_sibilant('z'));
  TEST_ASSERT_TRUE(varna_is_sibilant('s'));
  TEST_ASSERT_TRUE(varna_is_sibilant('h'));
  TEST_ASSERT_FALSE(varna_is_sibilant('k'));
}

/* ── Sūtra 1.1.9: savarṇa — 20 test pairs ─────────────────────────────── */

void test_savarna_vowels(void) {
  /* Same-place vowels of different length are savarṇa */
  TEST_ASSERT_TRUE(varna_is_savarna('a', 'A'));
  TEST_ASSERT_TRUE(varna_is_savarna('A', 'a'));
  TEST_ASSERT_TRUE(varna_is_savarna('i', 'I'));
  TEST_ASSERT_TRUE(varna_is_savarna('u', 'U'));
  TEST_ASSERT_TRUE(varna_is_savarna('f', 'F'));
  TEST_ASSERT_TRUE(varna_is_savarna('x', 'X'));
}

void test_savarna_same_letter(void) {
  TEST_ASSERT_TRUE(varna_is_savarna('k', 'k'));
  TEST_ASSERT_TRUE(varna_is_savarna('a', 'a'));
}

void test_not_savarna_different_place_vowels(void) {
  TEST_ASSERT_FALSE(varna_is_savarna('a', 'i'));  /* kantya vs talavya */
  TEST_ASSERT_FALSE(varna_is_savarna('i', 'u'));  /* talavya vs oshthya */
  TEST_ASSERT_FALSE(varna_is_savarna('a', 'u'));
  TEST_ASSERT_FALSE(varna_is_savarna('i', 'f'));
}

void test_not_savarna_vowel_vs_consonant(void) {
  TEST_ASSERT_FALSE(varna_is_savarna('a', 'k'));
  TEST_ASSERT_FALSE(varna_is_savarna('i', 'y'));
}

void test_not_savarna_different_voicing(void) {
  /* k and g are same place (kantya) but different voicing */
  TEST_ASSERT_FALSE(varna_is_savarna('k', 'g'));
  TEST_ASSERT_FALSE(varna_is_savarna('t', 'd'));
  TEST_ASSERT_FALSE(varna_is_savarna('p', 'b'));
}

void test_not_savarna_different_aspiration(void) {
  /* k and kh: same place, same voicing, but different aspiration */
  TEST_ASSERT_FALSE(varna_is_savarna('k', 'K'));
  TEST_ASSERT_FALSE(varna_is_savarna('g', 'G'));
  TEST_ASSERT_FALSE(varna_is_savarna('d', 'D'));
}

void test_not_savarna_stop_vs_nasal_same_place(void) {
  /* k and ṅ: same place, same voicing status? no — ṅ is voiced nasal, k is voiceless */
  TEST_ASSERT_FALSE(varna_is_savarna('k', 'N'));
  /* g and ṅ: both voiced kantya but one is nasal */
  TEST_ASSERT_FALSE(varna_is_savarna('g', 'N'));
}

/* ── Sūtra 1.1.1: vṛddhi — ādaic ─────────────────────────────────────── */

void test_vrddhi_substitution(void) {
  TEST_ASSERT_EQUAL_CHAR('A', varna_vrddhi('a'));
  TEST_ASSERT_EQUAL_CHAR('A', varna_vrddhi('A'));
  TEST_ASSERT_EQUAL_CHAR('E', varna_vrddhi('i'));  /* ai */
  TEST_ASSERT_EQUAL_CHAR('E', varna_vrddhi('I'));
  TEST_ASSERT_EQUAL_CHAR('O', varna_vrddhi('u'));  /* au */
  TEST_ASSERT_EQUAL_CHAR('O', varna_vrddhi('U'));
  TEST_ASSERT_EQUAL_CHAR('A', varna_vrddhi('f'));  /* ṛ → ār */
}

/* ── Sūtra 1.1.2: guṇa — adeṅ ─────────────────────────────────────────── */

void test_guna_substitution(void) {
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('a'));
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('A'));
  TEST_ASSERT_EQUAL_CHAR('e', varna_guna('i'));
  TEST_ASSERT_EQUAL_CHAR('e', varna_guna('I'));
  TEST_ASSERT_EQUAL_CHAR('o', varna_guna('u'));
  TEST_ASSERT_EQUAL_CHAR('o', varna_guna('U'));
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('f'));  /* ṛ → ar */
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('x'));  /* ḷ → al */
}

/* ── Length conversion ───────────────────────────────────────────────── */

void test_lengthen(void) {
  TEST_ASSERT_EQUAL_CHAR('A', varna_lengthen('a'));
  TEST_ASSERT_EQUAL_CHAR('I', varna_lengthen('i'));
  TEST_ASSERT_EQUAL_CHAR('U', varna_lengthen('u'));
  TEST_ASSERT_EQUAL_CHAR('F', varna_lengthen('f'));
  TEST_ASSERT_EQUAL_CHAR('X', varna_lengthen('x'));
  /* Already long — identity */
  TEST_ASSERT_EQUAL_CHAR('A', varna_lengthen('A'));
  /* Non-vowel — identity */
  TEST_ASSERT_EQUAL_CHAR('k', varna_lengthen('k'));
}

void test_shorten(void) {
  TEST_ASSERT_EQUAL_CHAR('a', varna_shorten('A'));
  TEST_ASSERT_EQUAL_CHAR('i', varna_shorten('I'));
  TEST_ASSERT_EQUAL_CHAR('u', varna_shorten('U'));
  TEST_ASSERT_EQUAL_CHAR('f', varna_shorten('F'));
  TEST_ASSERT_EQUAL_CHAR('x', varna_shorten('X'));
  TEST_ASSERT_EQUAL_CHAR('k', varna_shorten('k'));
}

void test_savarna_rep(void) {
  /* Canonical representative = short form of a vowel class */
  TEST_ASSERT_EQUAL_CHAR('a', varna_savarna_rep('a'));
  TEST_ASSERT_EQUAL_CHAR('a', varna_savarna_rep('A'));
  TEST_ASSERT_EQUAL_CHAR('i', varna_savarna_rep('I'));
  TEST_ASSERT_EQUAL_CHAR('f', varna_savarna_rep('F'));
  TEST_ASSERT_EQUAL_CHAR('x', varna_savarna_rep('X'));
  TEST_ASSERT_EQUAL_CHAR('k', varna_savarna_rep('k'));  /* consonant — identity */
}

/* ── SLP1 validity ─────────────────────────────────────────────────────── */

void test_slp1_validity(void) {
  /* All 50 phonemes + M + H = 52 */
  TEST_ASSERT_TRUE(varna_is_valid_slp1('a'));
  TEST_ASSERT_TRUE(varna_is_valid_slp1('k'));
  TEST_ASSERT_TRUE(varna_is_valid_slp1('M'));
  TEST_ASSERT_TRUE(varna_is_valid_slp1('H'));
  TEST_ASSERT_TRUE(varna_is_valid_slp1('h'));
  /* Invalid: random letters not in SLP1 */
  TEST_ASSERT_FALSE(varna_is_valid_slp1('z' + 1));  /* { */
  TEST_ASSERT_FALSE(varna_is_valid_slp1('1'));
  TEST_ASSERT_FALSE(varna_is_valid_slp1(' '));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_all_14_vowels_classified_as_vowels);
  RUN_TEST(test_sandhya_vowels_are_long);
  RUN_TEST(test_short_long_vowel_pairs);
  RUN_TEST(test_all_33_consonants);
  RUN_TEST(test_special_markers_neither_vowel_nor_consonant);
  RUN_TEST(test_voicing);
  RUN_TEST(test_aspiration);
  RUN_TEST(test_nasals);
  RUN_TEST(test_semivowels);
  RUN_TEST(test_sibilants);
  RUN_TEST(test_savarna_vowels);
  RUN_TEST(test_savarna_same_letter);
  RUN_TEST(test_not_savarna_different_place_vowels);
  RUN_TEST(test_not_savarna_vowel_vs_consonant);
  RUN_TEST(test_not_savarna_different_voicing);
  RUN_TEST(test_not_savarna_different_aspiration);
  RUN_TEST(test_not_savarna_stop_vs_nasal_same_place);
  RUN_TEST(test_vrddhi_substitution);
  RUN_TEST(test_guna_substitution);
  RUN_TEST(test_lengthen);
  RUN_TEST(test_shorten);
  RUN_TEST(test_savarna_rep);
  RUN_TEST(test_slp1_validity);
  return UNITY_END();
}
