#include "unity.h"
#include "ashtadhyayi.h"
#include <string.h>

static ASH_DB *g_db = NULL;

void setUp(void) {
  g_db = ash_db_load("data/");
}

void tearDown(void) {
  ash_db_free(g_db);
  g_db = NULL;
}

void test_pipeline_db_load(void) {
  TEST_ASSERT_NOT_NULL(g_db);
}

void test_pipeline_bhu_bhavati(void) {
  ASH_Form f = ash_tinanta(g_db, "BU", 1, ASH_LAT,
                           ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("Bavati", f.slp1);
  TEST_ASSERT_TRUE(f.step_count > 3);
  for (int i = 0; i < f.step_count; i++) {
    TEST_ASSERT_TRUE(f.steps[i].sutra_id > 0);
  }
  ash_form_free(&f);
}

void test_pipeline_rama_prathama(void) {
  ASH_Form f = ash_subanta(g_db, "rAma", ASH_PUMS,
                           ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAmH", f.slp1);
  TEST_ASSERT_TRUE(f.step_count >= 1);
  ash_form_free(&f);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_pipeline_db_load);
  RUN_TEST(test_pipeline_bhu_bhavati);
  RUN_TEST(test_pipeline_rama_prathama);
  return UNITY_END();
}
