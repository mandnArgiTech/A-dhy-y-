# 10 — User Manual and API Integration

This chapter explains how to run and integrate the exposed API and CLI entrypoints in this repository.

Related:
- Public architecture and value: [01_README_Vision_and_Value.md](01_README_Vision_and_Value.md)
- Developer workflow: [08_Developer_Build_and_Contribution_Guide.md](08_Developer_Build_and_Contribution_Guide.md)

---

## 1) Public API surface

Primary API header:

- `include/ashtadhyayi.h`

Major public groups:

- DB lifecycle: `ash_db_load`, `ash_db_free`
- Sūtra lookup: `ash_sutra_by_addr`, `ash_sutra_by_id`, `ash_sutra_print`
- Derivation:
  - `ash_tinanta`, `ash_tinanta_paradigm`
  - `ash_subanta`, `ash_subanta_paradigm`
  - `ash_krit`
  - `ash_samasa`
- Sandhi: `ash_sandhi_apply`, `ash_sandhi_split`
- Phonology utilities: `ash_pratyahara_expand`, `ash_in_pratyahara`, `ash_guna`, `ash_vrddhi`
- Encoding conversion: `ash_encode`
- Result handling: `ash_form_print_prakriya`, `ash_form_free`

---

## 2) Minimal integration pattern

```cpp
#include "ashtadhyayi.h"
#include <stdio.h>

int main() {
  ASH_DB *db = ash_db_load("data/");
  if (!db) return 1;

  ASH_Form f = ash_tinanta(
    db, "BU", 1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI
  );

  if (f.valid) {
    printf("SLP1: %s\n", f.slp1);
    printf("IAST: %s\n", f.iast);
    printf("DEVA: %s\n", f.devanagari);
    ash_form_print_prakriya(&f, stdout);
  } else {
    printf("ERROR: %s\n", f.error);
  }

  ash_form_free(&f);
  ash_db_free(db);
  return 0;
}
```

---

## 3) Concrete I/O examples

## 3.1 Tinanta

Input:

- root: `BU`
- gana: `1`
- lakara: `ASH_LAT`
- purusha: `ASH_PRATHAMA`
- vacana: `ASH_EKAVACANA`
- pada: `ASH_PARASMAI`

API call:

```cpp
ASH_Form f = ash_tinanta(db, "BU", 1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
```

Output fields:

- `f.slp1` (example shape: conjugated SLP1 form),
- `f.iast` (Unicode transliteration),
- `f.devanagari` (Unicode Devanāgarī),
- `f.steps` (rule-trace rows).

## 3.2 Subanta

Input:

- stem: `rAma`
- linga: `ASH_PUMS`
- vibhakti: `ASH_PRATHAMA_VIB`
- vacana: `ASH_EKAVACANA`

Call:

```cpp
ASH_Form f = ash_subanta(db, "rAma", ASH_PUMS, ASH_PRATHAMA_VIB, ASH_EKAVACANA);
```

## 3.3 Sandhi

```cpp
char out[256];
bool changed = ash_sandhi_apply("rAmaH", "asti", out, sizeof(out), ASH_ENC_IAST);
```

- `out` contains encoded result.
- Return value indicates whether a sandhi transformation changed the boundary.

## 3.4 Samāsa

```cpp
ASH_Form f = ash_samasa(db, "rAja", "puruzwa", ASH_SAMASA_TATPURUSHA, ASH_PUMS);
```

Returns one compound form with one logged samāsa step in current implementation.

---

## 4) CLI manual (`ash_demo`)

Built executable:

- `build/ash_demo`

Modes:

1. No args: run showcase demos.
2. Tinanta mode:
   - `ash_demo tinanta ROOT GANA LAKARA PURUSHA VACANA PADA`
3. Subanta mode:
   - `ash_demo subanta STEM LINGA VIBHAKTI VACANA`

Examples:

```bash
./build/ash_demo
./build/ash_demo tinanta BU 1 LAT PRATHAMA EKAVACANA PARASMAI
./build/ash_demo subanta rAma PUMS PRATHAMA EKAVACANA
```

If invalid arguments are supplied, demo prints usage and exits nonzero.

---

## 5) Encoding integration guidance

Internal processing is SLP1-centric.  
Use `ash_encode()` at boundaries:

```cpp
char *iast = ash_encode("rAma", ASH_ENC_SLP1, ASH_ENC_IAST);
// free(iast) after use
```

Supported enums:

- `ASH_ENC_SLP1`
- `ASH_ENC_IAST`
- `ASH_ENC_DEVANAGARI`
- `ASH_ENC_HK`

---

## 6) Error handling contract

For derivation APIs:

- check `ASH_Form.valid`,
- consume `ASH_Form.error` when invalid,
- always call `ash_form_free()` for cleanup.

For DB:

- `ash_db_load()` returns `NULL` on failure (e.g., missing data path or allocation failure).

---

## 7) Integration checklist (application developers)

1. Generate/load `data/*.tsv`.
2. Initialize DB once.
3. Route requests to relevant `ash_*` API.
4. Render desired output encoding (`slp1`, `iast`, or `devanagari`).
5. Log or expose `steps` for explainability where needed.
6. Free form and DB resources correctly.

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
