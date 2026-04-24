/* context.h — derivation context; Story 3.1 */
#ifndef PRAKRIYA_CTX_H
#define PRAKRIYA_CTX_H
#include "term.h"
#include "ashtadhyayi.h"
#include <stdio.h>
#define MAX_TERMS          8
#define MAX_PRAKRIYA_STEPS 128
typedef struct {
  uint32_t sutra_id;
  char     description[128];
  char     form_before[128];
  char     form_after[128];
} PrakriyaStep;
typedef struct {
  Term         terms[MAX_TERMS];
  int          term_count;
  ASH_Lakara   lakara;
  ASH_Purusha  purusha;
  ASH_Vacana   vacana;
  ASH_Pada     pada;
  ASH_Linga    linga;
  ASH_Vibhakti vibhakti;
  int          gana;
  PrakriyaStep steps[MAX_PRAKRIYA_STEPS];
  int          step_count;
  bool         error;
  char         error_msg[256];
} PrakriyaCtx;
void prakriya_init_tinanta(PrakriyaCtx *ctx, const char *dhatu_slp1, int gana,
                            ASH_Lakara l, ASH_Purusha p, ASH_Vacana v, ASH_Pada pd);
void prakriya_init_subanta(PrakriyaCtx *ctx, const char *stem_slp1, ASH_Linga li,
                            ASH_Vibhakti vib, ASH_Vacana v);
void prakriya_log(PrakriyaCtx *ctx, uint32_t sutra_id, const char *desc);
void prakriya_current_form(const PrakriyaCtx *ctx, char *out, size_t len);
void prakriya_print_trace(const PrakriyaCtx *ctx, FILE *stream);
ASH_Form prakriya_build_result(const PrakriyaCtx *ctx);
#endif
