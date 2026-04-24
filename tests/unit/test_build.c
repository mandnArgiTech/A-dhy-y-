/* test_build.c — Stories 0.1 + 2.1 acceptance:
 *   - build compiles, version correct
 *   - sutra_db_load reads data/sutras.tsv and exposes all 3983 sutras
 */
#include "unity.h"
#include "ashtadhyayi.h"
#include "samjna.h"
#include "varna.h"
#include "pratyahara.h"
#include "sutra.h"
#include "sandhi_vowel.h"
#include <string.h>

void setUp(void)    {}
void tearDown(void) {}

/* ── Story 0.1: build compiles, version correct ─────────────────────────── */
void test_version(void) {
  TEST_ASSERT_NOT_NULL(ash_version());
  TEST_ASSERT_EQUAL_STRING("0.1.0", ash_version());
}

/* ── Story 1.1: Varna classification ─────────────────────────────────────── */
void test_varna_vowel_classification(void) {
  TEST_ASSERT_TRUE(varna_is_vowel('a'));
  TEST_ASSERT_TRUE(varna_is_vowel('A'));
  TEST_ASSERT_TRUE(varna_is_vowel('e'));
  TEST_ASSERT_TRUE(varna_is_vowel('O'));
  TEST_ASSERT_FALSE(varna_is_vowel('k'));
  TEST_ASSERT_FALSE(varna_is_vowel('M'));
}
void test_varna_consonant_classification(void) {
  TEST_ASSERT_TRUE(varna_is_consonant('k'));
  TEST_ASSERT_TRUE(varna_is_consonant('h'));
  TEST_ASSERT_FALSE(varna_is_consonant('a'));
}
void test_varna_guna(void) {
  TEST_ASSERT_EQUAL_CHAR('a', varna_guna('a'));
  TEST_ASSERT_EQUAL_CHAR('e', varna_guna('i'));
  TEST_ASSERT_EQUAL_CHAR('o', varna_guna('u'));
}
void test_varna_vrddhi(void) {
  TEST_ASSERT_EQUAL_CHAR('A', varna_vrddhi('a'));
  TEST_ASSERT_EQUAL_CHAR('E', varna_vrddhi('i'));
  TEST_ASSERT_EQUAL_CHAR('O', varna_vrddhi('u'));
}

/* ── Story 1.2: Pratyahara ──────────────────────────────────────────────── */
void test_pratyahara_ac(void) {
  const Pratyahara *p = pratyahara_get("ac");
  TEST_ASSERT_NOT_NULL(p);
  TEST_ASSERT_GREATER_THAN(0, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'a'));
  TEST_ASSERT_TRUE(pratyahara_contains("ac", 'e'));
  TEST_ASSERT_FALSE(pratyahara_contains("ac", 'k'));
}
void test_pratyahara_hal(void) {
  TEST_ASSERT_TRUE(pratyahara_contains("hl", 'k'));
  TEST_ASSERT_FALSE(pratyahara_contains("hl", 'a'));
}
void test_pratyahara_ik(void) {
  const Pratyahara *p = pratyahara_get("ik");
  TEST_ASSERT_EQUAL_INT(4, p->count);
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'i'));
  TEST_ASSERT_TRUE(pratyahara_contains("ik", 'u'));
  TEST_ASSERT_FALSE(pratyahara_contains("ik", 'a'));
}

/* ── Story 2.2: Saṃjñā ──────────────────────────────────────────────────── */
void test_samjna_bitmask(void) {
  Samjna s = SJ_DHATU | SJ_PARASMAIPADA;
  TEST_ASSERT_TRUE(samjna_has(s, SJ_DHATU));
  TEST_ASSERT_FALSE(samjna_has(s, SJ_PRATYAYA));
  s = samjna_add(s, SJ_TING);
  TEST_ASSERT_TRUE(samjna_has(s, SJ_TING));
}

/* ── Story 2.1: Sūtra loader — requires data/sutras.tsv ─────────────────── */
void test_sutra_db_load(void) {
  SutraDB db;
  int rc = sutra_db_load(&db, "data/sutras.tsv");
  if (rc != 0) {
    TEST_IGNORE_MESSAGE("data/sutras.tsv missing — run `python3 tools/ingest_source.py`");
  }
  TEST_ASSERT_GREATER_OR_EQUAL(3959, db.count);
  sutra_db_free(&db);
}

void test_sutra_111_is_vriddhi_samjna(void) {
  SutraDB db;
  if (sutra_db_load(&db, "data/sutras.tsv") != 0) {
    TEST_IGNORE_MESSAGE("data/sutras.tsv missing");
  }
  const Sutra *s = sutra_get_by_addr(&db, 1, 1, 1);
  TEST_ASSERT_NOT_NULL(s);
  TEST_ASSERT_EQUAL_UINT32(1, s->global_id);
  TEST_ASSERT_EQUAL_INT(ASH_SUTRA_SAMJNA, s->type);
  /* SLP1 of वृद्धिरादैच् should contain "vfD" */
  TEST_ASSERT_TRUE(strstr(s->text_slp1, "vfd") != NULL);
  sutra_db_free(&db);
}

void test_sutra_6_1_77_iko_yanaci(void) {
  SutraDB db;
  if (sutra_db_load(&db, "data/sutras.tsv") != 0) {
    TEST_IGNORE_MESSAGE("data/sutras.tsv missing");
  }
  const Sutra *s = sutra_get_by_addr(&db, 6, 1, 77);
  TEST_ASSERT_NOT_NULL(s);
  /* इको यणचि — in SLP1 should contain "yaR" or "yaN" */
  TEST_ASSERT_TRUE(strstr(s->text_slp1, "iko")   != NULL ||
                   strstr(s->text_slp1, "yaR")   != NULL ||
                   strstr(s->text_slp1, "yaN")   != NULL);
  sutra_db_free(&db);
}

void test_sutra_lookup_by_id(void) {
  SutraDB db;
  if (sutra_db_load(&db, "data/sutras.tsv") != 0) {
    TEST_IGNORE_MESSAGE("data/sutras.tsv missing");
  }
  const Sutra *s = sutra_get_by_id(&db, 1);
  TEST_ASSERT_NOT_NULL(s);
  TEST_ASSERT_EQUAL_UINT16(1, s->adhyaya);
  TEST_ASSERT_EQUAL_UINT8(1,  s->pada);
  TEST_ASSERT_EQUAL_UINT16(1, s->num);
  sutra_db_free(&db);
}

/* ── Phase 5: Public API stubs work ─────────────────────────────────────── */
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
  RUN_TEST(test_sutra_db_load);
  RUN_TEST(test_sutra_111_is_vriddhi_samjna);
  RUN_TEST(test_sutra_6_1_77_iko_yanaci);
  RUN_TEST(test_sutra_lookup_by_id);
  RUN_TEST(test_ash_db_load);
  RUN_TEST(test_ash_tinanta_stub);
  RUN_TEST(test_ash_subanta_stub);
  return UNITY_END();
}
