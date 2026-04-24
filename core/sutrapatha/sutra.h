/* sutra.h — sutra data model; implemented in Story 2.1 */
#ifndef SUTRA_H
#define SUTRA_H
#include "ashtadhyayi.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#define SUTRA_TEXT_LEN   256
#define SUTRA_MAX_COUNT  4100
typedef struct {
  uint16_t      adhyaya;
  uint8_t       pada;
  uint16_t      num;
  uint32_t      global_id;
  char          text_slp1[SUTRA_TEXT_LEN];
  ASH_SutraType type;
  uint32_t      adhikara_parent;
  uint32_t      anuvrtti_from;
} Sutra;
typedef struct {
  Sutra   *sutras;
  uint32_t count;
  uint32_t addr_index[9][5][250]; /* [adhyaya][pada][num] → global_id */
} SutraDB;
int          sutra_db_load(SutraDB *db, const char *tsv_path);
void         sutra_db_free(SutraDB *db);
const Sutra *sutra_get_by_id(const SutraDB *db, uint32_t global_id);
const Sutra *sutra_get_by_addr(const SutraDB *db, int adhyaya, int pada, int num);
void         sutra_print(const Sutra *s, FILE *stream);
typedef void (*SutraVisitor)(const Sutra *s, void *userdata);
void sutra_foreach_range(const SutraDB *db, uint32_t from_id, uint32_t to_id,
                          SutraVisitor fn, void *userdata);
int  sutra_filter_by_type(const SutraDB *db, ASH_SutraType type,
                           const Sutra **out, int max_out);
#endif
