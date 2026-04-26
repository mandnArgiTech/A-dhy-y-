# 07 — Data Structures, Memory, and Performance (C Core)

This chapter documents real data structures and runtime characteristics in the repository.

Related:
- Architecture: [04_Architecture_and_System_Design.md](04_Architecture_and_System_Design.md)
- Derivation loop: [06_State_Context_and_Execution_Pipeline.md](06_State_Context_and_Execution_Pipeline.md)

---

## 1) Data structures actually implemented

### 1.1 Core linguistic/state structures

- `Sutra` / `SutraDB`:
  - dense array of sutras,
  - `addr_index[9][5][250]` for O(1)-style address lookup by `(adhyaya,pada,num)` domain.
- `Term`:
  - fixed-size char buffers,
  - saṃjñā bitmask,
  - bounded rule history.
- `PrakriyaCtx`:
  - bounded term array + bounded step log array.
- `Pipeline`:
  - loaded `SutraDB`,
  - copied dhātu array and count.

### 1.2 Domain tables

- Tiṅ tables (`LAT_PARASMAI`, `LAT_ATMANE`).
- Sup table (`SUP_TABLE`, built from `SUP_RAW`).
- Taddhita specs (`TADDHITA_SPECS`).
- Phoneme tables in encoding and phonology modules.

### 1.3 What is not present

- No AST classes.
- No trie implementation.
- No custom hash-map type.
- No graph runtime object for derivation search.

The current architecture is table + struct + procedural transforms.

---

## 2) Memory management model

### 2.1 Ownership boundaries

- Public DB handle:
  - `ash_db_load()` allocates `ASH_DB`,
  - `ash_db_free()` releases pipeline-owned memory + handle.
- Public form results:
  - `ASH_Form.steps` is heap-allocated where needed,
  - caller releases via `ash_form_free()`.

### 2.2 Stack vs heap strategy

- Stack:
  - transient local buffers (`char[64]`, `char[128]`, etc.),
  - `PrakriyaCtx` often stack-allocated in module entry functions.
- Heap:
  - loaded DB arrays (`sutras`, `dhatus`, ancillary DB entries),
  - returned trace arrays.

### 2.3 Safety style

- Widespread `memset` zero-initialization of structs.
- Bounded copies (`strncpy`, `snprintf`, length checks).
- Null guards at API and module boundaries.

### 2.4 RAII/smart pointers

This codebase is C, so no RAII/smart pointers are used.
Resource lifetime is manual and function-contract based.

---

## 3) Complexity analysis (implemented algorithms)

### 3.1 Lookup complexity

- `sutra_get_by_id`: O(1) array index.
- `sutra_get_by_addr`: O(1) via prebuilt index array + ID lookup.
- `pipeline_find_dhatu`: O(n) linear scan over loaded dhātus.
- `pratyahara_contains`: O(k) scan over precomputed class members.

### 3.2 Derivation complexity (practical)

- Tinanta LAT path: bounded procedural steps + string ops (roughly O(L), L = string length).
- Subanta path:
  - O(C) dispatch over small class set,
  - then bounded per-class transformation logic.
- Sandhi apply:
  - constant branch checks + bounded string concatenation/copy.

### 3.3 Space complexity

- Core DB space:
  - O(S + D) where S=sutra count, D=dhatu count.
- Derivation workspace:
  - mostly O(1) bounded stack arrays per call.

---

## 4) Performance-sensitive hotspots

Likely hotspots based on call patterns and loops:

- Dhātu lookup linear scan (`pipeline_find_dhatu`).
- Frequent encoding conversion allocations (`enc_slp1_to_iast`, `enc_slp1_to_devanagari`).
- TSV loaders for large corpora at startup.

Potential optimization targets (architecturally compatible):

- Add indexed dhātu lookup (sorted vector + binary search or hash table).
- Reuse encoding buffers for batch processing.
- Split heavy path from trace-allocation path when trace is not required.

---

## 5) Profiling and benchmarking strategy

Current repository already provides:

- test execution via `ctest`,
- coverage report script (`tools/coverage_report.py`),
- phase gates (`validate-phaseN`).

Practical benchmark extension path:

1. Create focused benchmark executables for `ash_tinanta` / `ash_subanta` batch calls.
2. Run with and without trace packaging.
3. Measure:
   - forms/sec,
   - mean latency,
   - startup load cost.

---

## 6) Summary

The project uses deterministic low-level C data structures with bounded local state and explicit ownership. Performance is currently dominated by linear scans and string transforms rather than heavy graph or parser machinery.

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
