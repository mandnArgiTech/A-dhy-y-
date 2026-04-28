# BUG-006 — README claims "100% authentic, complete" but implementation covers a small subset

**Severity:** LOW (truth in advertising), MEDIUM (sets wrong user expectations)  
**Phase blocked:** none (cosmetic), but blocks honest releases  
**Files affected:** `README.md`, `01_README_Vision_and_Value.md`

---

## Symptom

`README.md` line 3:

> A 100% authentic, modular C implementation of Pāṇini's Aṣṭādhyāyī — the world's first formal generative grammar (~500 BCE), covering all 3,983 sūtras of the BORI edition.

What's actually implemented as of this snapshot:

| Component | Status |
|-----------|--------|
| Phonology (varṇa, pratyāhāra, sandhi, encoding) | ✓ implemented and tested |
| Sūtra loader (3,983 sūtras parsed) | ✓ loaded; not all referenced |
| Saṃjñā registry (38 categories) | ✓ implemented |
| Anubandha stripping | ✓ partial (handles ~6 of the ~10 cases in 1.3.2-1.3.9) |
| Adhikāra / Anuvṛtti / Paribhāṣā | ✓ skeletal (5 anuvṛtti seeds, 8 paribhāṣās with no application logic) |
| Tiṅanta (verb conjugation) | ✗ **only LAT, only 5 hardcoded roots, only the 3rd-pers-sg cell is actually verified** |
| Subanta (noun declension) | ✗ **only 3 of 24 cells implemented (and they return literal "rAm…" strings)** |
| Kṛt / Taddhita | partial — modules exist but few rules |
| Samāsa | partial |
| Conflict resolution | skeletal |
| Validation against oracles | broken (BUG-001, BUG-002) |

A more honest characterization is "**foundation in place; full Aṣṭādhyāyī coverage in progress**".

## Required fix

Rewrite the README's opening to state real status. Move the aspirational language to a "Vision" section.

### Suggested new opening

```markdown
# libAshtadhyayi — A C library for Pāṇinian Sanskrit grammar

A modular C17 library implementing Pāṇini's Aṣṭādhyāyī (~500 BCE).

## Current implementation status (vX.Y.Z)

The foundation is solid and well-tested:
- **Phonology**: 52-phoneme varṇa table, all 44 pratyāhāras, full vowel/consonant/visarga sandhi
- **Encoding**: SLP1 ↔ IAST ↔ Devanāgarī ↔ HK round-trip
- **Sūtra database**: All 3,983 BORI-edition sūtras loaded with type classification
- **Metadata**: 38-bit saṃjñā registry, anubandha stripping, adhikāra scope tracking, 8 paribhāṣās

The derivation pipeline is partial:
- **Tiṅanta (verbs)**: laṭ tense only; ~5 verified roots; full lakāra coverage in progress
- **Subanta (nouns)**: a-stem masculine partial; other classes in progress
- **Kṛt / Taddhita / Samāsa**: skeletal modules

Validation against the bundled reference oracles (216,168 noun forms + 254,736 verb forms)
is in active development; current match rates are tracked in `tests/regression/`.

## Vision

The end goal is full coverage of all 3,983 sūtras with verifiable derivations
matching the bundled reference oracles, plus prakriyā traces that cite the same
sūtras as the classical commentary tradition.
```

## Acceptance Criteria

- [ ] README opening rewritten with honest status (no "100%", no "complete", no "all 3,983")
- [ ] `01_README_Vision_and_Value.md` updated similarly — separate vision from status
- [ ] Add a `## Status` section to README with checkbox table per phase/story
- [ ] Add link to `.cursor/bugs/` directory and `.cursor/stories/` directory so reviewers can see open work
- [ ] Re-run all tests; commit; tag the commit `v0.1.0-foundation`

## Estimated effort

1 hour
