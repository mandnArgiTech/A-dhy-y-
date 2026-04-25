#include "unity.h"
#include "ashtadhyayi.h"
#include "encoding.h"
#include "sutra.h"
#include "term.h"
#include "context.h"
#include "pipeline.h"
#include "sandhi_vowel.h"
#include "sandhi_hal.h"
#include "sandhi_visarga.h"
#include "samasa.h"
#include "krit/krit_primary.h"
#include "taddhita/taddhita.h"
#include "subanta/a_stem.h"
#include "subanta/aaiu_stems.h"
#include "subanta/consonant_stems.h"
#include "subanta/karaka.h"
#include "adhikara.h"
#include "paribhasha.h"
#include "pratyahara.h"
#include "samjna.h"
#include "guna_vrddhi.h"
#include "conflict.h"
#include "dhatupatha/dhatupatha.h"
#include "ganapatha/ganapatha.h"
#include "unadipatha/unadi.h"
#include "tinanta/vikaranas.h"
#include "tinanta/lakara.h"
#include "test_paths.h"
#include <stdlib.h>
#include <string.h>

void setUp(void) {}
void tearDown(void) {}

static const char *data_file(const char *name) {
  static char path[256];
  return test_resolve_data_file(name, path, sizeof(path));
}

static const char *data_dir(void) {
  static char dir[256];
  return test_resolve_data_dir(dir, sizeof(dir));
}

static void count_range_cb(const Sutra *s, void *ud) {
  int *count = (int *)ud;
  if (s && count) (*count)++;
}

void test_sutra_loader_api_branches(void) {
  SutraDB db = {0};
  const Sutra *out[8] = {0};
  char buf[256];
  int count = 0;
  int n;
  FILE *tmp;

  TEST_ASSERT_EQUAL_INT(0, sutra_db_load(&db, data_file("sutras.tsv")));
  TEST_ASSERT_NULL(sutra_get_by_id(&db, 0));
  TEST_ASSERT_NULL(sutra_get_by_id(&db, db.count + 1));
  TEST_ASSERT_NULL(sutra_get_by_addr(&db, 0, 1, 1));
  TEST_ASSERT_NULL(sutra_get_by_addr(&db, 1, 0, 1));
  TEST_ASSERT_NULL(sutra_get_by_addr(&db, 1, 1, 0));
  TEST_ASSERT_NOT_NULL(sutra_get_by_addr(&db, 1, 1, 1));

  sutra_foreach_range(&db, 1, 3, count_range_cb, &count);
  TEST_ASSERT_EQUAL_INT(3, count);

  n = sutra_filter_by_type(&db, ASH_SUTRA_SAMJNA, out, 8);
  TEST_ASSERT_GREATER_THAN_INT(0, n);

  tmp = tmpfile();
  TEST_ASSERT_NOT_NULL(tmp);
  sutra_print(&db.sutras[0], tmp);
  rewind(tmp);
  memset(buf, 0, sizeof(buf));
  TEST_ASSERT_NOT_NULL(fgets(buf, sizeof(buf), tmp));
  TEST_ASSERT_NOT_NULL(strstr(buf, "1.1.1"));
  fclose(tmp);

  sutra_db_free(&db);
}

