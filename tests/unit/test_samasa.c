#include "unity.h"
#include "ashtadhyayi.h"

static ASH_DB *g_db = NULL;

void setUp(void) {
  g_db = ash_db_load("data/");
}

void tearDown(void) {
  ash_db_free(g_db);
  g_db = NULL;
}

void test_tatpurusha_basic(void) {
  TEST_ASSERT_NOT_NULL(g_db);

  ASH_Form f = ash_samasa(g_db, "rAja", "puruza",
                          ASH_SAMASA_TATPURUSHA, ASH_PUMS);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAjapuruza", f.slp1);
  TEST_ASSERT_EQUAL_UINT32(201022, f.steps[0].sutra_id);
  ash_form_free(&f);
}

void test_karmadharaya_sandhi(void) {
  TEST_ASSERT_NOT_NULL(g_db);

  ASH_Form f = ash_samasa(g_db, "nIla", "utpala",
                          ASH_SAMASA_KARMADHAARAYA, ASH_PUMS);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("nIlotpala", f.slp1);
  ash_form_free(&f);
}

void test_dvandva_dual_marker(void) {
  TEST_ASSERT_NOT_NULL(g_db);

  ASH_Form f = ash_samasa(g_db, "rAma", "kfzRa",
                          ASH_SAMASA_DVANDVA, ASH_PUMS);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAmakfzRO", f.slp1);
  ash_form_free(&f);
}

void test_bahuvrihi_basic(void) {
  TEST_ASSERT_NOT_NULL(g_db);

  ASH_Form f = ash_samasa(g_db, "pIta", "aMbara",
                          ASH_SAMASA_BAHUVRIHI, ASH_PUMS);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("pItAMbara", f.slp1);
  ash_form_free(&f);
}

void test_avyayibhava_neuter_acc(void) {
  TEST_ASSERT_NOT_NULL(g_db);

  ASH_Form f = ash_samasa(g_db, "upa", "gaNgA",
                          ASH_SAMASA_AVYAYIBHAVA, ASH_NAPUMSAKA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("upagaNgAm", f.slp1);
  ash_form_free(&f);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_tatpurusha_basic);
  RUN_TEST(test_karmadharaya_sandhi);
  RUN_TEST(test_dvandva_dual_marker);
  RUN_TEST(test_bahuvrihi_basic);
  RUN_TEST(test_avyayibhava_neuter_acc);
  return UNITY_END();
}
