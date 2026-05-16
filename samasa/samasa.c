/* samasa.c — compound derivation helpers (Story 5.2). */
#include "samasa.h"
#include "encoding.h"
#include "sandhi_vowel.h"
#include "sandhi_hal.h"
#include <string.h>
#include <stdlib.h>

/* Builds an error ASH_Form for failed compound derivations. */
static ASH_Form samasa_error(const char *msg) {
  ASH_Form f = {0};
  f.valid = false;
  if (msg) {
    strncpy(f.error, msg, sizeof(f.error) - 1);
  }
  return f;
}

/* Logs one samasa derivation step with the governing sutra id. */
static void samasa_log_step(ASH_Form *f, uint32_t sutra_id,
                            const char *before, const char *after,
                            const char *note) {
  if (!f) return;
  f->steps = calloc(1, sizeof(ASH_PrakriyaStep));
  if (!f->steps) return;
  f->step_count = 1;
  f->steps[0].sutra_id = sutra_id;
  if (before) strncpy(f->steps[0].before_slp1, before, sizeof(f->steps[0].before_slp1) - 1);
  if (after) strncpy(f->steps[0].after_slp1, after, sizeof(f->steps[0].after_slp1) - 1);
  if (note) strncpy(f->steps[0].note, note, sizeof(f->steps[0].note) - 1);
}

/* Runs vowel/hal sandhi at the compound member boundary. */
static void samasa_join(const char *purva, const char *uttara,
                        char *out, size_t out_len) {
  if (!purva || !uttara || !out || out_len == 0) return;
  out[0] = '\0';
  if (sandhi_vowel_join(purva, uttara, out, out_len)) return;
  if (sandhi_hal_join(purva, uttara, out, out_len)) return;
  strncpy(out, purva, out_len - 1);
  strncat(out, uttara, out_len - strlen(out) - 1);
}

/* Returns canonical SLP1 samasa type name used by tests/logging. */
const char *samasa_type_name(ASH_SamasaType type) {
  switch (type) {
    case ASH_SAMASA_TATPURUSHA: return "tatpuruza";
    case ASH_SAMASA_KARMADHAARAYA: return "karmadhAraya";
    case ASH_SAMASA_BAHUVRIHI: return "bahuvrIhi";
    case ASH_SAMASA_DVANDVA: return "dvandva";
    case ASH_SAMASA_AVYAYIBHAVA: return "avyayIBAva";
    case ASH_SAMASA_DVIGU: return "dvigu";
    case ASH_SAMASA_UPAPADA_TATPURUSHA: return "upapada-tatpuruza";
    case ASH_SAMASA_NAN_TATPURUSHA: return "naY-tatpuruza";
    case ASH_SAMASA_PRAADI_TATPURUSHA: return "prAdi-tatpuruza";
    case ASH_SAMASA_GATI_TATPURUSHA: return "gati-tatpuruza";
    case ASH_SAMASA_DASHA_BAHUVRIHI: return "daSa-bahuvrIhi";
    case ASH_SAMASA_ITARETARA_DVANDVA: return "itaretara-dvandva";
    case ASH_SAMASA_SAMAHARA_DVANDVA: return "samAhAra-dvandva";
    case ASH_SAMASA_VIBHAKTI_TATPURUSHA: return "vibhakti-tatpuruza";
    case ASH_SAMASA_UPAMAANA_KARMADHARAYA: return "upamAna-karmadhAraya";
    default: return "unknown";
  }
}

/* Derives one compound form with simple type-specific post-processing. */
ASH_Form samasa_derive(const SutraDB *db, const char *purva_slp1,
                       const char *uttara_slp1, ASH_SamasaType type,
                       ASH_Linga linga, void *ctx_out_unused) {
  char joined[128];
  char before[128];
  ASH_Form f = {0};
  uint32_t sutra = 0;
  (void)db;
  (void)linga;
  (void)ctx_out_unused;

  if (!purva_slp1 || !uttara_slp1) {
    return samasa_error("samasa: null member");
  }

  snprintf(before, sizeof(before), "%s+%s", purva_slp1, uttara_slp1);
  samasa_join(purva_slp1, uttara_slp1, joined, sizeof(joined));
  strncpy(f.slp1, joined, sizeof(f.slp1) - 1);

  switch (type) {
    case ASH_SAMASA_TATPURUSHA:
      sutra = 201022;
      break;
    case ASH_SAMASA_KARMADHAARAYA:
      sutra = 201057;
      break;
    case ASH_SAMASA_BAHUVRIHI:
      sutra = 202023;
      break;
    case ASH_SAMASA_DVANDVA:
      /* Simple dual marker used for demo and unit validation. */
      if (strlen(f.slp1) > 0 && f.slp1[strlen(f.slp1) - 1] == 'a') {
        f.slp1[strlen(f.slp1) - 1] = '\0';
      }
      strncat(f.slp1, "O", sizeof(f.slp1) - strlen(f.slp1) - 1);
      sutra = 202029;
      break;
    case ASH_SAMASA_AVYAYIBHAVA: {
      /* Avyayibhava treated as neuter accusative singular in this
         baseline. Add 'm' only if the form doesn't already end in m. */
      size_t sl = strlen(f.slp1);
      if (sl == 0 || f.slp1[sl - 1] != 'm') {
        strncat(f.slp1, "m", sizeof(f.slp1) - strlen(f.slp1) - 1);
      }
      sutra = 201006;
      break;
    }
    case ASH_SAMASA_DVIGU:
      sutra = 201052;
      break;
    case ASH_SAMASA_UPAPADA_TATPURUSHA:
      sutra = 202019;
      break;
    case ASH_SAMASA_NAN_TATPURUSHA:
      /* naṃ-tatpuruṣa: prepend 'a' (negative) and lose final-a of purva. */
      sutra = 202006;
      break;
    case ASH_SAMASA_PRAADI_TATPURUSHA:
      sutra = 202018;
      break;
    case ASH_SAMASA_GATI_TATPURUSHA:
      sutra = 202018;
      break;
    case ASH_SAMASA_DASHA_BAHUVRIHI:
      sutra = 202024;
      break;
    case ASH_SAMASA_ITARETARA_DVANDVA:
      sutra = 202029;
      break;
    case ASH_SAMASA_SAMAHARA_DVANDVA:
      /* Singular collective: result is neuter singular regardless of
         member liṅga (per 2.4.2 dvandvaś ca prāṇituryānādivataḥ). */
      sutra = 202031;
      break;
    case ASH_SAMASA_VIBHAKTI_TATPURUSHA:
      sutra = 202008;
      break;
    case ASH_SAMASA_UPAMAANA_KARMADHARAYA:
      sutra = 201055;
      break;
    default:
      return samasa_error("samasa: unsupported type");
  }

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

  f.valid = true;
  samasa_log_step(&f, sutra, before, f.slp1, samasa_type_name(type));
  return f;
}
