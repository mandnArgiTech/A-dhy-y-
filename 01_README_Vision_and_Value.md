# 01 — Vision and Value (Master Index)

This repository implements a **deterministic Paninian grammar engine** as a modular systems library in C (C17), with a C-compatible public API (`include/ashtadhyayi.h`) and C++ interoperability via `extern "C"`.

If you master this architecture, you can build:

- A Sanskrit morphology microservice (tinanta/subanta derivation over API calls).
- A deterministic linguistic rule engine with traceable rule IDs.
- A phonology/sandhi transformation pipeline for NLP preprocessing.
- A grammar-aware CLI toolchain for lexical generation and validation.
- A regression-validated linguistic platform grounded in TSV corpora and phase gates.

---

## Concrete capabilities extracted from this codebase

Current engine capabilities (as implemented, not hypothetical):

- Data loading from TSV (`sutras.tsv`, `dhatupatha.tsv`) into runtime structures.
- Sūtra lookup by global ID or traditional address.
- Tiṅanta derivation entrypoint (`ash_tinanta`) with **LAT path implemented**.
- Subanta derivation entrypoint (`ash_subanta`) across several stem classes.
- Kṛt derivation (`ash_krit`) with curated and fallback generation logic.
- Samāsa derivation (`ash_samasa`) across six samāsa categories.
- Sandhi join/split public APIs (`ash_sandhi_apply`, `ash_sandhi_split`).
- Pratyāhāra expansion and membership checks.
- Internal SLP1 processing with output conversion to IAST/Devanāgarī/HK.
- Unit-test + phase-validation framework (`validate-phase0`…`validate-phase6`).

---

## Core analogy: Pāṇini as systems architecture

Panini’s grammar maps naturally to modern software architecture:

- **Śāstra corpus** → immutable runtime data (TSV-loaded rule/corpus tables).
- **Sūtra** → executable transform or constraint unit.
- **Adhikāra/anuvṛtti/paribhāṣā** → scope, inherited context, and meta-execution policy.
- **Prakriyā** → deterministic state-transition pipeline with trace logs.
- **Vipratiṣedha** → conflict-resolution policy among competing candidates.

In engineering terms, this repo is a **deterministic rule VM** over compact phonological/morphological state.

---

## Quick start

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
./build/ash_demo
```

Minimal API usage:

```cpp
#include "ashtadhyayi.h"

int main() {
  ASH_DB *db = ash_db_load("data/");
  ASH_Form f = ash_tinanta(db, "BU", 1, ASH_LAT, ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
  if (f.valid) {
    // f.slp1 / f.iast / f.devanagari available
  }
  ash_form_free(&f);
  ash_db_free(db);
  return 0;
}
```

---

## Master Table of Contents

1. [01_README_Vision_and_Value.md](01_README_Vision_and_Value.md)
2. [02_Paninian_Domain_Knowledge.md](02_Paninian_Domain_Knowledge.md)
3. [03_Theoretical_to_Technical_Glossary.md](03_Theoretical_to_Technical_Glossary.md)
4. [04_Architecture_and_System_Design.md](04_Architecture_and_System_Design.md)
5. [05_The_Paninian_Rule_Engine.md](05_The_Paninian_Rule_Engine.md)
6. [06_State_Context_and_Execution_Pipeline.md](06_State_Context_and_Execution_Pipeline.md)
7. [07_Data_Structures_Memory_and_Performance.md](07_Data_Structures_Memory_and_Performance.md)
8. [08_Developer_Build_and_Contribution_Guide.md](08_Developer_Build_and_Contribution_Guide.md)
9. [09_Testing_Validation_and_Debugging.md](09_Testing_Validation_and_Debugging.md)
10. [10_User_Manual_and_API_Integration.md](10_User_Manual_and_API_Integration.md)
11. [11_Known_Issues_Edge_Cases_and_Roadmap.md](11_Known_Issues_Edge_Cases_and_Roadmap.md)

---

## Reading order

- Read [02_Paninian_Domain_Knowledge.md](02_Paninian_Domain_Knowledge.md) first for theory.
- Then [03_Theoretical_to_Technical_Glossary.md](03_Theoretical_to_Technical_Glossary.md) for concept mapping.
- Then [04](04_Architecture_and_System_Design.md) → [05](05_The_Paninian_Rule_Engine.md) → [06](06_State_Context_and_Execution_Pipeline.md) for implementation internals.
- Use [08](08_Developer_Build_and_Contribution_Guide.md), [09](09_Testing_Validation_and_Debugging.md), and [10](10_User_Manual_and_API_Integration.md) for day-to-day engineering.

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
