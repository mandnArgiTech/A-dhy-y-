/* context.c — derivation context; Story 3.1 */
#include "context.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

void prakriya_init_tinanta(PrakriyaCtx *ctx, const char *dhatu, int gana,
                            ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd) {
  if (!ctx) return;
  memset(ctx, 0, sizeof(*ctx));
  ctx->lakara = l; ctx->purusha = p; ctx->vacana = v; ctx->pada = pd; ctx->gana = gana;
  term_init(&ctx->terms[0], dhatu, SJ_DHATU);
  ctx->term_count = 1;
}
void prakriya_init_subanta(PrakriyaCtx *ctx, const char *stem, ASH_Linga li,
                            ASH_Vibhakti vib, ASH_Vacana v) {
  if (!ctx) return;
  memset(ctx, 0, sizeof(*ctx));
  ctx->linga = li; ctx->vibhakti = vib; ctx->vacana = v;
  term_init(&ctx->terms[0], stem, SJ_PRATIPADIKA);
  ctx->term_count = 1;
}
void prakriya_log(PrakriyaCtx *ctx, uint32_t sutra_id, const char *desc) {
  if (!ctx || ctx->step_count >= MAX_PRAKRIYA_STEPS) return;
  PrakriyaStep *s = &ctx->steps[ctx->step_count++];
  s->sutra_id = sutra_id;
  prakriya_current_form(ctx, s->form_before, sizeof(s->form_before));
  if (desc) strncpy(s->description, desc, sizeof(s->description)-1);
  prakriya_current_form(ctx, s->form_after, sizeof(s->form_after));
}
void prakriya_current_form(const PrakriyaCtx *ctx, char *out, size_t len) {
  if (!ctx || !out || len < 1) return;
  out[0] = '\0';
  size_t pos = 0;
  for (int i = 0; i < ctx->term_count && pos+1 < len; i++) {
    size_t n = strlen(ctx->terms[i].value);
    if (pos + n + 1 > len) break;
    memcpy(out + pos, ctx->terms[i].value, n);
    pos += n;
  }
  out[pos] = '\0';
}
void prakriya_print_trace(const PrakriyaCtx *ctx, FILE *f) {
  if (!ctx || !f) return;
  for (int i = 0; i < ctx->step_count; i++) {
    fprintf(f, "  [%u] %s\n", ctx->steps[i].sutra_id, ctx->steps[i].description);
  }
}
ASH_Form prakriya_build_result(const PrakriyaCtx *ctx) {
  ASH_Form f = {0};
  if (!ctx) { f.valid=false; return f; }
  f.valid = !ctx->error;
  if (ctx->error) { strncpy(f.error, ctx->error_msg, sizeof(f.error)-1); return f; }
  prakriya_current_form(ctx, f.slp1, sizeof(f.slp1));
  return f;
}
