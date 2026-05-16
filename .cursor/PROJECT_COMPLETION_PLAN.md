# Project Completion Plan — closing every gap and edge

**Status snapshot (as of branch `claude/review-project-status-b4lfb`):**
- Phases 0–6 all have green validation gates.
- Tinanta LAT-parasmai oracle: **100%** (468/468)
- Subanta oracle: **98.45%** (1147/1165)
- 8 of 10 lakāras live (5 at 100% match)
- 13 forward-looking story specs already written

This plan enumerates every remaining gap and orders the work to bring
the engine to **complete coverage** with no known edge-case holes.

---

## Track A — Verb morphology (tinanta) completion

| # | Item | Story | Effort | Status |
|---|---|---|---|---|
| A1 | Push LRT to 100% (currently 90%) | new spec | 3h | not started |
| A2 | Push LUT to 100% (currently 90%) | new spec | 3h | not started |
| A3 | Push LRN to 100% (currently 90%) | new spec | 3h | not started |
| A4 | Reduplication engine (shared infra) | 3.27 | 6h | spec only |
| A5 | LIT (perfect) | 3.18 | 12h | spec only |
| A6 | LUN (aorist, 7 sub-classes) | 3.24 | 14h | spec only |
| A7 | Gaṇa-3 (juhotyādi) reduplication | 3.27 | 6h | spec only |
| A8 | Ātmanepada full coverage across all lakāras | 3.26 | 8h | spec only |
| A9 | Pada-flag enforcement from dhātupāṭha | new spec | 2h | not started |
| A10 | Causative (ṇic / aya) full | new spec | 8h | not started |
| A11 | Desiderative (san) | new spec | 10h | not started |
| A12 | Frequentative / intensive (yaṅ) | new spec | 8h | not started |
| A13 | Compound verbs (upasarga + dhātu) | new spec | 6h | not started |

**Track A total: ~89 hours**

Acceptance: all 10 lakāras at ≥ 95% oracle match across the full
parasmaipada AND ātmanepada test set.

---

## Track B — Nominal morphology (subanta) completion

| # | Item | Story | Effort | Status |
|---|---|---|---|---|
| B1 | catur (numeral feminine) — 8 forms | new spec | 1h | not started |
| B2 | pitṛprasU (ū-compound corner) | new spec | 2h | not started |
| B3 | Pronouns: idam, adas, etat, tat, yat, ka (masc), sa, sarva | new spec | 8h | not started |
| B4 | Numerals 1–10, 11–19, decads | new spec | 6h | not started |
| B5 | Adjective declension (subanta of kṛdanta) | new spec | 4h | not started |
| B6 | Compound stems (samāsa output) inflecting as subanta | new spec | 4h | not started |
| B7 | Less-common consonant stems (vāc, marut, suhṛd, śak) | new spec | 6h | not started |
| B8 | Avyaya (indeclinables) — adverbs, particles | new spec | 3h | not started |

**Track B total: ~34 hours**

Acceptance: subanta oracle ≥ 99.5% on a 5000-row sample covering all
declension classes.

---

## Track C — Derivation modules

| # | Item | Story | Effort | Status |
|---|---|---|---|---|
| C1 | Kṛt — full ~129 primary suffixes (current: 5) | new spec | 20h | partial via 5.6 |
| C2 | Kṛdanta morphological inflection wiring | new spec | 4h | not started |
| C3 | Taddhita — secondary suffixes (current: minimal) | new spec | 12h | partial via 5.4 |
| C4 | Uṇādi — proper derivation, not just lookup | new spec | 8h | partial via 5.5 |
| C5 | Samāsa — all 6 types with proper rule application | 5.7 | 12h | spec only |
| C6 | Samāsa + subanta composition pipeline | new spec | 3h | not started |

**Track C total: ~59 hours**

Acceptance: kṛt oracle ≥ 80%, taddhita ≥ 70%, samāsa ≥ 70%, uṇādi
derivation produces real forms (not just lookups).

---

## Track D — Validation, oracles, trace fidelity

