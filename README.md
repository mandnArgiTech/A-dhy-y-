# libAshtadhyayi — A C library for Pāṇinian Sanskrit grammar

A modular C17 library implementing Pāṇini's Aṣṭādhyāyī (~500 BCE). All
ten lakāras (tenses/moods) are implemented; all major nominal classes
(including the full sarvanāma list, demonstratives idam/adas, personal
pronouns asmad/yuṣmad, and numerals 1-10) are wired; the kṛt suffix
inventory covers 59 of the ~129 Pāṇinian suffixes.

## Current implementation status

| Area | Status |
|------|--------|
| **Phonology + encoding** | varṇa, pratyāhāra (52 expansions), SLP1 ↔ IAST ↔ Devanāgarī ↔ HK |
| **Sandhi** | vowel, consonant, visarga + nat va/ṣatva post-process |
| **Sūtra database** | 3,983 BORI-edition sūtras loaded with metadata |
| **Saṃjñā / Anubandha / Adhikāra / Anuvṛtti** | full structures populated |
| **Paribhāṣā** | 8 codified meta-rules + working conflict resolver (apavāda > nitya > antaraṅga > paratva) |
| **Tinanta** | **all 10 lakāras** wired, P + Ā padas, gaṇa 1-10 with full gaṇa-3 reduplication |
| **Subanta** | a-/ā-/i-/ī-/u-/ū-/ṛ-/n-/s-/an-/as-/in-/vat-/j-final/voiceless-stop-final stem classes; full sarvanāma, demonstrative, personal pronoun, numeral paradigms |
| **Kṛt** | **59 suffixes** wired (kta, ktavat, śatṛ, śānac, tavya, anīya, ya, lyap, ktvā, tum, lyuṭ, ghañ, ṇvul, tṛc, ktin, kyap, ṇyat, kvip, ṇamul, ktri, ktu, kmarac, gha, ka, ac, khal, vun, iṣṇuc, ukañ, tavyat, kelimar, ṇvu, man, tṛn, kas, kvasu, kānac, ini, kin, kvasus, atan, ktavatus, nan, ghurac, āluc, atṛn, manin, ra, kit, bhava, vanip, vanac, śacinit, kthan, khishnuc, kha, ktavyan, ktṛp) |
| **Taddhita** | **35 suffixes** (aṇ, ya, in, mat, tā, tva, ka, ika, iya, tama, tara, vat, maya, āna, vya, tal, tval, ini, vini, lac, na, ṭhak, ṭhan, vatup, da, dvayasac, daghnac, mātrac, kalpap, deśīyar, tvan, ṭhal, aiya, dhan, vat2) |
| **Samāsa** | **15 compound sub-types** (six classical + 9 specialized: upapada-tat, naṃ-tat, prādi-tat, gati-tat, daśa-bv, itaretara-dv, samāhāra-dv, vibhakti-tat, upamāna-karm.) |
| **CI** | GitHub Actions matrix (Ubuntu × {gcc, clang}, macOS × clang) + oracle-coverage regression gate |

## Coverage against BORI oracles (50-root sample × 21 slots)

### Tinanta — all 10 lakāras

| Lakāra | P | Ā |
|--------|---|---|
| laṭ | 98% | 92% |
| liṭ | 92% | 82% |
| luṭ | 96% | 92% |
| lṛṭ | 96% | 92% |
| loṭ | 98% | 92% |
| laṅ | 98% | 92% |
| vidhi-liṅ | 98% | 92% |
| āśīr-liṅ | 94% | 92% |
| lṛṅ | 96% | 92% |
| luṅ | 98% | 92% |

LIT 1eka P broader test (all 712 gaṇa-1 P roots): **86.1%**.

### Subanta — 80-stem sample × 21 slots: **96.55%**

- Sarvanāma (22 stems: tad, yad, etad, kim, sarva, viśva, ubha, ubhaya, eka, anya, anyatara, itara, katara, katama, sva, tva, para, antara, apara, avara, uttara, adhara, pūrva, dakṣiṇa): **PUMS 100%, NAPUMSAKA 100%**
- Demonstratives idam, adas (3 liṅgas each): **100%**
- Personal asmad, yuṣmad: **100%**
- Numerals dvi, tri, catur, paJcan, ṣaṣ, saptan, aṣṭan, navan, daśan: **100%**

## Vision

The long-term goal is complete Aṣṭādhyāyī coverage with verifiable
derivations, source-backed data, and prakriyā traces that cite the
sūtra IDs responsible for each step.

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

| File | Forms | Used for |
|------|-------|----------|
| **`dhatu/dhatuforms_*.txt`** | **254,736** (2,229 × 10 × 2 × 9) | Tiṅanta validation |
| **`shabda/data2.txt`** | **216,168** (9,007 × 24) | Subanta validation |
| **`shabda/shabdaprakriya.txt`** | **13,456 steps** (4,863 derivations) | Trace validation |

### Classical commentary (reference only, not loaded at runtime)

