#include "unity.h"
#include "ganapatha/ganapatha.h"

static const char *locate_ganapatha_path(void) {
  FILE *fp = fopen("data/ganapatha.tsv", "r");
  if (fp) {
    fclose(fp);
    return "data/ganapatha.tsv";
  }
  fp = fopen("../data/ganapatha.tsv", "r");
  if (fp) {
    fclose(fp);
    return "../data/ganapatha.tsv";
  }
  return "data/ganapatha.tsv";
}

void setUp(void) {}
void tearDown(void) {}

void test_ganapatha_load_and_membership(void) {
  GanapathaDB db = {0};
  TEST_ASSERT_EQUAL_INT(0, ganapatha_db_load(&db, locate_ganapatha_path()));
  TEST_ASSERT_TRUE(db.count > 1000);
  TEST_ASSERT_TRUE(ganapatha_has_member(&db, "srvAdiH", "srv"));
  TEST_ASSERT_TRUE(ganapatha_has_member(&db, "mAheSvarasUtra", "hl"));
  ganapatha_db_free(&db);
}

void test_ganapatha_find_group(void) {
  GanapathaDB db = {0};
  const GanapathaEntry *e;
  TEST_ASSERT_EQUAL_INT(0, ganapatha_db_load(&db, locate_ganapatha_path()));
  e = ganapatha_find_group(&db, "srvAdiH");
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_EQUAL_STRING("srvAdiH", e->gana_name_slp1);
  ganapatha_db_free(&db);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_ganapatha_load_and_membership);
  RUN_TEST(test_ganapatha_find_group);
  return UNITY_END();
}
