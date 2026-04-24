/* test_metadata.c — Stories 2.4, 2.5, 2.6 acceptance tests
 *
 * Adhikāra + Anuvṛtti + Paribhāṣā
 */
#include "unity.h"
#include "adhikara.h"
#include "anuvrtti.h"
#include "paribhasha.h"
#include "ashtadhyayi.h"
#include "sutra.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

/* ── Adhikāra tests ────────────────────────────────────────────────── */

void test_adhikara_build_from_db(void) {
  /* Build a small mock DB */
  SutraDB db;
  memset(&db, 0, sizeof(db));
  db.sutras = calloc(5, sizeof(Sutra));
  db.count = 5;

  /* Sutra 1: regular vidhi */
  db.sutras[0].global_id = 1;
  db.sutras[0].type = ASH_SUTRA_VIDHI;

  /* Sutra 2: an adhikāra */
  db.sutras[1].global_id = 2;
  db.sutras[1].type = ASH_SUTRA_ADHIKARA;

  /* Sutra 3: vidhi governed by 2 */
  db.sutras[2].global_id = 3;
  db.sutras[2].type = ASH_SUTRA_VIDHI;

  /* Sutra 4: vidhi governed by 2 */
  db.sutras[3].global_id = 4;
  db.sutras[3].type = ASH_SUTRA_VIDHI;

  /* Sutra 5: new adhikāra */
  db.sutras[4].global_id = 5;
  db.sutras[4].type = ASH_SUTRA_ADHIKARA;

  int built = adhikara_build(&db, NULL);
  TEST_ASSERT_GREATER_OR_EQUAL(2, built);

  /* Sutra 3 and 4 should have adhikara_parent = 2 */
  TEST_ASSERT_EQUAL_UINT32(2, db.sutras[2].adhikara_parent);
  TEST_ASSERT_EQUAL_UINT32(2, db.sutras[3].adhikara_parent);

  /* Sutra 2 (an adhikāra itself) has no parent */
  TEST_ASSERT_EQUAL_UINT32(0, db.sutras[1].adhikara_parent);

  /* Sutra 5 (new adhikāra) has no parent either */
  TEST_ASSERT_EQUAL_UINT32(0, db.sutras[4].adhikara_parent);

  free(db.sutras);
}

void test_adhikara_governs_and_parent_of(void) {
  SutraDB db;
  memset(&db, 0, sizeof(db));
  db.sutras = calloc(3, sizeof(Sutra));
  db.count = 3;
  db.sutras[0].global_id = 1; db.sutras[0].type = ASH_SUTRA_ADHIKARA;
  db.sutras[1].global_id = 2; db.sutras[1].type = ASH_SUTRA_VIDHI;
  db.sutras[2].global_id = 3; db.sutras[2].type = ASH_SUTRA_VIDHI;

  adhikara_build(&db, NULL);

  /* But sutra_get_by_id requires addr_index populated. Without it, use direct fields. */
  TEST_ASSERT_EQUAL_UINT32(1, db.sutras[1].adhikara_parent);
  TEST_ASSERT_EQUAL_UINT32(1, db.sutras[2].adhikara_parent);

  free(db.sutras);
}

/* ── Anuvṛtti tests ────────────────────────────────────────────────── */

void test_anuvrtti_load_seed_data(void) {
  AnuvrttiDB db;
  int n = anuvrtti_load(&db, NULL);
  TEST_ASSERT_GREATER_THAN(0, n);
  TEST_ASSERT_GREATER_OR_EQUAL(3, db.count);
  anuvrtti_db_free(&db);
}

void test_anuvrtti_get_known_sutra(void) {
  AnuvrttiDB db;
  anuvrtti_load(&db, NULL);

  /* 6.1.77 iko yaṇ aci — seeded with 'sTAne' from 1.1.49 */
  const AnuvrttiEntry *e = anuvrtti_get(&db, 60177);
  TEST_ASSERT_NOT_NULL(e);
  TEST_ASSERT_GREATER_THAN(0, e->term_count);
  TEST_ASSERT_NOT_NULL(strstr(e->terms[0].term_slp1, "sTAne"));

  anuvrtti_db_free(&db);
}

void test_anuvrtti_get_unknown_sutra(void) {
  AnuvrttiDB db;
  anuvrtti_load(&db, NULL);
  const AnuvrttiEntry *e = anuvrtti_get(&db, 999999);
  TEST_ASSERT_NULL(e);
  anuvrtti_db_free(&db);
}

void test_anuvrtti_resolve_to_string(void) {
  AnuvrttiDB db;
  anuvrtti_load(&db, NULL);
  char buf[256];
  int n = anuvrtti_resolve(&db, 60177, buf, sizeof(buf));
  TEST_ASSERT_GREATER_THAN(0, n);
  TEST_ASSERT_NOT_NULL(strstr(buf, "sTAne"));
  anuvrtti_db_free(&db);
}

/* ── Paribhāṣā tests ────────────────────────────────────────────────── */

void test_paribhasha_get_known(void) {
  const Paribhasha *pb = paribhasha_get(PB_STHANIVAD);
  TEST_ASSERT_NOT_NULL(pb);
  TEST_ASSERT_NOT_NULL(pb->label_slp1);
  TEST_ASSERT_NOT_NULL(pb->description_en);
  TEST_ASSERT_EQUAL_STRING("sTAnivad", pb->label_slp1);
}

void test_paribhasha_all_eight_defined(void) {
  for (int i = 1; i < PB_COUNT; i++) {
    const Paribhasha *pb = paribhasha_get((ParibhashaId)i);
    TEST_ASSERT_NOT_NULL_MESSAGE(pb, "all paribhāṣās from 1 to PB_COUNT-1 defined");
    TEST_ASSERT_NOT_NULL(pb->label_slp1);
  }
}

void test_paribhasha_out_of_range(void) {
  TEST_ASSERT_NULL(paribhasha_get((ParibhashaId)0));
  TEST_ASSERT_NULL(paribhasha_get((ParibhashaId)999));
}

void test_paribhasha_sthanivad_applies(void) {
  /* Multi-char subst → sthānivad applies */
  TEST_ASSERT_TRUE(paribhasha_sthanivad_applies("gam", "jagm", 0));
  /* Single-char al substitution → does not */
  TEST_ASSERT_FALSE(paribhasha_sthanivad_applies("a", "i", 0));
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_adhikara_build_from_db);
  RUN_TEST(test_adhikara_governs_and_parent_of);
  RUN_TEST(test_anuvrtti_load_seed_data);
  RUN_TEST(test_anuvrtti_get_known_sutra);
  RUN_TEST(test_anuvrtti_get_unknown_sutra);
  RUN_TEST(test_anuvrtti_resolve_to_string);
  RUN_TEST(test_paribhasha_get_known);
  RUN_TEST(test_paribhasha_all_eight_defined);
  RUN_TEST(test_paribhasha_out_of_range);
  RUN_TEST(test_paribhasha_sthanivad_applies);
  return UNITY_END();
}
