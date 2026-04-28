# libAshtadhyayi — A C library for Pāṇinian Sanskrit grammar

A modular C17 library implementing foundations of Pāṇini's Aṣṭādhyāyī (~500 BCE). Full rule coverage is in progress; current releases focus on a traceable engine, data ingestion, phonology, sandhi, and early morphology.

## Current implementation status

The foundation is implemented and tested:

- **Phonology and encoding**: varṇa classification, pratyāhāra lookup, SLP1/IAST/Devanāgarī/HK conversion, vowel/consonant/visarga sandhi.
- **Sūtra database**: all 3,983 BORI-edition sūtras load with address and type metadata.
- **Metadata**: saṃjñā, anubandha, adhikāra, anuvṛtti, and paribhāṣā structures are present.
- **Derivation pipeline**: laṭ tiṅanta and subanta paths are partial and under active validation against bundled oracles.

Open implementation and review work is tracked in `.cursor/stories/` and `.cursor/bugs/`.

## Vision

The long-term goal is complete Aṣṭādhyāyī coverage with verifiable derivations, source-backed data, and prakriyā traces that cite the sūtra IDs responsible for each step.

## Source Authority & Reference Oracles

| Resource | Role | Size |
|----------|------|------|
| [mandnArgiTech/A-dhy-y-](https://github.com/mandnArgiTech/A-dhy-y-) | **This repo** — C implementation | — |
| [ashtadhyayi-com/data](https://github.com/ashtadhyayi-com/data) | Primary data + reference oracles | |

### Core text data (used by engine at runtime)

| File | Entries | Purpose |
|------|---------|---------|
| `sutraani/data.txt` | 3,983 sūtras | Aṣṭādhyāyī (BORI edition) |
| `shivasutra/data.txt` | 14 | Māheśvara-sūtras + Kāśikā commentary |
| `pratyahara/data.txt` | 52 | Pre-computed pratyāhāra expansions |
| `dhatu/data.txt` | 2,259 | Dhātupāṭha with meanings |
| `ganapath/data.txt` | 262 gaṇas | Named lists referenced by sūtras |
| `unaadi/data.txt` | 748 | Uṇādi sūtras (supplementary kṛt) |
| `krut/pratyay.txt` | 129 | Kṛt suffix definitions |
| `krut/prakruti.txt` | 444 | Kṛt derivational entries |
| `linganushasanam/data.txt` | 189 | Gender assignment rules |
| `paribhashendushekhar/data.txt` | 133 | Paribhāṣās with Nāgeśa's commentary |

### Reference oracles (validation targets)

| File | Forms | Target Phase | Validation Target |
|------|-------|--------------|-------------------|
| **`shabda/data2.txt`** | **216,168** (9,007 × 24) | Phase 4 subanta | informational strict match report |
| **`dhatu/dhatuforms_*.txt`** | **254,736** (2,229 × 10 × 2 × 9) | Phase 3 tiṅanta | informational strict match report |
| **`shabda/shabdaprakriya.txt`** | **13,456 steps** (4,863 derivations) | Trace validation | sampled informational report |

### Classical commentary (reference only, not loaded at runtime)

| File | Entries | What it is |
|------|---------|-----------|
| `mahabhashyam/1.txt`…`*.txt` | Many | Patañjali's Mahābhāṣya — THE definitive commentary |
| `vakyapadeeyam/data.txt` | 1,997 | Bhartṛhari's Vākyapadīya — philosophy of language |
| `bhushanasara/data.txt` | 73 | Vaiyākaraṇa Bhūṣaṇa Sāra — philosophical grammar |
| `paramalaghumanjoosha/data.txt` | 14 chapters | Nāgeśa's philosophical treatise |
| `ska/data.txt` | 6,481 | Siddhānta Kaumudī — commentary reorganization |
| `shiksha/data.txt` | 60 | Pāṇini's Śikṣā — phonetics |
| `fit/data.txt` | 87 | Phiṭ-sūtras — accent rules (Phase 7 candidate) |

All data files have Devanāgarī primary form + metadata. Ingestion scripts in
`tools/ingest_*.py` convert Devanāgarī → SLP1 and produce TSVs in `data/`.

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

Current state: unit and validation targets are active, and the public API is wired for concrete runtime behavior.

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

## Phase status

| Phase | Status | Notes |
|-------|--------|-------|
| 0 | [x] Active | Data ingestion regenerates TSVs from bundled fallbacks. |
| 1 | [x] Active | Phonology, encoding, and sandhi tests run in `validate-phase1`. |
| 2 | [x] Active | Sūtra and metadata structures load and test. |
| 3 | [ ] Partial | laṭ tinanta path exists; broader lakāra and root coverage in progress. |
| 4 | [ ] Partial | Several subanta stem classes exist; oracle rates are informational. |
| 5 | [ ] Partial | Pipeline, samāsa, kṛt, taddhita, and uṇādi modules are scaffolded. |
| 6 | [ ] Partial | Validation harness reports strict raw rates; release criteria still evolving. |

See `.cursor/stories/` for story scope and `.cursor/bugs/` for known bug-fix work.

## Encoding Convention

All internal processing uses **SLP1** (ASCII). Conversion to IAST / Devanāgarī happens only at output boundaries via `encoding/encoding.h`.

## Validation Strategy

Instead of comparing against external oracles (vyakarana, scl), we validate against
the **pre-computed reference oracles** from the same source repository:

- **Phase 3 tiṅanta** → `data/dhatuforms.tsv` (254,736 forms), sampled with strict Devanāgarī equality.
- **Phase 4 subanta** → `data/shabda_forms.tsv` (216,168 forms), sampled with strict Devanāgarī equality.
- **Prakriyā traces** → `data/shabdaprakriya.tsv` (4,863 traces), sampled informationally until CLI trace export is complete.

Current oracle scripts are informational while morphology coverage matures; they report raw rates and mismatches without hiding known gaps.

## License

MIT — Sanskrit grammar belongs to humanity.
