# Story 6.7 — Comprehensive conflict resolution and paribhāṣā orchestration

**Phase:** 6 — Validation Harness & API Polish
**Difficulty:** Very High
**Estimated time:** 16 hours
**Depends on:** all of phases 0–5; specifically Story 3.16 (LAT 100%)
                and Story 4.15 (subanta 98%+)

---

## Objective

The current engine applies sūtras in carefully-ordered hardcoded
chains within each derivation module (lat_bhvadi.c, a_stem_full.c,
etc.). The Aṣṭādhyāyī's actual architecture is **declarative**:
rules are listed largely in topical order, and the *paribhāṣā*
meta-rules govern which rule fires when several are applicable.

This story unifies the rule-application machinery so that:

1. **Every applied rule** flows through `core/conflict/conflict.c`,
   which evaluates candidate sūtras and selects the winner per the
   four-level priority (apavāda → nitya → antaraṅga → paratva).
2. **Every relevant paribhāṣā** from `data/paribhasha.tsv` is wired
   as either a precondition (gates rule firing) or a postcondition
   (modifies the result).
3. The hardcoded ordering inside lat_bhvadi.c et al. is replaced by
   a generic dispatcher that lists candidate rules and lets conflict.c
   choose.

After this story, **adding a new rule** is a single-table-row change
rather than a `if/else if` injection in the middle of derivation
logic.

---

## Background — current state

- `core/conflict/conflict.c` exists with a 4-level priority function but
  is rarely invoked (only by test_conflict.c).
- `core/metadata/paribhasha.c` exposes paribhāṣā predicates but no
  derivation path consults them.
- `prakriya/tinanta/lat_bhvadi.c` directly chains:
  `if-else` for substitution → IV_GAM_YAM list → KRAM list → guṇa →
  vikaraṇa → ec→ay → tiṅ → lengthening → visarga → ṣatva.
  This linear if-else IS a hand-tuned rule order; it works for LAT
  but doesn't scale.

---

## Files

### `prakriya/rule_engine.h` / `rule_engine.c` (new, ~600 lines)

```c
typedef struct {
  uint32_t sutra_id;
  bool (*precondition)(const PrakriyaCtx *);
  void (*apply)(PrakriyaCtx *);
  /* Conflict metadata */
  bool is_apavada_of[8];   /* sūtra IDs this is an exception to */
  bool is_nitya;
  bool is_antaranga;
} RuleEntry;

/** The master rule registry — one entry per implemented sūtra. */
extern const RuleEntry RULE_REGISTRY[];

/** Run the rule engine to convergence. */
bool rule_engine_run(PrakriyaCtx *ctx);
```

### `prakriya/tinanta/lat_bhvadi.c` (refactor)
Rewrite the linear if-else as a series of calls into rule_engine_run().
Each existing transformation becomes a `RuleEntry` in the registry.

### `core/conflict/conflict.c` (extend)
Already has the 4-level priority resolver. Make it consult the
paribhāṣā predicates from `core/metadata/paribhasha.c`.

### `tests/regression/conflict_oracle.tsv` (new)
A list of derivations where the rule choice matters (e.g. 6.1.77
yaṅ vs 6.1.101 savarṇadīrgha — apavāda wins; 8.4.41 ṣṭuva vs
8.4.45 yaro'nunāsike — antaraṅga wins).

---

## Acceptance Criteria

- [ ] All existing LAT/subanta derivations still produce the same
      forms (oracle rates unchanged).
- [ ] Adding a new sūtra requires editing only RULE_REGISTRY (no
      changes to derivation modules).
- [ ] `test_conflict.c` covers all four priority levels with at
      least 3 examples each.
- [ ] Tinanta oracle stays at 100%, subanta stays at ≥ 98%.
- [ ] Phase-3, Phase-4, Phase-5, Phase-6 validation all green.
- [ ] Number of LOC in lat_bhvadi.c shrinks by ≥ 30% (the hand-coded
      chains move to RULE_REGISTRY).

---

## Notes

- This is the architectural milestone that turns the project from
  "engine that produces correct forms for the implemented LAT/subanta
  cases" to "Pāṇinian rule applicator with fidelity to the
  Aṣṭādhyāyī's declarative architecture."
- It is also the prerequisite for adding lakāras 3.17–3.25 cleanly:
  each new lakāra would just register its endings table + class-
  specific rules without touching shared derivation code.
- The 16-hour estimate assumes the LAT module is the only one
  refactored initially; bringing subanta + krit + samāsa under the
  rule engine doubles the effort.
