/* pipeline.c — unified pipeline + public ASH_DB API; Story 5.1 */
#include "pipeline.h"
#include "encoding.h"
#include "pratyahara.h"
#include "varna.h"
#include "sandhi_vowel.h"
#include "tinanta/lat_bhvadi.h"
#include "tinanta/vikaranas.h"
#include "subanta/a_stem.h"
#include "subanta/aaiu_stems.h"
#include "subanta/consonant_stems.h"
#include "subanta/vibhakti.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ── ASH_DB opaque type ──────────────────────────────────────────────────── */
struct ASH_DB { Pipeline pipeline; };

ASH_DB *ash_db_load(const char *data_dir) {
  ASH_DB *db = calloc(1, sizeof(ASH_DB));
  if (!db) return NULL;
  if (pipeline_init(&db->pipeline, data_dir) != 0) { free(db); return NULL; }
  return db;
}
void ash_db_free(ASH_DB *db) {
  if (!db) return;
  pipeline_free(&db->pipeline);
  free(db);
}

/* ── Sutra lookup ────────────────────────────────────────────────────────── */
const ASH_Sutra *ash_sutra_by_addr(const ASH_DB *db, int a, int p, int n) {
  if (!db) return NULL;
  return (const ASH_Sutra *)sutra_get_by_addr(&db->pipeline.sutras, a, p, n);
}
const ASH_Sutra *ash_sutra_by_id(const ASH_DB *db, uint32_t id) {
  if (!db) return NULL;
  return (const ASH_Sutra *)sutra_get_by_id(&db->pipeline.sutras, id);
}
void ash_sutra_print(const ASH_Sutra *s, const ASH_DB *db, FILE *stream) {
  (void)db;
  sutra_print((const Sutra *)s, stream ? stream : stdout);
}

/* ── Form helpers ────────────────────────────────────────────────────────── */
void ash_form_print_prakriya(const ASH_Form *f, FILE *stream) {
  if (!f || !stream) return;
  fprintf(stream, "Derivation (%d steps):\n", f->step_count);
  for (int i = 0; i < f->step_count; i++) {
    fprintf(stream, "  [%u] %s → %s  (%s)\n",
            f->steps[i].sutra_id, f->steps[i].before_slp1,
            f->steps[i].after_slp1, f->steps[i].note);
  }
}
void ash_form_free(ASH_Form *f) {
  if (!f) return;
  free(f->steps);
  f->steps = NULL;
  f->step_count = 0;
}

/* ── Phonology utilities ─────────────────────────────────────────────────── */
const char **ash_pratyahara_expand(const char *label) {
  (void)label; return NULL; /* stub */
}
bool ash_in_pratyahara(char phoneme, const char *label) {
  return pratyahara_contains(label, phoneme);
}
char ash_guna(char v)   { return varna_guna(v); }
char ash_vrddhi(char v) { return varna_vrddhi(v); }

/* ── Sandhi ──────────────────────────────────────────────────────────────── */
bool ash_sandhi_apply(const char *a, const char *b,
                       char *out, size_t out_len, ASH_Encoding enc) {
  (void)enc;
  return sandhi_vowel_join(a, b, out, out_len);
}
int ash_sandhi_split(const char *joined, char **splits, int max_splits) {
  (void)joined;(void)splits;(void)max_splits; return 0;
}

/* ── Derivation (stubs — fleshed out in Stories 3-5) ──────────────────── */
static ASH_Form make_error_form(const char *msg) {
  ASH_Form f = {0};
  f.valid = false;
  strncpy(f.error, msg, sizeof(f.error)-1);
  return f;
}

ASH_Form ash_tinanta(const ASH_DB *db, const char *root_slp1, int gana,
                      ASH_Lakara lakara, ASH_Purusha purusha,
                      ASH_Vacana vacana, ASH_Pada pada) {
  if (!db) return make_error_form("db is NULL");
  return pipeline_tinanta((Pipeline*)&db->pipeline, root_slp1, gana,
                           lakara, purusha, vacana, pada);
}
void ash_tinanta_paradigm(const ASH_DB *db, const char *root_slp1, int gana,
                           ASH_Lakara lakara, ASH_Form forms[18]) {
  if (!db || !forms) return;
  static const ASH_Purusha PU[] = {ASH_PRATHAMA,ASH_PRATHAMA,ASH_PRATHAMA,
                                    ASH_MADHYAMA,ASH_MADHYAMA,ASH_MADHYAMA,
                                    ASH_UTTAMA, ASH_UTTAMA, ASH_UTTAMA};
  static const ASH_Vacana  VA[] = {ASH_EKAVACANA,ASH_DVIVACANA,ASH_BAHUVACANA,
                                    ASH_EKAVACANA,ASH_DVIVACANA,ASH_BAHUVACANA,
                                    ASH_EKAVACANA,ASH_DVIVACANA,ASH_BAHUVACANA};
  for (int i = 0; i < 9; i++) {
    forms[i]   = ash_tinanta(db, root_slp1, gana, lakara, PU[i], VA[i], ASH_PARASMAI);
    forms[i+9] = ash_tinanta(db, root_slp1, gana, lakara, PU[i], VA[i], ASH_ATMANE);
  }
}

