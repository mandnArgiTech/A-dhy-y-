/* test_samjna.c — Story 2.2 acceptance tests */
#include "unity.h"
#include "samjna.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── Bitmask operations ─────────────────────────────────────────────── */

void test_has(void) {
  Samjna s = SJ_DHATU | SJ_PARASMAIPADA;
  TEST_ASSERT_TRUE(samjna_has(s, SJ_DHATU));
  TEST_ASSERT_TRUE(samjna_has(s, SJ_PARASMAIPADA));
  TEST_ASSERT_FALSE(samjna_has(s, SJ_PRATYAYA));
  TEST_ASSERT_FALSE(samjna_has(s, SJ_TING));
}

void test_add_remove(void) {
  Samjna s = SJ_NONE;
  TEST_ASSERT_EQUAL_UINT64(0, s);
  s = samjna_add(s, SJ_TING);
  TEST_ASSERT_TRUE(samjna_has(s, SJ_TING));
  s = samjna_add(s, SJ_SARVADHATUKA);
  TEST_ASSERT_TRUE(samjna_has(s, SJ_SARVADHATUKA));
  TEST_ASSERT_TRUE(samjna_has(s, SJ_TING));  /* still has TING */
  s = samjna_remove(s, SJ_TING);
  TEST_ASSERT_FALSE(samjna_has(s, SJ_TING));
  TEST_ASSERT_TRUE(samjna_has(s, SJ_SARVADHATUKA));
}

void test_all_38_bits_distinct(void) {
  /* Ensure no two flags collide */
  Samjna all = SJ_DHATU | SJ_PRATYAYA | SJ_ANGA | SJ_PADA | SJ_SUBANTA |
               SJ_TINANTA | SJ_SARVANAMAN | SJ_KARAKA | SJ_VIBHAKTI |
               SJ_GUNA | SJ_VRDDHI | SJ_IT | SJ_KIT | SJ_NGIT | SJ_NNIT |
               SJ_PIT | SJ_SARVADHATUKA | SJ_ARDHADHATUKA | SJ_TING |
               SJ_SUP | SJ_KRT | SJ_TADDHITA | SJ_SAMASA | SJ_NIPATA |
               SJ_UPASARGA | SJ_AVYAYA | SJ_SAT | SJ_NISTHA | SJ_KRDANTA |
               SJ_TADDHITANTA | SJ_PARASMAIPADA | SJ_ATMANEPADA |
               SJ_PRATIPADIKA | SJ_LAGHU | SJ_GURU | SJ_ANUDATTA |
               SJ_UDATTA | SJ_SVARITA;
  /* Popcount should be 38 */
  int bits = 0;
  for (uint64_t x = all; x; x &= x - 1) bits++;
  TEST_ASSERT_EQUAL_INT(38, bits);
}

/* ── describe ───────────────────────────────────────────────────────── */

void test_describe_none(void) {
  const char *d = samjna_describe(SJ_NONE);
  TEST_ASSERT_NOT_NULL(d);
  TEST_ASSERT_EQUAL_STRING("NONE", d);
}

void test_describe_single(void) {
  const char *d = samjna_describe(SJ_DHATU);
  TEST_ASSERT_NOT_NULL(d);
  TEST_ASSERT_EQUAL_STRING("DHATU", d);
}

void test_describe_combined(void) {
  Samjna s = SJ_DHATU | SJ_PARASMAIPADA;
  const char *d = samjna_describe(s);
  TEST_ASSERT_NOT_NULL(d);
  TEST_ASSERT_TRUE(strstr(d, "DHATU") != NULL);
  TEST_ASSERT_TRUE(strstr(d, "PARASMAIPADA") != NULL);
  TEST_ASSERT_TRUE(strstr(d, "|") != NULL);  /* separator */
}

void test_describe_all(void) {
  /* Ensure describe doesn't crash on a full bitmask */
  Samjna all = ~UINT64_C(0);
  const char *d = samjna_describe(all);
  TEST_ASSERT_NOT_NULL(d);
  TEST_ASSERT_TRUE(strstr(d, "DHATU") != NULL);
  TEST_ASSERT_TRUE(strstr(d, "SVARITA") != NULL);
}

/* ── sutra_of ────────────────────────────────────────────────────────── */

void test_sutra_of_known(void) {
  /* 1.1.1 = vṛddhi-samjñā */
  TEST_ASSERT_EQUAL_UINT64(SJ_VRDDHI,   samjna_of_sutra(10101));
  /* 1.1.2 = guṇa-samjñā */
  TEST_ASSERT_EQUAL_UINT64(SJ_GUNA,     samjna_of_sutra(10102));
  /* 1.3.1 = dhātu-samjñā */
  TEST_ASSERT_EQUAL_UINT64(SJ_DHATU,    samjna_of_sutra(10301));
  /* 1.4.14 = pada-samjñā */
  TEST_ASSERT_EQUAL_UINT64(SJ_PADA,     samjna_of_sutra(10414));
  /* 3.1.1  = pratyaya-samjñā */
  TEST_ASSERT_EQUAL_UINT64(SJ_PRATYAYA, samjna_of_sutra(30101));
  /* 6.4.1  = aṅga-samjñā */
  TEST_ASSERT_EQUAL_UINT64(SJ_ANGA,     samjna_of_sutra(60401));
}

void test_sutra_of_unknown(void) {
  /* Non-saṃjñā sūtra returns SJ_NONE */
  TEST_ASSERT_EQUAL_UINT64(SJ_NONE, samjna_of_sutra(999999));
  TEST_ASSERT_EQUAL_UINT64(SJ_NONE, samjna_of_sutra(0));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_has);
  RUN_TEST(test_add_remove);
  RUN_TEST(test_all_38_bits_distinct);
  RUN_TEST(test_describe_none);
  RUN_TEST(test_describe_single);
  RUN_TEST(test_describe_combined);
  RUN_TEST(test_describe_all);
  RUN_TEST(test_sutra_of_known);
  RUN_TEST(test_sutra_of_unknown);
  return UNITY_END();
}