void test_conflict_and_term_branches(void) {
  ConflictCandidate cands[3] = {0};
  ConflictCandidate built;
  Term t = {0};
  char final_c;
  char penult_c;

  TEST_ASSERT_EQUAL_INT(-1, conflict_resolve(NULL, cands, 0, NULL));
  cands[0] = (ConflictCandidate){101, false, false, false, false};
  cands[1] = (ConflictCandidate){102, true, false, false, false};
  cands[2] = (ConflictCandidate){103, false, false, false, false};
  TEST_ASSERT_EQUAL_INT(1, conflict_resolve(NULL, cands, 3, NULL));
  cands[1].is_apavada = false;
  TEST_ASSERT_EQUAL_INT(2, conflict_resolve(NULL, cands, 3, NULL));
  built = conflict_candidate_build(NULL, 555);
  TEST_ASSERT_EQUAL_UINT32(555u, built.sutra_id);
  TEST_ASSERT_TRUE(conflict_is_apavada_of(NULL, 2, 1));
  TEST_ASSERT_FALSE(conflict_is_apavada_of(NULL, 1, 2));

  term_init(&t, "gam", SJ_DHATU);
  term_substitute(&t, "gata", 304077);
  TEST_ASSERT_EQUAL_UINT8(1, t.rule_count);
  TEST_ASSERT_EQUAL_UINT32(304077u, t.rule_history[0]);
  final_c = term_final(&t);
  penult_c = term_penultimate(&t);
  TEST_ASSERT_EQUAL_CHAR('a', final_c);
  TEST_ASSERT_EQUAL_CHAR('t', penult_c);
  TEST_ASSERT_TRUE(term_ends_vowel(&t));
  TEST_ASSERT_FALSE(term_begins_vowel(&t));

  memset(&t, 0, sizeof(t));
  TEST_ASSERT_EQUAL_CHAR(0, term_final(&t));
  TEST_ASSERT_EQUAL_CHAR(0, term_penultimate(&t));
}

