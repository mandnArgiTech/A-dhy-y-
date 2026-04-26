# 03 — Theoretical to Technical Glossary (The Bridge)

This chapter maps Paninian concepts from [02_Paninian_Domain_Knowledge.md](02_Paninian_Domain_Knowledge.md) to **concrete constructs implemented in this repository**.

> Scope rule for this chapter: every technical claim below is based on code currently present in this workspace.

---

## 1) Paninian concept → exact implementation mapping

| Paninian concept | What it means | Exact implementation in this repo |
|---|---|---|
| Śiva-sūtras / phoneme inventory | Ordered phonological basis | `core/phonology/varna.c`, `core/phonology/pratyahara.c` |
| Pratyāhāra | Compressed phoneme class reference | `pratyahara_get()`, `pratyahara_contains()`, public `ash_pratyahara_expand()` |
| Dhātu | Verbal root | Loaded via `dhatupatha_db_load()` and copied into `Pipeline.dhatus` in `pipeline_init()` |
| Pratyaya | Suffix/operator | Tiṅ tables in `prakriya/tinanta/lakara.c`; Sup tables in `prakriya/subanta/vibhakti.c`; kṛt/taddhita suffix specs in module-local tables |
| It/anubandha | Marker letters removed from output form | `anubandha_strip()` + `it_to_samjna()` in `core/metadata/anubandha.c` |
| Saṃjñā | Category tags | 64-bit bitmask in `core/metadata/samjna.h`; used by `Term.samjna` |
| Aṅga/prātipadika/term state | Mutable derivation unit | `Term` struct in `prakriya/term.h` |
| Prakriyā | Stepwise derivation with provenance | `PrakriyaCtx` + `PrakriyaStep` in `prakriya/context.h`; public `ASH_PrakriyaStep` in `include/ashtadhyayi.h` |
| Adhikāra | Scope inheritance | `adhikara_build()` annotates `Sutra.adhikara_parent` |
| Anuvṛtti | Carried-over terms from prior sūtra | Seeded metadata DB in `core/metadata/anuvrtti.c` |
| Paribhāṣā | Meta-rule for rule behavior | `core/metadata/paribhasha.c` (table + predicates) |
| Vipratiṣedha (conflict) | Rule collision resolution | `core/conflict/conflict.c` (`conflict_resolve()`, candidate ranking) |
| Asiddhatva / precedence | Visibility and order semantics | Represented as conflict/paribhāṣā metadata; not globally orchestrated in one central derivation loop |
| Sandhi | Boundary phonological transforms | `sandhi_vowel.c`, `sandhi_hal.c`, `sandhi_visarga.c`; wrapped by `ash_sandhi_apply()` |
| Samāsa | Compound formation | `samasa_derive()` in `samasa/samasa.c` |
| Tiṅanta | Finite verb derivation | `pipeline_tinanta()` → `lat_bhvadi_derive()` |
| Subanta | Nominal derivation | `pipeline_subanta()` dispatch to stem-class modules |

---

## 2) Engine-oriented interpretations of key concepts

### 2.1 Pratyāhāra as lookup structures

- Implemented as precomputed static expansions in `core/phonology/pratyahara.c`.
- Query complexity is bounded by static table size.
- Public API surfaces this through `ash_pratyahara_expand()` and `ash_in_pratyahara()`.

### 2.2 Anuvṛtti as context carry-over

- Current implementation provides a seeded `AnuvrttiDB` (`anuvrtti_load()`).
- This is metadata lookup, not a global automatic term-propagation engine across all derivation modules.

### 2.3 Paribhāṣā as execution policy objects

- `paribhasha_get()` and `paribhasha_applies()` expose named meta-rules.
- `paribhasha_sthanivad_applies()` encodes one concrete predicate.
- These are currently not a universal dispatcher for all mutation paths; modules apply their own local logic.

### 2.4 Vikalpa (optionality)

- No global branching lattice is implemented.
- Most public derivations return one `ASH_Form`; optional alternatives are generally not enumerated.

### 2.5 Asiddhatva and precedence

- Conflict precedence is encoded in `wins_over()` (`apavada`, `nitya`, `optional`, `antaranga`, fallback `sutra_id` order).
- This conflict module exists and is unit-tested, but current derivation entrypoints do not universally route through it.

---

## 3) Data model glossary

| Conceptual role | Concrete type |
|---|---|
| Public sūtra record | `ASH_Sutra` (`include/ashtadhyayi.h`) |
| Internal sūtra DB | `SutraDB` (`core/sutrapatha/sutra.h`) |
| Public result object | `ASH_Form` |
| Public step trace row | `ASH_PrakriyaStep` |
| Internal derivation context | `PrakriyaCtx` |
| Internal derivation token | `Term` |
| Public DB handle | opaque `ASH_DB` (contains `Pipeline`) |
| Internal pipeline container | `Pipeline` (`prakriya/pipeline.h`) |

---

## 4) What is *not* implemented as an abstract machine today

Grounded limitations:

- No single global “match all sutras until fixpoint” loop.
- No generic AST/trie-based rule compiler.
- No centralized conflict scheduler invoked by all derivation branches.
- Some paths are table-driven and specialized (e.g., LAT-specific logic, stem-class dispatch chains).

These are architecture facts of current code, not theoretical constraints.

---

## Related chapters

- Theory primer: [02_Paninian_Domain_Knowledge.md](02_Paninian_Domain_Knowledge.md)
- Architecture blueprint: [04_Architecture_and_System_Design.md](04_Architecture_and_System_Design.md)
- Rule internals: [05_The_Paninian_Rule_Engine.md](05_The_Paninian_Rule_Engine.md)

---

🔙 Back to TOC: [Master Table of Contents](01_README_Vision_and_Value.md#master-table-of-contents)
