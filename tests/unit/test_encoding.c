/* test_encoding.c — Story 1.3 acceptance tests */
#include "unity.h"
#include "encoding.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── SLP1 → IAST ──────────────────────────────────────────────────────── */

void test_slp1_to_iast_simple_vowels(void) {
  char *r = enc_slp1_to_iast("a");
  TEST_ASSERT_EQUAL_STRING("a", r); free(r);

  r = enc_slp1_to_iast("A");
  TEST_ASSERT_EQUAL_STRING("\xc4\x81", r);  /* ā */
  free(r);

  r = enc_slp1_to_iast("I");
  TEST_ASSERT_EQUAL_STRING("\xc4\xab", r);  /* ī */
  free(r);
}

void test_slp1_to_iast_bhavati(void) {
  /* bhavati in SLP1 = "Bavati" */
  char *r = enc_slp1_to_iast("Bavati");
  TEST_ASSERT_EQUAL_STRING("bhavati", r);
  free(r);
}

void test_slp1_to_iast_rama(void) {
  char *r = enc_slp1_to_iast("rAma");
  TEST_ASSERT_EQUAL_STRING("r\xc4\x81ma", r);  /* rāma */
  free(r);
}

void test_slp1_to_iast_krishna(void) {
  /* kṛṣṇa in SLP1 = "kfzRa" */
  char *r = enc_slp1_to_iast("kfzRa");
  /* Expected: k + ṛ (ṛ = U+1E5B) + ṣ (U+1E63) + ṇ (U+1E47) + a */
  TEST_ASSERT_EQUAL_STRING("k\xe1\xb9\x9b\xe1\xb9\xa3\xe1\xb9\x87\x61", r);
  free(r);
}

void test_slp1_to_iast_bharata(void) {
  char *r = enc_slp1_to_iast("BArata");
  TEST_ASSERT_EQUAL_STRING("bh\xc4\x81rata", r);  /* bhārata */
  free(r);
}

void test_slp1_to_iast_namah(void) {
  char *r = enc_slp1_to_iast("namaH");
  TEST_ASSERT_EQUAL_STRING("nama\xe1\xb8\xa5", r);  /* namaḥ */
  free(r);
}

void test_slp1_to_iast_visnumam(void) {
  /* vizRo in SLP1 = viṣṇo */
  char *r = enc_slp1_to_iast("vizRo");
  TEST_ASSERT_EQUAL_STRING("vi\xe1\xb9\xa3\xe1\xb9\x87o", r);
  free(r);
}

/* ── IAST → SLP1 ──────────────────────────────────────────────────────── */

void test_iast_to_slp1_simple(void) {
  char *r = enc_iast_to_slp1("a");
  TEST_ASSERT_EQUAL_STRING("a", r); free(r);

  r = enc_iast_to_slp1("\xc4\x81");
  TEST_ASSERT_EQUAL_STRING("A", r); free(r);
}

void test_iast_to_slp1_bhavati(void) {
  char *r = enc_iast_to_slp1("bhavati");
  TEST_ASSERT_EQUAL_STRING("Bavati", r);
  free(r);
}

void test_iast_to_slp1_rama(void) {
  char *r = enc_iast_to_slp1("r\xc4\x81ma");
  TEST_ASSERT_EQUAL_STRING("rAma", r);
  free(r);
}

void test_iast_to_slp1_digraphs(void) {
  /* Verify digraphs are recognized longest-match first */
  char *r = enc_iast_to_slp1("kh");
  TEST_ASSERT_EQUAL_STRING("K", r); free(r);

  r = enc_iast_to_slp1("gh");
  TEST_ASSERT_EQUAL_STRING("G", r); free(r);

  r = enc_iast_to_slp1("ch");
  TEST_ASSERT_EQUAL_STRING("C", r); free(r);

  r = enc_iast_to_slp1("ai");
  TEST_ASSERT_EQUAL_STRING("E", r); free(r);

  r = enc_iast_to_slp1("au");
  TEST_ASSERT_EQUAL_STRING("O", r); free(r);
}

/* ── Round-trip ──────────────────────────────────────────────────────── */

void test_roundtrip_rama(void) {
  char *iast = enc_slp1_to_iast("rAma");
  char *back = enc_iast_to_slp1(iast);
  TEST_ASSERT_EQUAL_STRING("rAma", back);
  free(iast); free(back);
}

void test_roundtrip_bhavati(void) {
  char *iast = enc_slp1_to_iast("Bavati");
  char *back = enc_iast_to_slp1(iast);
  TEST_ASSERT_EQUAL_STRING("Bavati", back);
  free(iast); free(back);
}

