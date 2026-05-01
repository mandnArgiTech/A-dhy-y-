#include "unity.h"
#include "tinanta/lat_bhvadi.h"
#include "context.h"
#include <string.h>

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

/* BUG-009: prakriya trace must record real before→after transitions, not
   identical strings on every step. For BU + LAT-prathama-eka, at minimum the
   guṇa step (BU → Bo) and the tiṅ-assignment step (Bava → Bavati) should
   show a non-trivial transition. */
void test_lat_trace_has_real_transitions(void) {
  PrakriyaCtx ctx = {0};
  bool ok = lat_bhvadi_derive_ctx("BU", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                                  ASH_PARASMAI, &ctx);
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_TRUE_MESSAGE(ctx.step_count >= 2, "expected at least 2 trace steps");
  int real_transitions = 0;
  for (int i = 0; i < ctx.step_count; i++) {
    if (strcmp(ctx.steps[i].form_before, ctx.steps[i].form_after) != 0) {
      real_transitions++;
    }
  }
  TEST_ASSERT_TRUE_MESSAGE(real_transitions >= 2,
                           "at least 2 trace steps must have before != after");
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_lat_bhvadi_one_bhu);
  RUN_TEST(test_lat_bhvadi_one_gam);
  RUN_TEST(test_lat_gana4_div);
  RUN_TEST(test_lat_gana6_tud);
  RUN_TEST(test_lat_gana10_cur);
  RUN_TEST(test_lat_rejects_null_output);
  RUN_TEST(test_lat_trace_has_real_transitions);
  return UNITY_END();
}
