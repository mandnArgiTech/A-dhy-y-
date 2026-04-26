# 05 — The Paninian Rule Engine (Core Logic)

This is the rule-mechanics chapter.  
All claims here are grounded in currently implemented code paths.

Related:
- Architecture overview: [04_Architecture_and_System_Design.md](04_Architecture_and_System_Design.md)
- State lifecycle trace: [06_State_Context_and_Execution_Pipeline.md](06_State_Context_and_Execution_Pipeline.md)

---

## 1) How rule applicability is identified

The codebase uses **multiple pattern-selection styles**, not one generic matcher.

### 1.1 Direct dispatch by grammatical category

- `pipeline_tinanta()` checks lakāra and only executes LAT logic (`if (l != ASH_LAT) error`).
- `pipeline_subanta()` performs stem-class dispatch:
  - `a_stem_masc_can_handle()`,
  - `aa_stem_fem_can_handle()`,
  - `i_stem_can_handle()`,
  - `u_stem_can_handle()`,
  - `n_stem_can_handle()`,
  - `as_stem_can_handle()`,
  - `r_stem_can_handle()`.

This is deterministic and branch-ordered.

### 1.2 Table-driven pattern resolution

- Tiṅ endings are resolved by index over `(lakara, purusha, vacana, pada)` in `ting_get()`.
- Sup endings are resolved by `(vibhakti, vacana, linga)` in `sup_get()`.
- Taddhita and selected kṛt behavior use static spec tables and curated lexical overrides.

### 1.3 Character-level phonological matching

- Sandhi modules inspect boundary characters (`a_final`, `b_initial`) and apply conditional transforms:
  - savarṇadīrgha, guṇa, vṛddhi, yaṇ, ayavāyāv (vowel sandhi),
  - voicing/devoicing/anusvāra-like changes (hal sandhi),
  - visarga-conditioned replacement.

---

## 2) Rule execution style by subsystem

| Subsystem | Execution style |
|---|---|
| Tinanta LAT | Specialized procedural derivation (`lat_bhvadi_derive`) + fixed post-trace |
| Subanta | Stem-class module dispatch + local transform rules |
| Kṛt | Context-backed sequence with selected guṇa checks and suffix assignment |
| Taddhita | Spec lookup, optional vṛddhi, irregular table fallback |
| Samāsa | Join + type-specific post-edit + single trace step |
| Sandhi | Boundary rewrite functions with rule IDs in results |

No global “iterate over all sūtras and test applicability” engine exists currently.

---

## 3) Conflict resolution (*Vipratiṣedha*) in code

Conflict API exists in `core/conflict/conflict.c`.

### 3.1 Candidate model

`ConflictCandidate` includes:

- `sutra_id`,
- `is_apavada`,
- `is_nitya`,
- `is_antaranga`,
- `is_optional`.

### 3.2 Ranking function (`wins_over`)

Priority order implemented:

1. `is_apavada` (true wins),
2. `is_nitya` (true wins),
3. optionality (non-optional wins),
4. `is_antaranga` (true wins),
5. fallback: larger `sutra_id` wins.

This is a deterministic tournament in `conflict_resolve()`.

### 3.3 Current integration status

- Conflict logic is implemented and testable.
- Current major derivation entrypoints do not universally pass multi-candidate conflicts through this resolver.

---

## 4) Rule precedence and meta-rules (*Paribhāṣā*, *Asiddhatva*)

### 4.1 Implemented metadata layer

`core/metadata/paribhasha.c` exposes:

- named paribhāṣā table entries,
- `paribhasha_applies()`,
- `paribhasha_sthanivad_applies()`.

### 4.2 Practical execution behavior

- Paribhāṣā functions are available but not used as a central scheduler for all runtime rule ordering.
- Asiddhatva-like behavior is represented conceptually through conflict/meta modules, but not as a universal visibility engine driving every derivation path.

---

## 5) Pattern matching examples (code-level)

### 5.1 Subanta class gate

```cpp
if (li == ASH_PUMS && a_stem_masc_can_handle(normalized)) {
  ok = a_stem_masc_derive(normalized, vib, v, &ctx);
}
```

### 5.2 Tinanta lakāra gate

```cpp
if (l != ASH_LAT) {
  return make_error_form("only LAT implemented");
}
```

### 5.3 Sandhi boundary gate

```cpp
if (a_final == 'H') { ... }
else if (varna_is_vowel(a_final) && varna_is_vowel(b_initial)) { ... }
else { ... }
```

These demonstrate explicit deterministic branching as the active matching strategy.

---

## 6) Engine semantics summary

Current rule engine is best described as:

- deterministic,
- modular,
- partially table-driven,
- partially procedural,
- trace-aware,
- with conflict/meta infrastructure present but not globally unified.

---

## 7) Next chapter

For exact step lifecycle and state mutation sequence, read:
[06_State_Context_and_Execution_Pipeline.md](06_State_Context_and_Execution_Pipeline.md)

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
