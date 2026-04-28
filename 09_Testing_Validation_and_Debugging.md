# 09 — Testing, Validation, and Debugging

This chapter documents the verification stack and debugging hooks currently implemented in the repository.

Related:
- Build/contribution flow: [08_Developer_Build_and_Contribution_Guide.md](08_Developer_Build_and_Contribution_Guide.md)
- User-level invocation: [10_User_Manual_and_API_Integration.md](10_User_Manual_and_API_Integration.md)

---

## 1) Test architecture

### 1.1 Unit test framework

- Framework: Unity (`tests/unity/unity.c`).
- Discovery: `tests/unit/test_*.c` via CMake glob.
- Execution: CTest binaries generated per unit file.

### 1.2 Phase-gated validation

Staged targets:

- `validate-phase0`: ingestion validation scripts.
- `validate-phase1`: phonology/sandhi/encoding tests.
- `validate-phase2`: sutra/metadata tests.
- `validate-phase3`: tinanta tests + oracle script.
- `validate-phase4`: subanta tests + oracle script.
- `validate-phase5`: pipeline/samasa/krit/taddhita/unadi tests.
- `validate-phase6`: full ctest + coverage script.

---

## 2) Linguistic oracle validation

### 2.1 Tiṅanta comparison

`tools/run_vyakarana_oracle.py`:

- reads `data/dhatuforms.tsv` and `data/dhatupatha.tsv`,
- invokes `build/ash_demo tinanta ...`,
- normalizes output (`normalize_tinanta`),
- checks threshold in `--validate` mode (85% in current configuration).

### 2.2 Subanta comparison

`tools/run_scl_oracle.py`:

- reads `data/shabda_forms.tsv`,
- invokes `build/ash_demo subanta ...`,
- compares over configured supported subset,
- enforces 85% threshold in `--validate`.

### 2.3 Known differences register

`tests/KNOWN_DIFFERENCES.md` documents expected divergences and tracking format.

---

## 3) Coverage enforcement

`tools/coverage_report.py`:

- gathers gcov summaries,
- filters project `.c` sources,
- computes weighted line coverage,
- writes report to `tests/regression/sutra_coverage_report.txt`,
- fails below threshold (`ASH_COVERAGE_TARGET`, default `95.0`).

---

## 4) Traceability and debugging surfaces

### 4.1 Derivation traces

- Internal logger: `prakriya_log()`.
- Internal print helper: `prakriya_print_trace()`.
- Public trace payload: `ASH_Form.steps`.
- Public pretty printer: `ash_form_print_prakriya()`.

### 4.2 CLI debug harness

`examples/demo.c` supports:

- no-arg multi-feature demo,
- direct `tinanta` command mode,
- direct `subanta` command mode.

### 4.3 Branch-focused testing

`tests/unit/test_coverage_branches.c` intentionally exercises error and edge branches across modules (encoding, sandhi, metadata, conflict, kṛt/taddhita, unadi, pipeline).

---

## 5) Practical debugging workflow

1. Reproduce via minimal `ash_demo` command or closest unit test.
2. Inspect `ASH_Form.valid` and `ASH_Form.error`.
3. Print step trace when available.
4. Add/update nearest module-level unit test.
5. Run targeted `ctest -R <name>` then relevant `validate-phaseN`.

---

## 6) Current observability limits

Grounded limits:

- no centralized structured logging backend,
- no single global derivation debugger spanning every module path,
- trace granularity differs by subsystem (e.g., subanta ctx-based steps vs fixed LAT step list in tinanta pipeline).

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
