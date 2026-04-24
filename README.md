# Aṣṭādhyāyī — Complete C Library (`libAshtadhyayi`)

A 100% authentic, modular C implementation of Pāṇini's Aṣṭādhyāyī — the world's first formal generative grammar (~500 BCE), covering all 3,959 sūtras.

## Source Authority

| Resource | Role |
|----------|------|
| [mandnArgiTech/A-dhy-y-](https://github.com/mandnArgiTech/A-dhy-y-) | **Primary source** (this repo) |
| [sanskrit/vyakarana](https://github.com/sanskrit/vyakarana) | Validation oracle (tinanta) |
| [samsaadhanii/scl](https://github.com/samsaadhanii/scl) | Validation oracle (subanta, morphology) |
| [ashtadhyayi-com/data](https://github.com/ashtadhyayi-com/data) | Structured Dhātupāṭha / Gaṇapāṭha JSON |
| [ashtadhyayi.github.io](https://ashtadhyayi.github.io) | Human-readable sūtra index |
| [GRETIL](http://gretil.sub.uni-goettingen.de/) | Canonical IAST sūtra text |

## Architecture

```
libAshtadhyayi/
├── core/
│   ├── phonology/     # Varṇa, Pratyāhāra, Māheśvara-sūtras
│   ├── sutrapatha/    # 3,959 sūtras — loader, data model
│   ├── metadata/      # Saṃjñā, Anubandha, Anuvṛtti, Adhikāra, Paribhāṣā
│   └── conflict/      # Rule priority: apavāda > nitya > antaraṅga > paratva
├── ancillary/
│   ├── dhatupatha/    # 1,943 dhātus across 10 gaṇas
│   ├── ganapatha/     # Named lists (svarādi, etc.)
│   ├── unadipatha/    # Uṇādi kṛt suffixes
│   └── linganushasana/
├── prakriya/
│   ├── tinanta/       # Verbal conjugation — 10 lakaras
│   ├── subanta/       # Nominal declension — 8 cases × 3 numbers
│   ├── krit/          # Primary kṛt derivatives
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

## Quick Demo

```c
#include <ashtadhyayi.h>

ASH_DB *db = ash_db_load("data/");
ASH_Form f  = ash_tinanta(db, "BU", 1, ASH_LAT,
                           ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
printf("%s\n", f.iast);  // → "bhavati"
ash_form_print_prakriya(&f, stdout);
ash_form_free(&f);
ash_db_free(db);
```

## Implementation Phases

| Phase | Focus | Stories | Gate |
|-------|-------|---------|------|
| 0 | Bootstrap & Data Ingestion | 4 | `make validate-phase0` |
| 1 | Phonology Engine | 5 | `make validate-phase1` |
| 2 | Sūtra Engine & Saṃjñā | 6 | `make validate-phase2` |
| 3 | Pratyaya System & laṭ Tinanta | 6 | `make validate-phase3` |
| 4 | Subanta Declension | 5 | `make validate-phase4` |
| 5 | Pipeline, Samāsa, Kṛt, Taddhita | 5 | `make validate-phase5` |
| 6 | Validation Harness & API Polish | 6 | `make validate-phase6` |

## Encoding Convention

All internal processing uses **SLP1** (ASCII). Conversion to IAST / Devanāgarī happens only at output boundaries via `encoding/encoding.h`.

## Validation

Each phase runs against external oracles:
- `make validate-phase3` → compares 1,000 tinanta forms against `vyakarana` Python output
- `make validate-phase4` → compares 500 subanta forms against `scl` morphological analyzer

## License

MIT — Sanskrit grammar belongs to humanity.
