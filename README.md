# Aṣṭādhyāyī — Complete C Library (`libAshtadhyayi`)

A 100% authentic, modular C implementation of Pāṇini's Aṣṭādhyāyī — the world's first formal generative grammar (~500 BCE), covering all 3,983 sūtras of the BORI edition.

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
| **`shabda/data2.txt`** | **216,168** (9,007 × 24) | Phase 4 subanta | ≥99% match |
| **`dhatu/dhatuforms_*.txt`** | **254,736** (2,229 × 10 × 2 × 9) | Phase 3 tiṅanta | ≥99% match |
| **`shabda/shabdaprakriya.txt`** | **13,456 steps** (4,863 derivations) | Trace validation | ≥70% sūtra match per step |

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
