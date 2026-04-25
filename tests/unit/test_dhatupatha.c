#include "unity.h"
#include "dhatupatha/dhatupatha.h"

void setUp(void) {}
void tearDown(void) {}

/* Return a data path that works under both ctest and direct execution. */
static const char *dhatu_data_path(void) {
  FILE *f = fopen("data/dhatupatha.tsv", "r");
  if (f) {
    fclose(f);
    return "data/dhatupatha.tsv";
  }
  return "../data/dhatupatha.tsv";
}

void test_dhatupatha_load_and_find(void) {
  DhatupathaDB db = {0};
  TEST_ASSERT_EQUAL_INT(0, dhatupatha_db_load(&db, dhatu_data_path()));
  TEST_ASSERT_TRUE(db.count > 1000);
  TEST_ASSERT_TRUE(dhatupatha_contains(&db, "BU"));
  {
    const DhatupathaEntry *e = dhatupatha_find(&db, "BU", 1);
    TEST_ASSERT_NOT_NULL(e);
    TEST_ASSERT_EQUAL_INT(1, e->gana);
  }
  dhatupatha_db_free(&db);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_dhatupatha_load_and_find);
  return UNITY_END();
}