| File | Entries | What it is |
|------|---------|-----------|
| `mahabhashyam/1.txt`…`*.txt` | Many | Patañjali's Mahābhāṣya — definitive commentary |
| `vakyapadeeyam/data.txt` | 1,997 | Bhartṛhari's Vākyapadīya — philosophy of language |
| `bhushanasara/data.txt` | 73 | Vaiyākaraṇa Bhūṣaṇa Sāra — philosophical grammar |
| `paramalaghumanjoosha/data.txt` | 14 chapters | Nāgeśa's philosophical treatise |
| `ska/data.txt` | 6,481 | Siddhānta Kaumudī — commentary reorganization |
| `shiksha/data.txt` | 60 | Pāṇini's Śikṣā — phonetics |
| `fit/data.txt` | 87 | Phiṭ-sūtras — accent rules |

All data files have Devanāgarī primary form + metadata. Ingestion
scripts in `tools/ingest_*.py` convert Devanāgarī → SLP1 and produce
TSVs in `data/`.

## Architecture

```
libAshtadhyayi/
├── core/
│   ├── phonology/     # varṇa, pratyāhāra (52 expansions)
│   ├── sutrapatha/    # 3,983 sūtras with type classification
│   ├── metadata/      # saṃjñā, anubandha, anuvṛtti, adhikāra,
│   │                  # paribhāṣā (Phase ε resolver wired)
│   └── conflict/      # apavāda > nitya > antaraṅga > paratva
├── ancillary/
│   ├── dhatupatha/    # 2,259 dhātus with oracle forms
│   ├── ganapatha/     # 262 gaṇas
│   ├── unadipatha/    # 748 Uṇādi sūtras
│   └── linganushasana/# 189 gender rules
├── prakriya/
│   ├── tinanta/       # all 10 lakāras × P/Ā × gaṇa 1-10
│   ├── subanta/       # vowel-stems, cons-stems, pronouns, numerals
│   ├── krit/          # 59 primary kṛt suffixes
│   └── taddhita/      # 35 taddhita suffixes
├── sandhi/            # vowel, cons, visarga, ṇatva, ṣatva
├── samasa/            # 15 compound types
├── encoding/          # SLP1 ↔ IAST ↔ Devanāgarī ↔ HK codec
├── docs/              # docs/API.md — public C API reference
├── .github/workflows/ # CI matrix + oracle regression gate
└── tests/             # 27 unit tests (all passing)
```

## Build

```bash
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Quick Demo

```c
#include <ashtadhyayi.h>

ASH_DB *db = ash_db_load("data/");

/* Tinanta (verbal conjugation) — works for all 10 lakāras */
ASH_Form f = ash_tinanta(db, "BU", 1, ASH_LAT,
                         ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
printf("%s\n", f.iast);    /* → "bhavati" */

/* Subanta (declension) — pronouns, numerals, all stem classes */
ASH_Form s = ash_subanta(db, "tad", ASH_PUMS,
                         ASH_PRATHAMA_VIB, ASH_EKAVACANA);
printf("%s\n", s.slp1);    /* → "saH" */

/* Kṛt (primary derivation) */
ASH_Form k = ash_krit(db, "BU", 1, ASH_KRIT_GHAN);
printf("%s\n", k.slp1);    /* → "BAva" (bhāva) */

ash_form_free(&f); ash_form_free(&s); ash_form_free(&k);
ash_db_free(db);
```

See `docs/API.md` for the full public API reference.

## Implementation Phases

| Phase | Focus | Status |
|-------|-------|--------|
| α | Bootstrap & data ingestion | ✅ complete |
| β | Phonology + sūtra engine + tinanta (all 10 lakāras) | ✅ complete |
| γ | Subanta — vowel/consonant stems, pronouns, numerals | ✅ complete |
| δ | Derivation — kṛt (59), taddhita (35), samāsa (15) | ✅ core complete; remaining ~70 kṛt suffixes incremental |
| ε | Paribhāṣā conflict resolver | ✅ working with apavāda/nitya/antaraṅga registries |
| ζ | API stability, docs, CI | ✅ public API doc, GitHub Actions matrix, oracle gate |

## Paribhāṣā Conflict Resolution

When two sūtras both could fire, `paribhasha_resolve_pair(a, b)` picks
the winner via the classical priority ladder:

1. **apavāda > utsarga** — exception beats general (3.1.69 śyan beats 3.1.68 śap)
2. **nitya > anitya** — always-applicable beats sometimes-applicable
3. **antaraṅga > bahiraṅga** — inner cause beats outer cause
4. **paratva** — later sūtra wins (default tiebreaker)

```c
#include "paribhasha.h"
uint32_t winner = paribhasha_resolve_pair(703084, 703086);  /* → 703086 */
```

## Encoding Convention

All internal processing uses **SLP1** (ASCII). Conversion to IAST /
Devanāgarī happens only at output boundaries via `encoding/encoding.h`.

## Validation Strategy

We validate against the **pre-computed reference oracles** from the
ashtadhyayi-com/data repository:

- **Tiṅanta** → `data/dhatuforms.tsv` (254,736 forms)
- **Subanta** → `data/shabda_forms.tsv` (216,168 forms)
- **Prakriyā traces** → `data/shabdaprakriya.tsv` (13,456 steps)

The CI pipeline (.github/workflows/ci.yml) runs a 20-root LAT-P
regression on every push and fails if the match rate drops below 95%.

## License

MIT — Sanskrit grammar belongs to humanity.
