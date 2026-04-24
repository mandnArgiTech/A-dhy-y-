# Aṣṭādhyāyī — Complete C Library (`libAshtadhyayi`)

A 100% authentic, modular C implementation of Pāṇini's Aṣṭādhyāyī — the world's first formal generative grammar (~500 BCE), covering all 3,983 sūtras of the BORI edition.

## Source Authority & Reference Oracles

| Resource | Role | Size |
|----------|------|------|
| [mandnArgiTech/A-dhy-y-](https://github.com/mandnArgiTech/A-dhy-y-) | **This repo** — C implementation | — |
| [ashtadhyayi-com/data](https://github.com/ashtadhyayi-com/data) | Primary data + reference oracles | |
| &nbsp;&nbsp;`sutraani/data.txt`      | 3,983 sūtras (BORI edition) | 3.6 MB |
| &nbsp;&nbsp;`shivasutra/data.txt`    | 14 Māheśvara-sūtras + Kāśikā | 98 KB |
| &nbsp;&nbsp;`pratyahara/data.txt`    | 52 pratyāhāra expansions | 14 KB |
| &nbsp;&nbsp;`dhatu/data.txt`         | 2,259 dhātus with meanings | 2.4 MB |
| &nbsp;&nbsp;`ganapath/data.txt`      | 262 gaṇas | 175 KB |
| &nbsp;&nbsp;**`shabda/data2.txt`**   | **9,007 prātipadikas × 24 forms = 216,168 subanta forms (ORACLE)** | 9.8 MB |
| &nbsp;&nbsp;**`dhatu/dhatuforms_*.txt`** | **2,229 dhātus × 10 lakāras × 2 padas × 9 = 254,736 tiṅanta forms (ORACLE)** | 9.3 MB |
| &nbsp;&nbsp;**`shabda/shabdaprakriya.txt`** | **4,863 step-by-step prakriyā traces with sūtra IDs (ORACLE)** | 3.0 MB |
| &nbsp;&nbsp;`unaadi/data.txt`        | 748 Uṇādi sūtras | 204 KB |
| &nbsp;&nbsp;`krut/pratyay.txt`       | 129 kṛt suffixes | 22 KB |
| &nbsp;&nbsp;`krut/prakruti.txt`      | 444 kṛt derivational entries | 55 KB |
| &nbsp;&nbsp;`linganushasanam/data.txt`       | 189 liṅgānuśāsana sūtras | 62 KB |
| &nbsp;&nbsp;`paribhashendushekhar/data.txt`  | 133 paribhāṣās with commentary | 337 KB |

All data files have Devanāgarī primary form + metadata. Our ingestion scripts convert
Devanāgarī → SLP1 and produce TSVs in `data/` for C consumption.

## Architecture

```
libAshtadhyayi/
├── core/
│   ├── phonology/     # Varṇa, Pratyāhāra (52 pratyāhāras from oracle)
│   ├── sutrapatha/    # 3,983 sūtras with type classification
│   ├── metadata/      # Saṃjñā, Anubandha, Anuvṛtti, Adhikāra, Paribhāṣā (133)
│   └── conflict/      # Rule priority: apavāda > nitya > antaraṅga > paratva
├── ancillary/
│   ├── dhatupatha/    # 2,259 dhātus with pre-computed forms oracle
│   ├── ganapatha/     # 262 gaṇas
│   ├── unadipatha/    # 748 Uṇādi sūtras
│   └── linganushasana/# 189 gender rules
├── prakriya/
│   ├── tinanta/       # Verbal conjugation (validated against 254,736-form oracle)
│   ├── subanta/       # Nominal declension (validated against 216,168-form oracle)
│   ├── krit/          # 129 primary kṛt derivatives
│   └── taddhita/      # Secondary taddhita derivatives
├── sandhi/            # Vowel, consonant, visarga sandhi
├── samasa/            # 6 compound types
├── encoding/          # SLP1 ↔ IAST ↔ Devanāgarī ↔ HK codec
└── tests/             # Phase-gated validation harness
```

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build
```

Current state: 12/12 stub tests passing. All 21 public API functions implemented.

## Quick Demo

```c
#include <ashtadhyayi.h>

ASH_DB *db = ash_db_load("data/");
ASH_Form f  = ash_tinanta(db, "BU", 1, ASH_LAT,
                           ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
printf("%s\n", f.iast);          // → "bhavati"
ash_form_print_prakriya(&f, stdout);
ash_form_free(&f);
ash_db_free(db);
```

## Implementation Phases

| Phase | Focus | Stories | Gate |
|-------|-------|---------|------|
| 0 | Bootstrap & Data Ingestion | 5 | `make validate-phase0` |
| 1 | Phonology Engine | 5 | `make validate-phase1` |
| 2 | Sūtra Engine & Saṃjñā | 6 | `make validate-phase2` |
| 3 | Pratyaya System & laṭ Tinanta | 6 | `make validate-phase3` |
| 4 | Subanta Declension | 5 | `make validate-phase4` |
| 5 | Pipeline, Samāsa, Kṛt, Taddhita | 5 | `make validate-phase5` |
| 6 | Validation vs Reference Oracles & API Polish | 6 | `make validate-phase6` |

## Encoding Convention

All internal processing uses **SLP1** (ASCII). Conversion to IAST / Devanāgarī happens only at output boundaries via `encoding/encoding.h`.

## Validation Strategy

Instead of comparing against external oracles (vyakarana, scl), we validate against
the **pre-computed reference oracles** from the same source repository:

- **Phase 3 tiṅanta** → `data/dhatuforms.tsv` (254,736 forms). Target ≥99% match.
- **Phase 4 subanta** → `data/shabda_forms.tsv` (216,168 forms). Target ≥99% match.
- **Prakriyā traces** → `data/shabdaprakriya.tsv` (4,863 traces). Target ≥70% sūtra
  attribution match per step; ≥95% final-form match.

These oracles derive from the same Pāṇinian rules our library implements, so
discrepancies indicate bugs in our implementation (not legitimate grammatical ambiguity).

## License

MIT — Sanskrit grammar belongs to humanity.
