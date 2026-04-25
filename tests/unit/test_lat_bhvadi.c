#include "unity.h"
#include "tinanta/lat_bhvadi.h"

void setUp(void) {}
void tearDown(void) {}

void test_lat_bhvadi_one_bhu(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("BU", 1, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("Bavati", out);
}

void test_lat_bhvadi_one_gam(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("gam", 1, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("gacCati", out);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lat_bhvadi_one_bhu);
  RUN_TEST(test_lat_bhvadi_one_gam);
  return UNITY_END();
}
