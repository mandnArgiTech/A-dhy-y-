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

void test_unadi_load_and_lookup_by_id(void) {
  UnadiDB db;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, unadi_data_path()));
  /* Real Uṇādi-pāṭha has 748 sūtras starting at id 1001. */
  TEST_ASSERT_EQUAL_INT(748, db.count);
  const UnadiEntry *e = unadi_lookup_by_id(&db, 1001);
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_STRING("uR", e->pratyay_slp1);
  unadi_db_free(&db);
}

void test_unadi_lookup_by_pratyay(void) {
  UnadiDB db;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, unadi_data_path()));
  const UnadiEntry *e = unadi_lookup_by_pratyay(&db, "uR");
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_UINT32(1001, e->unadi_id);
  TEST_ASSERT_NULL(unadi_lookup_by_pratyay(&db, "definitely-not-a-pratyay"));
  unadi_db_free(&db);
}

void test_unadi_cite_pratyay(void) {
  UnadiDB db;
  ASH_Form f;
  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&db, unadi_data_path()));
  f = unadi_cite_pratyay(&db, "uR");
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("uR", f.slp1);
  TEST_ASSERT_GREATER_THAN(0, f.step_count);
  TEST_ASSERT_EQUAL_UINT32(1001, f.steps[0].sutra_id);
  ash_form_free(&f);

  f = unadi_cite_pratyay(&db, "no-such-pratyay");
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_TRUE(strstr(f.error, "not found") != NULL);
  ash_form_free(&f);

  unadi_db_free(&db);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_unadi_load_and_lookup_by_id);
  RUN_TEST(test_unadi_lookup_by_pratyay);
  RUN_TEST(test_unadi_cite_pratyay);
  return UNITY_END();
}
