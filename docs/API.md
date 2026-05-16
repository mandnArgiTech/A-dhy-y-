# libAshtadhyayi — Public C API

The entire public surface lives in `include/ashtadhyayi.h`. All
identifiers are prefixed `ash_` (functions) or `ASH_` (types/enums).
Strings are SLP1-encoded ASCII unless explicitly named `iast` or
`devanagari`.

## Lifecycle

```c
ASH_DB *db = ash_db_load("data");  // data directory contains TSVs
// ... call derivation functions with `db` ...
ash_db_free(db);
```

`ash_db_load(NULL)` is permitted for some calls that don't consult the
dhātupāṭha, but always pass a real DB for production use — pada-flag
enforcement, sūtra lookup, and other features require it.

## Encodings

```c
char *iast = ash_encode("Bavati", ASH_ENC_SLP1, ASH_ENC_IAST);
char *deva = ash_encode("Bavati", ASH_ENC_SLP1, ASH_ENC_DEVA);
// free(iast); free(deva);
```

Supported encodings: `ASH_ENC_SLP1`, `ASH_ENC_IAST`, `ASH_ENC_DEVA`.

## Tinanta (verbal conjugation)

```c
ASH_Form f = ash_tinanta(db, "BU", 1, ASH_LAT,
                         ASH_PRATHAMA, ASH_EKAVACANA, ASH_PARASMAI);
// f.valid, f.slp1, f.iast, f.devanagari, f.error, f.steps, f.step_count
ash_form_free(&f);
```

### Lakāra coverage (all 10 wired)

| Enum | Sanskrit | Function | P sample | Ā sample |
|------|----------|----------|----------|----------|
| `ASH_LAT` | laṭ (present) | regular | 98% | 92% |
| `ASH_LIT` | liṭ (perfect) | reduplicated | 92% | 82% |
| `ASH_LUT` | luṭ (peri-future) | regular | 96% | 92% |
| `ASH_LRT` | lṛṭ (s-future) | regular | 96% | 92% |
| `ASH_LOT` | loṭ (imperative) | regular | 98% | 92% |
| `ASH_LAN` | laṅ (imperfect) | a-augment | 98% | 92% |
| `ASH_VIDHILIM` | vidhi-liṅ (optative) | regular | 98% | 92% |
| `ASH_ASHIRLIM` | āśīr-liṅ (benedictive) | regular | 94% | 92% |
| `ASH_LRN` | lṛṅ (conditional) | s-future + a-aug | 96% | 92% |
| `ASH_LUN` | luṅ (aorist) | iṣ + root-aorist | 98% | 92% |

(All percentages are against BORI dhātuforms.tsv, first 50 gaṇa-1
roots × 21 slots each.)

### Gaṇa support

Reduplicative gaṇa-3 (juhotyādi) wired; all 9 hu LAT-P forms
oracle-correct (juhoti, juhutaH, juhvati, ...). Other athematic gaṇas
(2, 5, 7, 8) use the LAT_PARASMAI table with athematic glide insertion.

### Paradigm helper

```c
ash_tinanta_paradigm(db, "BU", 1, ASH_LAT, ASH_PARASMAI,
                     /*on_form*/ my_callback,
                     /*userdata*/ NULL);
```

Iterates all 9 (puruṣa, vacana) slots and invokes the callback.

## Subanta (nominal declension)

```c
ASH_Form f = ash_subanta(db, "rAma", ASH_PUMS,
                         ASH_PRATHAMA_VIB, ASH_EKAVACANA);
```

### Stem-class dispatch

The pipeline routes by stem ending and liṅga to a specialized handler:

- a-stem (rāma, vana, kanyā)
- ī/ū-stem (nadī, vadhū, brū-root, pitṛprasū)
- short i/u-stem (mati, dhenu, agni, śiśu)
- consonant-stem (rājan, manas, śṛṅvat, vāggmin)
- voiceless-stop final (gup, marut) and j-final (ākhubhuj)
- ṛ-stem (pitṛ)
- in-stem and as-stem masc/neut
- numeral (dvi, tri, catur, paJcan-daśan, ṣaṣ): 100% oracle
- sarvanāma (tad, yad, etad, kim, sarva + 18 more): 100% oracle
- idam, adas (three liṅgas): 100% oracle
- asmad, yuṣmad (personal): 100% oracle

Overall subanta coverage on the 80-stem BORI sample: 96.55%.

## Kṛt (primary derivation)

```c
ASH_Form f = ash_krit(db, "BU", 1, ASH_KRIT_TRC);  // → "Bavitf"
```

39 kṛt suffixes wired in `ASH_KritType`:

```
KTA KTAVAT SHATR SHANAC TAVYA ANIIYA YA LYAP KTVA TUM LYUT
GHaN Rvul tfc ktin kyap Ryat kvip Ramul
ktri ktu kmarac Ga ka ac Khal vun izṇuc ukañ tavyat
kelimar Rvu man tfn kas kvasu kAnac ini
```

Rule application: 7.2.115/116 vṛddhi for ñit/ṇit, 7.3.84/86 guṇa
for non-kit, 1.1.5 guṇa-block for kit, 7.2.10 iṭ-augment for seṭ,
7.2.11 śryukaH kiti (iṭ blocked for kit on vowel-final roots),
6.1.78 ec→ay at vowel-initial-suffix boundary.

## Taddhita (secondary derivation)

```c
ASH_Form f = ash_taddhita(db, "garga", TD_AN);  // → "gArgya"
```

15 suffixes: aN, ya, in, mat, tā, tva, ka, ika, iya, tama, tara,
vat, maya, ana, vya.

## Samāsa (compound formation)

```c
ASH_Form f = ash_samasa(db, "rAma", "putra", ASH_SAMASA_TATPURUSHA, ASH_PUMS);
```

## Sandhi

```c
char out[64];
ash_sandhi_apply("rAma", "iti", out, sizeof(out), ASH_ENC_SLP1);
// out == "rAmeti"

char *splits[16];
int n = ash_sandhi_split("rAmeti", splits, 16);
```

## Pratyāhāra expansion

```c
const char **chars = ash_pratyahara_expand("aR");
// {"a", "i", "u", NULL}
```

## Paribhāṣā conflict resolution

```c
#include "paribhasha.h"
uint32_t winner = paribhasha_resolve_pair(301068, 301069);
// 301069 — śyan beats śap (apavāda)
```

Available paribhāṣās: PB_STHANIVAD, PB_ASIDDHA_BAHIRANG, PB_PURVA_PATA,
PB_NITYA_OVER_ANITYA, PB_APAVADA_OVER_UTSARGA, PB_ANTARA_OVER_BAHIRA,
PB_SARVADHATUKE_ARDHA, PB_ANGA_ASIDDHA.

Resolution order: apavāda > nitya > antaraṅga > paratva.

## Memory model

- `ASH_Form` results carry malloc'd `steps` arrays — always call
  `ash_form_free(&f)` even if `!f.valid`.
- `ash_encode()` returns a malloc'd string — caller frees.
- `ash_db_load()` returns an opaque handle; `ash_db_free()` to release.
- All other types are by-value structs with embedded fixed-size buffers.

## Thread safety

Read-only operations on the same `ASH_DB *` are safe to call concurrently.
Mutating operations (currently none in the public surface) are not.

## Building

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Linking:

```sh
gcc app.c -I /path/to/include -L /path/to/build -lashtadhyayi
```

A pkg-config file is installed as `libashtadhyayi.pc`.
