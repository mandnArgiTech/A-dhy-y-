#include "unity.h"
#include "unadipatha/unadi.h"
#include <string.h>
#include <unistd.h>

void setUp(void) {}
void tearDown(void) {}

static const char *unadi_data_path(void) {
  if (access("data/unadipatha.tsv", R_OK) == 0) return "data/unadipatha.tsv";
  return "../data/unadipatha.tsv";
}

void test_unadi_load_and_lookup(void) {
  UnadiDB db;
  const UnadiEntry *e;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, unadi_data_path()));
  TEST_ASSERT_GREATER_THAN(2, db.count);
  e = unadi_lookup(&db, "vA", "yu");
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_STRING("vAyu", e->form_slp1);
  unadi_db_free(&db);
}

void test_unadi_attested(void) {
  UnadiDB db;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, unadi_data_path()));
  TEST_ASSERT_TRUE(unadi_is_attested(&db, "manas"));
  TEST_ASSERT_FALSE(unadi_is_attested(&db, "xyz"));
  unadi_db_free(&db);
}

void test_unadi_form(void) {
  UnadiDB db;
  ASH_Form f;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, unadi_data_path()));
  f = unadi_form(&db, "jan", "u");
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("jAnu", f.slp1);
  TEST_ASSERT_GREATER_THAN(0, f.step_count);
  ash_form_free(&f);
  unadi_db_free(&db);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_unadi_load_and_lookup);
  RUN_TEST(test_unadi_attested);
  RUN_TEST(test_unadi_form);
  return UNITY_END();
}
