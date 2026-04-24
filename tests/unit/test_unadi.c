/* test_unadi.c — Story 5.5 baseline tests */
#include "unity.h"
#include "unadipatha/unadi.h"

void setUp(void) {}
void tearDown(void) {}

void test_vayu_lookup(void) {
  UnadiDB db;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, "data/unadipatha.tsv"));
  const UnadiEntry *e = unadi_lookup(&db, "vA", "yu");
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_STRING("vAyu", e->form_slp1);
  unadi_db_free(&db);
}

void test_janu_lookup(void) {
  UnadiDB db;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, "data/unadipatha.tsv"));
  const UnadiEntry *e = unadi_lookup(&db, "jan", "u");
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_STRING("jAnu", e->form_slp1);
  unadi_db_free(&db);
}

void test_not_in_corpus(void) {
  UnadiDB db;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, "data/unadipatha.tsv"));
  TEST_ASSERT_NULL(unadi_lookup(&db, "zzz", NULL));
  unadi_db_free(&db);
}

void test_is_attested(void) {
  UnadiDB db;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, "data/unadipatha.tsv"));
  TEST_ASSERT_TRUE(unadi_is_attested(&db, "manas"));
  TEST_ASSERT_TRUE(unadi_is_attested(&db, "tapas"));
  TEST_ASSERT_FALSE(unadi_is_attested(&db, "xyz"));
  unadi_db_free(&db);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_vayu_lookup);
  RUN_TEST(test_janu_lookup);
  RUN_TEST(test_not_in_corpus);
  RUN_TEST(test_is_attested);
  return UNITY_END();
}
