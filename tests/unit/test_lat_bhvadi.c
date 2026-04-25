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

void test_lat_gana4_div(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("div", 4, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI,
                              out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("dIvyati", out);
}

void test_lat_gana6_tud(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("tud", 6, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI,
                              out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("tudati", out);
}

void test_lat_gana10_cur(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("cur", 10, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI,
                              out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("corayati", out);
}

void test_lat_rejects_null_output(void) {
  TEST_ASSERT_FALSE(lat_bhvadi_derive("BU", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                                      ASH_PARASMAI, NULL, 0));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lat_bhvadi_one_bhu);
  RUN_TEST(test_lat_bhvadi_one_gam);
  RUN_TEST(test_lat_gana4_div);
  RUN_TEST(test_lat_gana6_tud);
  RUN_TEST(test_lat_gana10_cur);
  RUN_TEST(test_lat_rejects_null_output);
  return UNITY_END();
}