| # | Item | Story | Effort | Status |
|---|---|---|---|---|
| D1 | Prakriyā-step oracle: match step traces sūtra-by-sūtra | new spec | 10h | 0% currently |
| D2 | Sūtra ID consistency: align internal IDs with shabdaprakriya.tsv | new spec | 6h | divergent |
| D3 | Conflict + paribhāṣā architecture refactor | 6.7 | 16h | spec only |
| D4 | Coverage report at function-level (≥ 95%) | new spec | 4h | partial |
| D5 | Valgrind-clean CI pass for all derivation paths | new spec | 4h | not run |
| D6 | Fuzz-testing harness for random root+gaṇa+lakāra | new spec | 6h | not started |

**Track D total: ~46 hours**

Acceptance: trace oracle ≥ 50% any-match, ≥ 30% ordered-match; valgrind
shows zero leaks; all derivation paths covered.

---

## Track E — Sandhi & phonology unification

| # | Item | Story | Effort | Status |
|---|---|---|---|---|
| E1 | Unify ṇatva (8.4.1 + 8.4.2) into one helper | new spec | 3h | duplicated currently |
| E2 | Unify ṣatva (8.3.59) into one helper | new spec | 2h | duplicated |
| E3 | Visarga sandhi engine (8.2.66 + 8.3.15 + 8.3.34 family) | new spec | 4h | partial |
| E4 | External sandhi (between padas) | new spec | 6h | not started |
| E5 | Saṃyoga / saṃyogānta rules (8.2.23 etc.) | new spec | 4h | not started |
| E6 | Anunāsika and visarga sub-rules (8.3.30+, 8.4.45+) | new spec | 4h | partial |

**Track E total: ~23 hours**

Acceptance: every sandhi rule lives in one place, called by every
relevant derivation site.

---

## Track F — API, docs, packaging

| # | Item | Story | Effort | Status |
|---|---|---|---|---|
| F1 | Public header surface review + ABI stability | new spec | 3h | not finalised |
| F2 | Error codes (current: free-form strings) | new spec | 3h | string-only |
| F3 | Doxygen + API reference HTML build | new spec | 4h | not built |
| F4 | Usage examples / tutorial program | new spec | 4h | minimal demo |
| F5 | Install / pkg-config / shared-library target | new spec | 2h | scaffolded |
| F6 | Continuous-integration setup (GitHub Actions) | new spec | 3h | not set up |
| F7 | License audit + provenance docs for data files | new spec | 2h | MIT noted |

**Track F total: ~21 hours**

Acceptance: `libashtadhyayi` can be `apt install`-ed (or compiled
once and `#include`-d) with a stable C API and HTML docs.

---

## Track G — Vedic and chandas extensions (optional)

| # | Item | Story | Effort | Status |
|---|---|---|---|---|
| G1 | Vedic-only lakāras (let, conditional alt) | new spec | 8h | not started |
| G2 | Chandasi rules (vedic-specific alternants) | new spec | 12h | not started |
| G3 | Svara (accent) marking | new spec | 8h | not started |

**Track G total: ~28 hours (optional — classical-only scope can skip)**

---

## Cumulative effort estimate

| Track | Hours | Cumulative |
|---|---|---|
| A — verb morphology | 89 | 89 |
| B — nominal morphology | 34 | 123 |
| C — derivation modules | 59 | 182 |
| D — validation/trace | 46 | 228 |
| E — sandhi unification | 23 | 251 |
| F — API/docs/packaging | 21 | 272 |
| G — vedic (optional) | 28 | 300 |

**Required for classical-Sanskrit completion: ~272 hours.**
**Required including vedic scope: ~300 hours.**

---

## Recommended execution order

### Phase α — close immediate gaps (≤ 1 week of focused work, ~25 hours)
- A1, A2, A3 (LRT/LUT/LRN to 100%)
- B1, B2 (catur, pitṛprasū)
- A9 (pada-flag enforcement)
- E1, E2 (unify ṇatva, ṣatva)

Goal: all 8 implemented lakāras at 100%; subanta ≥ 99%.

### Phase β — finish verbal system (≤ 1 month, ~40 hours)
- A4 (reduplication engine)
- A5 (LIT — uses A4)
- A7 (gaṇa-3 — also uses A4)
- A8 (ātmanepada validation)
- A6 (LUN — biggest single piece)

Goal: all 10 lakāras at ≥ 95% oracle match across both padas.