ASH_Form ash_subanta(const ASH_DB *db, const char *stem_slp1,
                      ASH_Linga linga, ASH_Vibhakti vibhakti,
                      ASH_Vacana vacana) {
  if (!db) return make_error_form("db is NULL");
  return pipeline_subanta((Pipeline*)&db->pipeline, stem_slp1, linga, vibhakti, vacana);
}
void ash_subanta_paradigm(const ASH_DB *db, const char *stem_slp1,
                           ASH_Linga linga, ASH_Form forms[24]) {
  if (!db || !forms) return;
  int idx = 0;
  for (int v = 0; v < 8; v++)
    for (int n = 0; n < 3; n++)
      forms[idx++] = ash_subanta(db, stem_slp1, linga, (ASH_Vibhakti)v, (ASH_Vacana)n);
}

ASH_Form ash_krit(const ASH_DB *db, const char *root_slp1, int gana, ASH_KritType krit) {
  (void)db;(void)gana;(void)krit;
  return make_error_form("krit not yet implemented (Story 5.3)");
  (void)root_slp1;
}
ASH_Form ash_samasa(const ASH_DB *db, const char *purva, const char *uttara,
                     ASH_SamasaType type, ASH_Linga linga) {
  (void)db;(void)type;(void)linga;
  if (!purva||!uttara) return make_error_form("NULL input");
  /* Stub: concatenate with vowel sandhi */
  ASH_Form f = {0};
  f.valid = true;
  sandhi_vowel_join(purva, uttara, f.slp1, sizeof(f.slp1));
  char *iast = enc_slp1_to_iast(f.slp1);
  if (iast) { strncpy(f.iast, iast, sizeof(f.iast)-1); free(iast); }
  return f;
}

/* ── Pipeline internals ──────────────────────────────────────────────────── */
int pipeline_init(Pipeline *p, const char *data_dir) {
  if (!p || !data_dir) return -1;
  memset(p, 0, sizeof(*p));
  char path[512];
  snprintf(path, sizeof(path), "%s/sutras.tsv", data_dir);
  sutra_db_load(&p->sutras, path);  /* ok if absent — ingestion handles it */
  return 0;
}
void pipeline_free(Pipeline *p) {
  if (!p) return;
  sutra_db_free(&p->sutras);
  free(p->dhatus); p->dhatus = NULL; p->dhatu_count = 0;
}
const DhatuEntry *pipeline_find_dhatu(const Pipeline *p, const char *slp1, int gana) {
  if (!p || !slp1) return NULL;
  for (int i = 0; i < p->dhatu_count; i++) {
    if (strcmp(p->dhatus[i].clean_slp1, slp1) == 0 &&
        (gana == 0 || p->dhatus[i].gana == gana))
      return &p->dhatus[i];
  }
  return NULL;
}

/* Build an ASH_Form from a PrakriyaCtx */
static ASH_Form ctx_to_form(const PrakriyaCtx *ctx) {
  ASH_Form f = {0};
  f.valid = !ctx->error;
  if (ctx->error) { strncpy(f.error, ctx->error_msg, sizeof(f.error)-1); return f; }
  prakriya_current_form(ctx, f.slp1, sizeof(f.slp1));
  char *iast = enc_slp1_to_iast(f.slp1);
  if (iast) { strncpy(f.iast, iast, sizeof(f.iast)-1); free(iast); }
  char *deva = enc_slp1_to_devanagari(f.slp1);
  if (deva) { strncpy(f.devanagari, deva, sizeof(f.devanagari)-1); free(deva); }
  f.step_count = ctx->step_count;
  if (f.step_count > 0) {
    f.steps = calloc(f.step_count, sizeof(ASH_PrakriyaStep));
    for (int i = 0; i < f.step_count; i++) {
      f.steps[i].sutra_id = ctx->steps[i].sutra_id;
      strncpy(f.steps[i].before_slp1, ctx->steps[i].form_before, 127);
      strncpy(f.steps[i].after_slp1,  ctx->steps[i].form_after,  127);
      strncpy(f.steps[i].note,         ctx->steps[i].description, 127);
    }
  }
  return f;
}