void test_roundtrip_corpus(void) {
  /* 15 common Sanskrit words */
  const char *slp1_words[] = {
    "rAma", "Bavati", "gacCati", "kfzRa", "BArata",
    "namaH", "vizRo", "deva", "DArma", "sUtra",
    "sfzwi", "AtmA", "manas", "tapas", "yoga",
    NULL
  };
  for (int i = 0; slp1_words[i]; i++) {
    char *iast = enc_slp1_to_iast(slp1_words[i]);
    char *back = enc_iast_to_slp1(iast);
    TEST_ASSERT_EQUAL_STRING_MESSAGE(slp1_words[i], back, "roundtrip failed");
    free(iast); free(back);
  }
}

/* ── SLP1 → Devanāgarī ───────────────────────────────────────────────── */

void test_slp1_to_devanagari_rama(void) {
  char *r = enc_slp1_to_devanagari("rAma");
  TEST_ASSERT_NOT_NULL(r);
  /* Should start with र (U+0930 = 0xE0 0xA4 0xB0) */
  TEST_ASSERT_EQUAL_UINT8(0xe0, (uint8_t)r[0]);
  TEST_ASSERT_EQUAL_UINT8(0xa4, (uint8_t)r[1]);
  TEST_ASSERT_EQUAL_UINT8(0xb0, (uint8_t)r[2]);
  /* Then ा (mātrā A) at position 3-5 */
  TEST_ASSERT_EQUAL_UINT8(0xe0, (uint8_t)r[3]);
  TEST_ASSERT_EQUAL_UINT8(0xa4, (uint8_t)r[4]);
  TEST_ASSERT_EQUAL_UINT8(0xbe, (uint8_t)r[5]);
  free(r);
}

void test_slp1_to_devanagari_bhavati(void) {
  /* भवति */
  char *r = enc_slp1_to_devanagari("Bavati");
  TEST_ASSERT_NOT_NULL(r);
  /* Output should be भवति (no inherent-a ambiguity with explicit 'a' matras) */
  /* Check it contains भ (0xE0 0xA4 0xAD) */
  TEST_ASSERT_TRUE(strstr(r, "\xe0\xa4\xad") != NULL);  /* भ */
  TEST_ASSERT_TRUE(strstr(r, "\xe0\xa4\xb5") != NULL);  /* व */
  TEST_ASSERT_TRUE(strstr(r, "\xe0\xa4\xa4") != NULL);  /* त */
  free(r);
}

void test_slp1_to_devanagari_trailing_consonant(void) {
  /* A consonant at end must get virāma (e.g. रामम्) */
  char *r = enc_slp1_to_devanagari("rAmam");
  TEST_ASSERT_NOT_NULL(r);
  /* Should end with म् = 0xE0 0xA4 0xAE 0xE0 0xA5 0x8D */
  size_t n = strlen(r);
  TEST_ASSERT_EQUAL_UINT8(0xe0, (uint8_t)r[n-3]);
  TEST_ASSERT_EQUAL_UINT8(0xa5, (uint8_t)r[n-2]);
  TEST_ASSERT_EQUAL_UINT8(0x8d, (uint8_t)r[n-1]);
  free(r);
}

/* ── Dispatcher ash_encode ───────────────────────────────────────────── */

void test_ash_encode_routes_correctly(void) {
  char *r = ash_encode("rAma", ASH_ENC_SLP1, ASH_ENC_IAST);
  TEST_ASSERT_EQUAL_STRING("r\xc4\x81ma", r);
  free(r);
}

void test_ash_encode_identity(void) {
  char *r = ash_encode("rAma", ASH_ENC_SLP1, ASH_ENC_SLP1);
  TEST_ASSERT_EQUAL_STRING("rAma", r);
  free(r);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_slp1_to_iast_simple_vowels);
  RUN_TEST(test_slp1_to_iast_bhavati);
  RUN_TEST(test_slp1_to_iast_rama);
  RUN_TEST(test_slp1_to_iast_krishna);
  RUN_TEST(test_slp1_to_iast_bharata);
  RUN_TEST(test_slp1_to_iast_namah);
  RUN_TEST(test_slp1_to_iast_visnumam);
  RUN_TEST(test_iast_to_slp1_simple);
  RUN_TEST(test_iast_to_slp1_bhavati);
  RUN_TEST(test_iast_to_slp1_rama);
  RUN_TEST(test_iast_to_slp1_digraphs);
  RUN_TEST(test_roundtrip_rama);
  RUN_TEST(test_roundtrip_bhavati);
  RUN_TEST(test_roundtrip_corpus);
  RUN_TEST(test_slp1_to_devanagari_rama);
  RUN_TEST(test_slp1_to_devanagari_bhavati);
  RUN_TEST(test_slp1_to_devanagari_trailing_consonant);
  RUN_TEST(test_ash_encode_routes_correctly);
  RUN_TEST(test_ash_encode_identity);
  return UNITY_END();
}
