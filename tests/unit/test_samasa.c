#include "unity.h"
#include "samasa.h"

void setUp(void) {}
void tearDown(void) {}

void test_tatpurusha_basic(void) {
  ASH_Form f = samasa_derive(NULL, "rAja", "puruzwa",
                             ASH_SAMASA_TATPURUSHA, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAjapuruzwa", f.slp1);
  TEST_ASSERT_EQUAL_UINT32(201022, f.steps[0].sutra_id);
  ash_form_free(&f);
}

void test_karmadhAraya_sandhi(void) {
  ASH_Form f = samasa_derive(NULL, "nIla", "utpala",
                             ASH_SAMASA_KARMADHAARAYA, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("nIlotpala", f.slp1);
  ash_form_free(&f);
}

void test_dvandva_dual_marker(void) {
  ASH_Form f = samasa_derive(NULL, "rAma", "kfzRa",
                             ASH_SAMASA_DVANDVA, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAmakfzRO", f.slp1);
  ash_form_free(&f);
}

void test_avyayibhava_neuter_acc_sg_marker(void) {
  ASH_Form f = samasa_derive(NULL, "upa", "gaNgA",
                             ASH_SAMASA_AVYAYIBHAVA, ASH_NAPUMSAKA, NULL);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("upagaNgAm", f.slp1);
  ash_form_free(&f);
}

void test_bahuvrihi_and_dvigu_paths(void) {
  ASH_Form f1 = samasa_derive(NULL, "mahA", "puruza",
                              ASH_SAMASA_BAHUVRIHI, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f1.valid);
  TEST_ASSERT_EQUAL_UINT32(202023, f1.steps[0].sutra_id);
  ash_form_free(&f1);

  ASH_Form f2 = samasa_derive(NULL, "tri", "loka",
                              ASH_SAMASA_DVIGU, ASH_PUMS, NULL);
  TEST_ASSERT_TRUE(f2.valid);
  TEST_ASSERT_EQUAL_UINT32(201052, f2.steps[0].sutra_id);
  ash_form_free(&f2);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_tatpurusha_basic);
  RUN_TEST(test_karmadhAraya_sandhi);
  RUN_TEST(test_dvandva_dual_marker);
  RUN_TEST(test_avyayibhava_neuter_acc_sg_marker);
  RUN_TEST(test_bahuvrihi_and_dvigu_paths);
  return UNITY_END();
}