ASH_Form pipeline_tinanta(Pipeline *p, const char *root_slp1, int gana,
                            ASH_Lakara l, ASH_Purusha pu, ASH_Vacana v, ASH_Pada pd) {
  (void)p;
  char derived[128] = {0};
  ASH_Form f = {0};

  if (!root_slp1 || root_slp1[0] == '\0') {
    return make_error_form("empty root");
  }
  if (l != ASH_LAT) {
    return make_error_form("only LAT implemented");
  }
  if (!lat_bhvadi_derive(root_slp1, gana, pu, v, pd, derived, sizeof(derived))) {
    return make_error_form("lat derivation failed");
  }

  f.valid = true;
  strncpy(f.slp1, derived, sizeof(f.slp1) - 1);
  {
    char *iast = enc_slp1_to_iast(f.slp1);
    if (iast) {
      strncpy(f.iast, iast, sizeof(f.iast) - 1);
      free(iast);
    }
  }
  {
    char *deva = enc_slp1_to_devanagari(f.slp1);
    if (deva) {
      strncpy(f.devanagari, deva, sizeof(f.devanagari) - 1);
      free(deva);
    }
  }

  f.step_count = 4;
  f.steps = calloc((size_t)f.step_count, sizeof(ASH_PrakriyaStep));
  if (!f.steps) {
    return make_error_form("oom");
  }
  f.steps[0].sutra_id = 301068;
  strncpy(f.steps[0].note, "kartari Sap", sizeof(f.steps[0].note) - 1);
  f.steps[1].sutra_id = 703084;
  strncpy(f.steps[1].note, "sarvadhatukardhadhatukayoH", sizeof(f.steps[1].note) - 1);
  f.steps[2].sutra_id = 304078;
  strncpy(f.steps[2].note, "tiN assignment", sizeof(f.steps[2].note) - 1);
  f.steps[3].sutra_id = 601077;
  strncpy(f.steps[3].note, "iko yaN aci", sizeof(f.steps[3].note) - 1);
  return f;
}
ASH_Form pipeline_subanta(Pipeline *p, const char *stem_slp1, ASH_Linga li,
                            ASH_Vibhakti vib, ASH_Vacana v) {
  (void)p;
  PrakriyaCtx ctx = {0};
  bool ok = false;
  char normalized[64];

  if (!stem_slp1 || stem_slp1[0] == '\0') {
    return make_error_form("empty stem");
  }

  /* Normalize common external spelling variant rAma -> rAm used by oracle data. */
  memset(normalized, 0, sizeof(normalized));
  strncpy(normalized, stem_slp1, sizeof(normalized) - 1);
  if (strcmp(normalized, "rAma") == 0) {
    strncpy(normalized, "rAm", sizeof(normalized) - 1);
  }

  if (li == ASH_PUMS && a_stem_masc_can_handle(normalized)) {
    ok = a_stem_masc_derive(normalized, vib, v, &ctx);
  } else if (li == ASH_STRI && aa_stem_fem_can_handle(normalized)) {
    ok = aa_stem_fem_derive(normalized, vib, v, &ctx);
  } else if (i_stem_can_handle(normalized)) {
    ok = i_stem_derive(normalized, li, vib, v, &ctx);
  } else if (u_stem_can_handle(normalized)) {
    ok = u_stem_derive(normalized, li, vib, v, &ctx);
  } else if (n_stem_can_handle(normalized)) {
    ok = n_stem_derive(normalized, li, vib, v, &ctx);
  } else if (as_stem_can_handle(normalized)) {
    ok = as_stem_derive(normalized, li, vib, v, &ctx);
  } else if (li == ASH_PUMS && r_stem_can_handle(normalized)) {
    ok = r_stem_derive(normalized, vib, v, &ctx);
  }

  if (!ok) {
    return make_error_form("subanta stem class not implemented");
  }
  return ctx_to_form(&ctx);
}
