#include "unity.h"
#include "unadipatha/unadi.h"
#include "test_paths.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static const char *unadi_data_path(void) {
  static char path[256];
  return test_resolve_data_file("unadipatha.tsv", path, sizeof(path));
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

void test_unadi_lookup_no_suffix_and_missing_form(void) {
  UnadiDB db = {0};
  ASH_Form f;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, unadi_data_path()));
  TEST_ASSERT_NOT_NULL(unadi_lookup(&db, "jan", NULL));
  f = unadi_form(&db, "missing", "x");
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_TRUE(strstr(f.error, "not found") != NULL);
  ash_form_free(&f);
  unadi_db_free(&db);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_unadi_load_and_lookup);
  RUN_TEST(test_unadi_attested);
  RUN_TEST(test_unadi_form);
  RUN_TEST(test_unadi_lookup_no_suffix_and_missing_form);
  return UNITY_END();
}
