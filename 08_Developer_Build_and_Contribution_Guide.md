# 08 — Developer Build and Contribution Guide

This chapter is a contributor playbook based on the actual build/test topology in this repository.

Related:
- Validation and debugging: [09_Testing_Validation_and_Debugging.md](09_Testing_Validation_and_Debugging.md)
- API usage: [10_User_Manual_and_API_Integration.md](10_User_Manual_and_API_Integration.md)

---

## 1) Toolchain and language requirements

From `CMakeLists.txt`:

- CMake minimum: `3.18`.
- Language: `C` (project configured as `LANGUAGES C`).
- C standard: `C17`, required.
- Default warnings: `-Wall -Wextra -Werror -pedantic`.
- Optional sanitizers: `ASH_ENABLE_SANITIZERS=ON` adds ASan/UBSan flags.

`include/ashtadhyayi.h` includes `extern "C"` guards, so C++ consumers can link the C API.

---

## 2) Build commands

### 2.1 Canonical build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
```

Or via Make wrapper:

```bash
make build
```

### 2.2 Run tests

```bash
ctest --test-dir build --output-on-failure
```

Or:

```bash
make test
```

---

## 3) Phase validation targets

Available targets:

- `validate-phase0`
- `validate-phase1`
- `validate-phase2`
- `validate-phase3`
- `validate-phase4`
- `validate-phase5`
- `validate-phase6`

Run via:

```bash
make validate-phase3
```

or:

```bash
cmake --build build --target validate-phase3
```

`phase0` runs ingestion validators; later phases combine unit tests and oracle scripts.

---

## 4) How to add or modify a sūtra-driven rule

This codebase currently uses module-local rule coding, not one global compiler over `SutraDB`.
Use this practical pattern:

1. Identify the subsystem (`sandhi`, `tinanta`, `subanta`, `krit`, `taddhita`, `samasa`).
2. Add/adjust the rule in the appropriate module function.
3. Ensure a sūtra ID is logged into trace structures where that path supports tracing.
4. Add/update focused unit tests in `tests/unit/test_<module>.c`.
5. Run the relevant phase gate.

### 4.1 Example pattern (module mutation + trace)

```cpp
prakriya_log(&ctx, 703084, "sArvadhAtukArdhadhAtukayoH");
```

### 4.2 Sūtra ID convention caution

Different parts of the repo use different numeric conventions:

- `SutraDB.global_id`: loaded serial ID from TSV.
- Several derivation logs: packed traditional-style numeric addresses.

When extending a module, follow that module’s existing convention for consistency.

---

## 5) Adding new source files

If you create a new `.c` module:

1. Place it in the correct subsystem directory.
2. Add path into corresponding source list in `CMakeLists.txt`:
   - `PRAKRIYA_SRC`, `SANDHI_SRC`, `METADATA_SRC`, etc.
3. Reconfigure/rebuild.

Unit tests are auto-discovered by glob (`tests/unit/test_*.c`) and usually need no CMake edit.

---

## 6) Data and ingestion workflow

Runtime depends on generated TSVs under `data/`.

Validation ingestion commands are already encoded in `validate-phase0`:

- `tools/ingest_source.py --validate`
- `tools/ingest_dhatupatha.py --validate`
- `tools/ingest_ganapatha.py --validate`
- `tools/ingest_pratyahara.py --validate`
- `tools/ingest_shabda.py --validate`
- `tools/ingest_dhatuforms.py --validate`
- `tools/ingest_shabdaprakriya.py --validate`

---

## 7) Coding conventions from repository practice

- Public symbols prefixed `ASH_` / `ash_`.
- Internal symbols are subsystem-scoped (`varna_`, `sutra_`, `prakriya_`, etc.).
- Internal processing uses SLP1; convert at output boundary.
- Prefer bounded string ops and explicit null checks.
- Keep test coverage for changed behavior by extending module-specific unit tests.

---

## 8) Suggested contributor checklist

Before opening a PR:

1. Build cleanly with strict warnings.
2. Run targeted unit tests.
3. Run relevant `validate-phaseN`.
4. Confirm no regressions in demo/API path touched by your change.
5. Update docs if behavior surface changed.

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
