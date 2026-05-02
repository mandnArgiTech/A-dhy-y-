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

/* BUG-008: 7.3.77 must apply ONLY to {iz, gam, yam, iyaN}. Other gaṇa-1
   roots ending in `am` (nam, ram, kṣam, ...) take the default a-path. */
void test_lat_nam_no_chai_substitution(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("nam", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  /* Must NOT produce 'nacCati' (the over-applied 7.3.77 result). */
  TEST_ASSERT_NULL_MESSAGE(strstr(out, "cC"),
                           "nam must not undergo 7.3.77 substitution");
}

/* BUG-008: 7.3.76 kramaḥ parasmaipadeṣu lengthens kram → krAm in P. */
void test_lat_kram_parasmai_vrddhi(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("kram", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("krAmati", out);
}

/* BUG-008: a-stem gaṇa-1 root that does not end in `am` produces the
   default form. */
void test_lat_pat_default(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("pat", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("patati", out);
}

/* BUG-008: e-stem gaṇa-1 root produces the default form. */
void test_lat_sev_default(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("sev", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  /* sev → guṇa fires on e (already guṇa) → still sev → +a → seva → +ti */
  TEST_ASSERT_EQUAL_STRING("sevati", out);
}

/* BUG-008: gaṇa-4 root that is not in the GANA4_IDIRGHA list (e.g. nft)
   should not get any spurious i-lengthening. */
void test_lat_nft_gana4_no_idirgha(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("nft", 4, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("nftyati", out);
}

/* BUG-011: 7.3.101 ato dīrgho yaṅi lengthens stem-final `a` before
   uttama-puruṣa endings (mi, vas, mas in parasmai). */
void test_lat_bhu_uttama_eka_dirgha(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("BU", 1, ASH_UTTAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("BavAmi", out);
}

/* BUG-011 + BUG-012 together: BavAvaH (uttama-dvi) needs both ā-lengthening
   and final-s → visarga. */
void test_lat_bhu_uttama_dvi_dirgha_visarga(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("BU", 1, ASH_UTTAMA, ASH_DVIVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("BavAvaH", out);
}

/* BUG-012: 8.2.66 + 8.3.15 final s → H. Tas-ending forms become taH. */
void test_lat_bhu_prathama_dvi_visarga(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("BU", 1, ASH_PRATHAMA, ASH_DVIVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("BavataH", out);
}

/* BUG-012: ātmanepada forms ending in `e` should be unaffected by visarga rule. */
void test_lat_bhu_atmane_no_regression(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("BU", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_ATMANE, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("Bavate", out);
}

/* Story 3.8: i-anubandha dhātus (7.1.58 idito num dhātoḥ). */
void test_lat_ruqi_num_augment(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("ruqi~", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("ruRqati", out);
}

/* Story 3.8: ñ-it dhātu hfY → harati (Y stripped, ṛ→ar guṇa). */
void test_lat_hfY_jit(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("hfY", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("harati", out);
}

/* Story 3.7: 7.3.78 root substitutions in gaṇa-1. */
void test_lat_pa_substitution(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("pA", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("pibati", out);
}

void test_lat_stha_substitution(void) {
  /* The dhātupāṭha entry ष्ठा (zWA) is normalised to sWA via 6.1.64
     dhātv-ādeḥ ṣaḥ saḥ; the substitution table then maps sWA → tizWa. */
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("zWA", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("tizWati", out);
}

void test_lat_drsh_substitution(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("dfS", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("paSyati", out);
}

void test_lat_sad_substitution(void) {
  /* zad → sad via 6.1.64; substitution then maps sad → sId. */
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("zad", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("sIdati", out);
}

void test_lat_ghra_substitution(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("GrA", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("jiGrati", out);
}

/* Story 3.7: 7.2.116 ata upadhāyāḥ for gaṇa-10 with `a`-upadha. */
void test_lat_taq_gana10_alengthen(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("taq", 10, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("tAqayati", out);
}

/* Story 3.7: gaṇa-10 root with consonant upadha must NOT lengthen. */
void test_lat_cur_gana10_no_alengthen(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("cur", 10, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_STRING("corayati", out);
}

/* All 9 LAT-parasmai forms of bhū together — the canonical paradigm. */
void test_lat_bhu_all_nine_parasmai(void) {
  struct {
    ASH_Purusha p; ASH_Vacana v; const char *expected;
  } cases[] = {
    {ASH_PRATHAMA, ASH_EKAVACANA,  "Bavati"},
    {ASH_PRATHAMA, ASH_DVIVACANA,  "BavataH"},
    {ASH_PRATHAMA, ASH_BAHUVACANA, "Bavanti"},
    {ASH_MADHYAMA, ASH_EKAVACANA,  "Bavasi"},
    {ASH_MADHYAMA, ASH_DVIVACANA,  "BavaTaH"},
    {ASH_MADHYAMA, ASH_BAHUVACANA, "BavaTa"},
    {ASH_UTTAMA,   ASH_EKAVACANA,  "BavAmi"},
    {ASH_UTTAMA,   ASH_DVIVACANA,  "BavAvaH"},
    {ASH_UTTAMA,   ASH_BAHUVACANA, "BavAmaH"},
  };
  for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
    char out[64] = {0};
    bool ok = lat_bhvadi_derive("BU", 1, cases[i].p, cases[i].v,
                                ASH_PARASMAI, out, sizeof(out));
    TEST_ASSERT_TRUE(ok);
    TEST_ASSERT_EQUAL_STRING(cases[i].expected, out);
  }
}

/* BUG-010: dhātupāṭha upadeśa with anubandhas must be stripped before
   derivation. The standard upadeśa for ṇī (root 1.0647) is "RIva~". */
void test_lat_strips_dhatupatha_upadesa(void) {
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("RIva~", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  /* 6.1.65 ṇo naḥ: initial ṇ (R) is realised as n. */
  TEST_ASSERT_EQUAL_CHAR_MESSAGE('n', out[0],
                                 "expected output to start with 'n' not 'R'");
}

void test_lat_strips_anunasika_marker(void) {
  /* gam upadesa = "gamx~" (gana-1, root 1.1137). After stripping x~ the
     clean root is "gam"; the regular gana-1 derivation should follow. */
  char out[64] = {0};
  bool ok = lat_bhvadi_derive("gamx~", 1, ASH_PRATHAMA, ASH_EKAVACANA,
                              ASH_PARASMAI, out, sizeof(out));
  TEST_ASSERT_TRUE(ok);
  TEST_ASSERT_EQUAL_CHAR_MESSAGE('g', out[0],
                                 "expected output to start with 'g'");
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
  RUN_TEST(test_lat_nam_no_chai_substitution);
  RUN_TEST(test_lat_kram_parasmai_vrddhi);
  RUN_TEST(test_lat_pat_default);
  RUN_TEST(test_lat_sev_default);
  RUN_TEST(test_lat_nft_gana4_no_idirgha);
  RUN_TEST(test_lat_ruqi_num_augment);
  RUN_TEST(test_lat_hfY_jit);
  RUN_TEST(test_lat_pa_substitution);
  RUN_TEST(test_lat_stha_substitution);
  RUN_TEST(test_lat_drsh_substitution);
  RUN_TEST(test_lat_sad_substitution);
  RUN_TEST(test_lat_ghra_substitution);
  RUN_TEST(test_lat_taq_gana10_alengthen);
  RUN_TEST(test_lat_cur_gana10_no_alengthen);
  RUN_TEST(test_lat_bhu_uttama_eka_dirgha);
  RUN_TEST(test_lat_bhu_uttama_dvi_dirgha_visarga);
  RUN_TEST(test_lat_bhu_prathama_dvi_visarga);
  RUN_TEST(test_lat_bhu_atmane_no_regression);
  RUN_TEST(test_lat_bhu_all_nine_parasmai);
  RUN_TEST(test_lat_strips_dhatupatha_upadesa);
  RUN_TEST(test_lat_strips_anunasika_marker);
  RUN_TEST(test_lat_trace_has_real_transitions);
  return UNITY_END();
}