### Phase γ — nominal completion (≤ 2 weeks, ~26 hours)
- B3 (pronouns)
- B4 (numerals)
- B5 (kṛdanta inflection)
- B6 (samāsa + subanta)
- B7 (rare consonant stems)
- B8 (avyaya)

Goal: subanta oracle ≥ 99.5% on a 5000-row sample.

### Phase δ — derivation modules (≤ 1 month, ~59 hours)
- C1 (full kṛt set — 129 suffixes)
- C2 (kṛdanta inflection wiring)
- C3 (taddhita expansion)
- C5 (samāsa rules)
- C4 (uṇādi derivation)
- C6 (samāsa+subanta pipeline)
- A10, A11, A12 (causative, desiderative, intensive)

Goal: kṛt ≥ 80%, taddhita ≥ 70%, samāsa ≥ 70%.

### Phase ε — architectural refactor (≤ 3 weeks, ~46 hours)
- E3, E4, E5, E6 (sandhi engine unification)
- D2 (sūtra-ID alignment)
- D3 (conflict + paribhāṣā orchestration)

Goal: rule-engine architecture; new rules require single-row table
edits; trace-step IDs align with `shabdaprakriya.tsv`.

### Phase ζ — validation & polish (≤ 2 weeks, ~31 hours)
- D1 (trace oracle ≥ 50%)
- D4, D5, D6 (coverage, valgrind, fuzz)
- F1–F7 (API stability, docs, CI, packaging)

Goal: shippable library with API stability guarantee and ≥ 95%
function-coverage in tests.

### Phase η (optional) — vedic
- G1, G2, G3

---

## Definition of "done"

The project is **complete with no known gaps** when:

1. Every Pāṇinian sūtra in `data/sutras.tsv` (3,983 sūtras) is either:
   - Implemented and tested with at least one oracle-passing example, OR
   - Explicitly marked "out of scope" with a justification in
     `tests/KNOWN_DIFFERENCES.md`.
2. Every paradigm row in `data/dhatuforms.tsv` (254,736 forms) is
   either matched by the engine, OR is listed in a known-gap file
   with the specific Pāṇinian rule chain that handles it.
3. Every paradigm row in `data/shabda_forms.tsv` (216,168 forms)
   is either matched OR similarly listed.
4. Every paradigm in `data/shabdaprakriya.tsv` (4,863 derivations,
   13,456 steps) has step-by-step trace alignment ≥ 95%.
5. `make validate-phase6` passes with all four sub-oracles ≥ 95%
   (currently: tinanta 100%, subanta 98.45%, prakriyā 0%).
6. The 13 currently-spec'd stories (LAT-α/β/γ/δ/ε/ζ) plus the
   gap-stories enumerated here are all closed.
7. Valgrind shows zero leaks across all unit tests + a 1000-form
   randomised derivation soak test.
8. Public API has a SemVer commitment (v1.0.0 tag).

---

## Risk register

| Risk | Severity | Mitigation |
|---|---|---|
| LUN aorist's 7-class diversity is fundamentally combinatorial | High | Encode per-root class from a closed seed list; default to a-aorist; track per-class oracle separately |
| Reduplication has many root-specific exceptions | Medium | Use 7.4.60–62 plus a small exception table (cakāra, jagāma, etc.) |
| Conflict resolver refactor may regress oracle rates | High | Maintain oracle parity gate in CI; refactor in small steps with oracle checked at each step |
| Trace-step ID alignment may require oracle-data fixes | Medium | Track divergences in `KNOWN_DIFFERENCES.md`; align internally and document |
| Vedic scope explosion | Low | Treat Track G as explicitly optional; classical-Sanskrit completion is at 272h |

---

## Notes on what is intentionally excluded

The following are **out of scope** even for "complete coverage":
- Lexical semantics (meanings) — only morphology / phonology.
- Sentence-level syntax — kāraka assignment is partially handled but
  full parsing isn't a Pāṇinian-grammar goal.
- Translation, interlinear glossing, NLP-ready exports — separate
  toolchain.
- Comparative Sanskrit (e.g. Buddhist hybrid Sanskrit) — only the
  Aṣṭādhyāyī scope is in.
- Performance optimisation — current ~ms-per-derivation latency is
  fine for offline use.