void test_adhikara_pipeline_and_public_api_branches(void) {
  ASH_DB *db;
  ASH_Form forms18[18];
  ASH_Form forms24[24];
  ASH_Form paradigms[18];
  ASH_Form f;
  const ASH_Sutra *s;
  FILE *tmp;
  char line[256];
  uint32_t parents[4] = {0};
  SutraDB sdb = {0};
  int parent_count;

  db = ash_db_load(data_dir());
  TEST_ASSERT_NOT_NULL(db);

  s = ash_sutra_by_addr(db, 1, 1, 1);
  TEST_ASSERT_NOT_NULL(s);
  TEST_ASSERT_NOT_NULL(ash_sutra_by_id(db, s->global_id));
  TEST_ASSERT_NULL(ash_sutra_by_id(db, 0));
  TEST_ASSERT_NULL(ash_sutra_by_addr(NULL, 1, 1, 1));

  tmp = tmpfile();
  TEST_ASSERT_NOT_NULL(tmp);
  ash_sutra_print(s, db, tmp);
  rewind(tmp);
  memset(line, 0, sizeof(line));
  TEST_ASSERT_NOT_NULL(fgets(line, sizeof(line), tmp));
  TEST_ASSERT_NOT_NULL(strstr(line, "1.1."));
  fclose(tmp);

  ash_tinanta_paradigm(db, "BU", 1, ASH_LAT, forms18);
  TEST_ASSERT_TRUE(forms18[0].valid);
  TEST_ASSERT_TRUE(forms18[17].valid);
  for (int i = 0; i < 18; i++) ash_form_free(&forms18[i]);

  ash_subanta_paradigm(db, "rAma", ASH_PUMS, forms24);
  TEST_ASSERT_TRUE(forms24[0].valid);
  TEST_ASSERT_TRUE(forms24[23].valid);
  for (int i = 0; i < 24; i++) ash_form_free(&forms24[i]);

  f = ash_subanta(db, "rAmA", ASH_STRI, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = ash_subanta(db, "kvi", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = ash_subanta(db, "mDu", ASH_NAPUMSAKA, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = ash_subanta(db, "rAjn", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = ash_subanta(db, "mns", ASH_NAPUMSAKA, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = ash_subanta(db, "pitf", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = ash_tinanta(db, "", 1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.error, "empty root"));
  ash_form_free(&f);

  f = ash_tinanta(db, "BU", 1, ASH_LIT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.error, "only LAT"));
  ash_form_free(&f);

  f = ash_subanta(db, "", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);

  f = ash_tinanta(db, "BU", 1, ASH_LAT, (ASH_Purusha)99, ASH_EKAVACANA, ASH_PARASMAI);
  TEST_ASSERT_FALSE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.error, "lat derivation failed"));
  ash_form_free(&f);

  f = ash_subanta(db, "unsupported", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);

  f = ash_krit(db, "pat", 1, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = ash_krit(NULL, "gam", 1, ASH_KRIT_KTA);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);

  f = ash_samasa(db, "rAja", "puruza", ASH_SAMASA_TATPURUSHA, ASH_PUMS);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = ash_samasa(NULL, "rAja", "puruza", ASH_SAMASA_TATPURUSHA, ASH_PUMS);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);

  f = ash_subanta(db, "rAmA", ASH_STRI, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = ash_subanta(db, "kvi", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = ash_subanta(db, "mDu", ASH_NAPUMSAKA, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  f = ash_subanta(db, "rAjn", ASH_PUMS, ASH_DVITIYA_VIB, ASH_BAHUVACANA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("rAjYH", f.slp1);
  ash_form_free(&f);

  f = ash_subanta(db, "mns", ASH_NAPUMSAKA, ASH_CATURTHI_VIB, ASH_DVIVACANA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("mns", f.slp1);
  ash_form_free(&f);

  f = ash_subanta(db, "pitf", ASH_PUMS, ASH_CATURTHI_VIB, ASH_DVIVACANA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("pitf", f.slp1);
  ash_form_free(&f);

  TEST_ASSERT_EQUAL_INT(0, sutra_db_load(&sdb, data_file("sutras.tsv")));
  TEST_ASSERT_GREATER_THAN_INT(0, adhikara_build(&sdb, NULL));
  TEST_ASSERT_FALSE(adhikara_governs(NULL, 1, 2));
  TEST_ASSERT_FALSE(adhikara_governs(&sdb, 1, 0));
  TEST_ASSERT_EQUAL_UINT32(0u, adhikara_parent_of(NULL, 1));
  parent_count = adhikara_parents_of(&sdb, 10, parents, 4);
  TEST_ASSERT_GREATER_OR_EQUAL_INT(0, parent_count);
  TEST_ASSERT_EQUAL_INT(0, adhikara_parents_of(NULL, 10, parents, 4));
  TEST_ASSERT_EQUAL_INT(0, adhikara_parents_of(&sdb, 10, NULL, 4));
  TEST_ASSERT_EQUAL_INT(0, adhikara_parents_of(&sdb, 10, parents, 0));
  sutra_db_free(&sdb);

  ash_tinanta_paradigm(NULL, "BU", 1, ASH_LAT, paradigms);
  ash_subanta_paradigm(NULL, "rAma", ASH_PUMS, forms24);

  ash_db_free(db);
}

void test_encoding_branch_paths(void) {
  char *r;
  char out[4];

  r = enc_slp1_to_iast(NULL);
  TEST_ASSERT_NULL(r);
  r = enc_slp1_to_hk(NULL);
  TEST_ASSERT_NULL(r);
  r = enc_iast_to_slp1(NULL);
  TEST_ASSERT_NULL(r);
  r = enc_hk_to_slp1(NULL);
  TEST_ASSERT_NULL(r);
  r = enc_devanagari_to_slp1(NULL);
  TEST_ASSERT_NULL(r);

  r = enc_slp1_to_iast("a?");
  TEST_ASSERT_EQUAL_STRING("a?", r);
  free(r);

  r = enc_slp1_to_hk("Bavati!");
  TEST_ASSERT_EQUAL_STRING("bhavati!", r);
  free(r);

  r = enc_iast_to_slp1("x%");
  TEST_ASSERT_EQUAL_STRING("x%", r);
  free(r);

  r = enc_hk_to_slp1("A%");
  TEST_ASSERT_EQUAL_STRING("A%", r);
  free(r);

  r = enc_devanagari_to_slp1("राम");
  TEST_ASSERT_NOT_NULL(r);
  free(r);

  r = ash_encode("rAma", ASH_ENC_SLP1, ASH_ENC_SLP1);
  TEST_ASSERT_EQUAL_STRING("rAma", r);
  free(r);

  r = ash_encode("rAma", ASH_ENC_SLP1, ASH_ENC_HK);
  TEST_ASSERT_EQUAL_STRING("rAma", r);
  free(r);

  r = ash_encode("rAma", ASH_ENC_HK, ASH_ENC_SLP1);
  TEST_ASSERT_EQUAL_STRING("rAma", r);
  free(r);

  r = ash_encode("rAma", ASH_ENC_SLP1, ASH_ENC_DEVANAGARI);
  TEST_ASSERT_NOT_NULL(r);
  free(r);

  TEST_ASSERT_EQUAL_INT(1, encoding_convert_buf("a", ASH_ENC_SLP1, ASH_ENC_IAST, out, sizeof(out)));
  TEST_ASSERT_EQUAL_INT(-1, encoding_convert_buf(NULL, ASH_ENC_SLP1, ASH_ENC_IAST, out, sizeof(out)));
}

void test_sandhi_branch_paths(void) {
  SandhiResult r;
  char out[4];
  char joined[64];

  r = sandhi_vowel_apply('k', 'a');
  TEST_ASSERT_FALSE(r.changed);
  TEST_ASSERT_EQUAL_STRING("ka", r.result_slp1);
  TEST_ASSERT_EQUAL_INT(0, sandhi_vowel_split('a', NULL, 0));
  TEST_ASSERT_TRUE(sandhi_vowel_join("a", "a", out, sizeof(out)));
  TEST_ASSERT_FALSE(sandhi_vowel_join("a", "", joined, sizeof(joined)));

  r = sandhi_hal_apply('s', 'm');
  TEST_ASSERT_FALSE(r.changed);
  TEST_ASSERT_EQUAL_STRING("sm", r.result_slp1);
  TEST_ASSERT_TRUE(sandhi_hal_join("g", "t", out, sizeof(out)));
  TEST_ASSERT_FALSE(sandhi_hal_join("", "ga", joined, sizeof(joined)));

  r = sandhi_visarga_apply('u', 'k');
  TEST_ASSERT_FALSE(r.changed);
  TEST_ASSERT_EQUAL_STRING("Hk", r.result_slp1);
}

void test_krit_taddhita_and_stem_branch_paths(void) {
  ASH_Form f;
  PrakriyaCtx ctx = {0};
  char out[64];
  FILE *tmpf;

  TEST_ASSERT_FALSE(krit_needs_it_augment(NULL, ASH_KRIT_KTA));
  TEST_ASSERT_FALSE(krit_needs_it_augment("gam", ASH_KRIT_SHATR));
  TEST_ASSERT_TRUE(krit_needs_it_augment("gam", ASH_KRIT_KTA));

  f = krit_derive(NULL, 1, ASH_KRIT_KTA);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);

  f = krit_derive("BU", 1, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("BUta", f.slp1);
  ash_form_free(&f);

  f = krit_derive("pat", 1, ASH_KRIT_SHANAC);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.slp1, "SAnac"));
  ash_form_free(&f);

  f = krit_derive("pat", 1, ASH_KRIT_ANIIYA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.slp1, "anIya"));
  ash_form_free(&f);

  f = krit_derive("pat", 1, ASH_KRIT_YA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.slp1, "ya"));
  ash_form_free(&f);

  f = krit_derive("pat", 1, ASH_KRIT_LYAP);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.slp1, "lyap"));
  ash_form_free(&f);

  f = krit_derive_with_prefix("gam", 1, ASH_KRIT_KTVA, "upa");
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.slp1, "upa"));
  ash_form_free(&f);

  f = taddhita_derive(NULL, "", TD_AN);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);

  f = taddhita_derive(NULL, "garga", (TaddhitaType)999);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);

  f = taddhita_derive(NULL, "Dana", TD_IN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("Danin", f.slp1);
  ash_form_free(&f);

  f = taddhita_derive(NULL, "laghu", TD_TARA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("laGutara", f.slp1);
  ash_form_free(&f);

  f = taddhita_derive(NULL, "indra", TD_AN);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_NOT_NULL(strstr(f.slp1, "ya"));
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_YA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_KA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_IKA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_IYA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_TAMA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_MAYA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_AANA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  f = taddhita_derive(NULL, "deva", TD_VYA);
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);

  TEST_ASSERT_EQUAL_STRING("unknown", taddhita_type_name((TaddhitaType)999));
  TEST_ASSERT_FALSE(taddhita_causes_vrddhi((TaddhitaType)999));

  TEST_ASSERT_FALSE(aa_stem_fem_derive("rAmA", ASH_PRATHAMA_VIB, (ASH_Vacana)99, &ctx));
  TEST_ASSERT_FALSE(i_stem_derive("kvi", ASH_STRI, ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_FALSE(u_stem_derive("mDu", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_FALSE(consonant_stem_derive("x", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA, out, sizeof(out)));
  TEST_ASSERT_TRUE(r_stem_derive("pitf", ASH_SHASTHI_VIB, ASH_EKAVACANA, &ctx));
  TEST_ASSERT_EQUAL_STRING("pituH", ctx.terms[0].value);

  TEST_ASSERT_TRUE(a_stem_masc_can_handle("rAm"));
  TEST_ASSERT_FALSE(a_stem_masc_can_handle("rA"));

  tmpf = tmpfile();
  TEST_ASSERT_NOT_NULL(tmpf);
  ash_form_print_prakriya(NULL, tmpf);
  fclose(tmpf);
}

void test_guna_vikaranas_lakara_misc_branches(void) {
  Term anga = {0};
  Term pratyaya = {0};
  uint32_t rule = 0;
  char vbuf[2];
  char out[8] = {0};
  Samjna s = SJ_DHATU;

  memset(&anga, 0, sizeof(anga));
  memset(&pratyaya, 0, sizeof(pratyaya));
  strncpy(anga.value, "pit", sizeof(anga.value) - 1);
  pratyaya.samjna = SJ_PRATYAYA;
  TEST_ASSERT_TRUE(guna_apply_to_upadha(&anga, &pratyaya, &rule));
  TEST_ASSERT_EQUAL_UINT32(703086u, rule);

  term_init(&anga, "k", SJ_ANGA);
  TEST_ASSERT_FALSE(guna_apply_to_upadha(&anga, &pratyaya, &rule));

  s = samjna_remove(s, SJ_DHATU);
  TEST_ASSERT_FALSE(samjna_has(s, SJ_DHATU));
  TEST_ASSERT_NOT_NULL(samjna_describe(SJ_DHATU | SJ_TING));
  TEST_ASSERT_NOT_EQUAL(0u, samjna_of_sutra(10101));
  TEST_ASSERT_EQUAL_UINT64(0u, samjna_of_sutra(999999));

  TEST_ASSERT_FALSE(vikarana_for_gana(1, NULL, 0));
  TEST_ASSERT_TRUE(vikarana_for_gana(1, out, sizeof(out)));
  TEST_ASSERT_EQUAL_STRING("a", out);
  TEST_ASSERT_TRUE(vikarana_for_gana(10, out, sizeof(out)));
  TEST_ASSERT_EQUAL_STRING("aya", out);
  TEST_ASSERT_TRUE(gana_uses_guna(1));
  TEST_ASSERT_FALSE(gana_uses_guna(4));
  TEST_ASSERT_TRUE(gana_uses_vrddhi(10));
  TEST_ASSERT_FALSE(gana_uses_vrddhi(6));

  TEST_ASSERT_NULL(ting_get(ASH_LIT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI));
  TEST_ASSERT_NULL(ting_get(ASH_LAT, (ASH_Purusha)99, ASH_EKAVACANA, ASH_PARASMAI));
  TEST_ASSERT_EQUAL_STRING("unknown", lakara_name((ASH_Lakara)99));
  TEST_ASSERT_FALSE(lakara_is_sarvadhatuka(ASH_LIT));

  TEST_ASSERT_NULL(paribhasha_get((ParibhashaId)0));
  TEST_ASSERT_TRUE(paribhasha_applies(PB_STHANIVAD, NULL));
  TEST_ASSERT_FALSE(paribhasha_sthanivad_applies("a", "i", 0));
  TEST_ASSERT_TRUE(paribhasha_sthanivad_applies("gam", "gacC", 0));

  TEST_ASSERT_NULL(pratyahara_get("zzz"));
  TEST_ASSERT_FALSE(pratyahara_contains("zzz", 'a'));
  TEST_ASSERT_NOT_NULL(pratyahara_all(NULL));
  TEST_ASSERT_NULL(ash_pratyahara_expand("ac"));
  TEST_ASSERT_TRUE(ash_in_pratyahara('a', "ac"));

  vbuf[0] = ash_guna('i');
  vbuf[1] = '\0';
  TEST_ASSERT_EQUAL_STRING("e", vbuf);
  vbuf[0] = ash_vrddhi('u');
  TEST_ASSERT_EQUAL_CHAR('O', vbuf[0]);
}

void test_context_print_and_misc_modules(void) {
  PrakriyaCtx ctx = {0};
  FILE *tmp;
  char line[256];
  ASH_Form f;
  DhatupathaDB dhdb = {0};
  GanapathaDB gdb = {0};
  UnadiDB udb = {0};

  prakriya_init_tinanta(&ctx, "BU", 1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  prakriya_log(&ctx, 301068, "kartari Sap");
  tmp = tmpfile();
  TEST_ASSERT_NOT_NULL(tmp);
  prakriya_print_trace(&ctx, tmp);
  rewind(tmp);
  memset(line, 0, sizeof(line));
  TEST_ASSERT_NOT_NULL(fgets(line, sizeof(line), tmp));
  TEST_ASSERT_NOT_NULL(strstr(line, "301068"));
  fclose(tmp);

  f = prakriya_build_result(NULL);
  TEST_ASSERT_FALSE(f.valid);

  TEST_ASSERT_EQUAL_INT(0, dhatupatha_db_load(&dhdb, data_file("dhatupatha.tsv")));
  TEST_ASSERT_NULL(dhatupatha_find(&dhdb, "xxx", 0));
  dhatupatha_db_free(&dhdb);

  TEST_ASSERT_EQUAL_INT(0, ganapatha_db_load(&gdb, data_file("ganapatha.tsv")));
  TEST_ASSERT_FALSE(ganapatha_has_member(&gdb, "srvAdiH", "missing"));
  {
    const GanapathaEntry *mah = ganapatha_find_group(&gdb, "mAheSvarasUtra");
    TEST_ASSERT_NOT_NULL(mah);
    TEST_ASSERT_EQUAL_STRING("mAheSvarasUtra", mah->gana_name_slp1);
  }
  ganapatha_db_free(&gdb);

  TEST_ASSERT_EQUAL_INT(0, unadi_db_load(&udb, data_file("unadipatha.tsv")));
  TEST_ASSERT_NULL(unadi_lookup(&udb, "nope", NULL));
  TEST_ASSERT_FALSE(unadi_is_attested(&udb, "unknown"));
  f = unadi_form(&udb, "nope", "x");
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);
  f = unadi_form(&udb, "jan", "u");
  TEST_ASSERT_TRUE(f.valid);
  ash_form_free(&f);
  unadi_db_free(&udb);

  TEST_ASSERT_EQUAL_INT(ASH_TRITIYA_VIB, karaka_default_vibhakti(ASH_KARANA));
  TEST_ASSERT_EQUAL_STRING("unknown", karaka_name((ASH_Karaka)99));
  TEST_ASSERT_EQUAL_STRING("unknown", vibhakti_name((ASH_Vibhakti)99));

  TEST_ASSERT_EQUAL_STRING("unknown", samasa_type_name((ASH_SamasaType)99));
  f = samasa_derive(NULL, NULL, "x", ASH_SAMASA_TATPURUSHA, ASH_PUMS, NULL);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);
  f = samasa_derive(NULL, "a", "b", (ASH_SamasaType)99, ASH_PUMS, NULL);
  TEST_ASSERT_FALSE(f.valid);
  ash_form_free(&f);
}

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_sutra_loader_api_branches);
  RUN_TEST(test_conflict_and_term_branches);
  RUN_TEST(test_adhikara_pipeline_and_public_api_branches);
  RUN_TEST(test_encoding_branch_paths);
  RUN_TEST(test_sandhi_branch_paths);
  RUN_TEST(test_krit_taddhita_and_stem_branch_paths);
  RUN_TEST(test_guna_vikaranas_lakara_misc_branches);
  RUN_TEST(test_context_print_and_misc_modules);
  return UNITY_END();
}
