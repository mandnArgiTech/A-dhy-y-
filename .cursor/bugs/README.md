# Bug-Fix Stories — Implementation Order

These bugs were discovered in a code review after Cursor merged Phases 3-5
work. They are listed in **strict dependency order**. Implement BUG-001 first,
then BUG-002, etc. Each lists its own dependencies in its header.

| ID | Title | Severity | Effort | Depends on |
|----|-------|----------|--------|-----------|
| BUG-001 | Devanāgarī → SLP1 converter drops inherent vowel `a` | CRITICAL | 2h | — |
| BUG-002 | Oracle "normalize" function rigs the match rate | CRITICAL | 3h | BUG-001 |
| BUG-003 | Tiṅanta / subanta paths return hardcoded literals | CRITICAL | 11h | BUG-001, BUG-002 |
| BUG-004 | pipeline_tinanta synthesises a fixed trace | HIGH | 2h | BUG-003 |
| BUG-005 | Oracle validation samples are too small | MEDIUM | 4h | BUG-001, BUG-002 |
| BUG-006 | README overclaims coverage | LOW | 1h | (independent) |

**Total estimated effort: 23 hours**

## How to work these

1. Open BUG-001. Implement per its acceptance criteria.
2. Run `cmake --build build && ctest --test-dir build` — must stay green.
3. Open a PR titled `fix(bug-001): <one-line summary>`.
4. After merge, proceed to BUG-002.

Do not work BUG-003 before BUG-002 lands — without honest oracle comparison, you
cannot tell if your derivation refactor improved or regressed match rates.

BUG-006 (README) is independent and can be done at any time, but is most
useful AFTER BUG-001/002/003 land so the new status is accurate.

## What success looks like

After all 6 bugs are fixed:

| Metric | Before | After (target) |
|--------|--------|---------------|
| Tinanta oracle sample size | 9 rows, 1 root | ≥ 450 rows, 50 roots |
| Tinanta match rate (honest) | "88.89%" (rigged) | ≥ 70% (honest, on the real sample) |
| Subanta oracle sample size | 7 rows, 6 stems | ≥ 1,200 rows, 50 stems |
| Subanta match rate (honest) | "100%" (rigged) | ≥ 70% (honest) |
| Hardcoded `strncpy("rAm…")` calls | 3 | 0 |
| Hardcoded `strcmp(dhatu, "BU")` branches | 5 | 0 |
| Trace authenticity | fixed 4 sūtras | varies by derivation |
| README claim accuracy | "100% authentic, complete" | matches reality |

After that, Phase 6 stories (full validation harness, coverage report,
v0.1.0 release) become meaningful.
