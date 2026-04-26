# 11 — Known Issues, Edge Cases, and Roadmap

This chapter lists implementation gaps and edge conditions visible from current code behavior.

Related:
- Rule-engine internals: [05_The_Paninian_Rule_Engine.md](05_The_Paninian_Rule_Engine.md)
- Testing and validation: [09_Testing_Validation_and_Debugging.md](09_Testing_Validation_and_Debugging.md)

---

## 1) Current known implementation constraints

## 1.1 Tinanta scope is limited to LAT in pipeline gate

`pipeline_tinanta()` explicitly rejects non-`ASH_LAT`:

- returns invalid `ASH_Form` with error `"only LAT implemented"`.

Impact:

- public `ash_tinanta` is not yet full-lakāra complete despite enum support for many lakāras.

## 1.2 Rule engine is not a universal global scheduler

Current architecture combines:

- table-driven and procedural module logic,
- optional metadata/conflict helpers.

Not yet present:

- one central “iterate all applicable sūtras until convergence” executor.

## 1.3 Conflict/paribhāṣā integration is partial

- `core/conflict/conflict.c` exists with deterministic ranking.
- `core/metadata/paribhasha.c` exposes meta-rule descriptors/predicates.

But:

- major derivation entrypoints do not consistently invoke a unified conflict/paribhāṣā orchestration layer.

## 1.4 Trace detail consistency varies by path

- Subanta and some derivative paths log through `PrakriyaCtx`.
- Tinanta LAT path currently synthesizes a fixed 4-step trace in `pipeline_tinanta`.

Result:

- trace quality is not uniform across all derivation families.

## 1.5 Numeric sutra ID convention inconsistency

The codebase mixes:

- TSV `global_id` serial indexing,
- packed traditional-style numeric IDs in traces and metadata maps.

Risk:

- confusion when correlating runtime logs with loaded `SutraDB` IDs.

---

## 2) Edge cases visible in code paths

## 2.1 Subanta stem normalization special-cases

Example:

- `pipeline_subanta()` normalizes `"rAma"` to `"rAm"` before class dispatch.

Implication:

- external input conventions and internal stem conventions can diverge and need careful handling for additional stems.

## 2.2 Sandhi split search is combinational over split points

`ash_sandhi_split()`:

- tries split points linearly and rechecks by recombination.

Implication:

- simple and deterministic, but not a linguistically ranked splitter and not exhaustive in sophisticated multi-rule ambiguity scenarios.

## 2.3 Manual lexical overrides in derivation modules

Examples:

- root-specific overrides in `lat_bhvadi.c`,
- curated forms in `krit_primary.c`,
- irregular entries in `taddhita.c`.

Implication:

- practical correctness for tested forms, but coverage of full lexical space depends on continued expansion.

## 2.4 Metadata loaders with seeded subsets

`anuvrtti_load()` uses seed table behavior and ignores TSV argument currently.

Implication:

- metadata support is present but not corpus-complete.

---

## 3) Error-handling and robustness notes

- Many functions guard null pointers and bounds.
- Failures are usually surfaced as invalid `ASH_Form` with message or boolean false.
- Some loader comments and behavior indicate incremental maturity (e.g., loader assumptions about generated data presence).

Practical recommendation:

- keep phase gates mandatory on every substantive change.

---

## 4) Testing/validation caveats

- Oracle scripts intentionally use scoped subsets/profiles for current implementation maturity.
- Coverage target enforcement depends on gcov artifact availability and coverage-enabled build context.

This is expected in an incremental implementation strategy, but should be tracked explicitly per milestone.

---

## 5) Forward roadmap (code-aligned)

The following roadmap items align directly with observed structure and current gaps:

1. Expand tinanta beyond LAT path in `pipeline_tinanta` and associated modules.
2. Unify trace generation so all derivation families log from actual state transitions.
3. Introduce consistent sutra-ID policy across metadata/trace/API lookup layers.
4. Integrate conflict/paribhāṣā modules into live multi-rule derivation scheduling paths.
5. Grow lexical coverage while reducing hardcoded irregular fallback reliance.
6. Extend anuvṛtti/adhikāra-driven contextual execution from metadata support to operational application where appropriate.
7. Add optional branch-enumeration mode (vikalpa-aware) while keeping deterministic default output mode.

---

## 6) Risk register summary

| Risk | Current severity | Mitigation path |
|---|---|---|
| Non-LAT tinanta unsupported in pipeline | High | implement remaining lakāra modules + route through pipeline |
| Inconsistent trace semantics across modules | Medium | standardize ctx-based step capture |
| Partial conflict integration | Medium | centralize candidate resolution hook in derivation dispatch |
| Mixed sutra-ID semantics | Medium | define canonical internal ID policy and migrate incrementally |
| Hardcoded lexical branches | Medium | increase data-driven mappings and add regression cases |

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
