/* pipeline.h — unified derivation pipeline; Story 5.1 */
#ifndef PIPELINE_H
#define PIPELINE_H
#include "ashtadhyayi.h"
#include "context.h"
#include "sutra.h"
#include "unadipatha/unadi.h"
typedef struct {
  char  upadesa_slp1[64];
  char  clean_slp1[32];
  int   gana;
  char  pada_flag;
  char  meaning_en[128];
} DhatuEntry;
typedef struct {
  SutraDB    sutras;
  DhatuEntry *dhatus;
  int         dhatu_count;
  UnadiDB     unadi_db;
} Pipeline;
int       pipeline_init(Pipeline *p, const char *data_dir);
void      pipeline_free(Pipeline *p);
const DhatuEntry *pipeline_find_dhatu(const Pipeline *p, const char *clean_slp1, int gana_hint);
ASH_Form  pipeline_tinanta(Pipeline *p, const char *root_slp1, int gana,
                            ASH_Lakara l, ASH_Purusha pu, ASH_Vacana v, ASH_Pada pd);
ASH_Form  pipeline_subanta(Pipeline *p, const char *stem_slp1, ASH_Linga li,
                            ASH_Vibhakti vib, ASH_Vacana v);
#endif
