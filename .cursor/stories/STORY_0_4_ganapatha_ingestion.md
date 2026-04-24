# Story 0.4 — Gaṇapāṭha & Śiva-Sūtra Ingestion

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Easy (scripts already implemented)**  
**Estimated time:** 30 min (run + verify)  
**Depends on:** Story 0.2

---

## Objective

Run `tools/ingest_ganapatha.py` to produce `data/ganapatha.tsv` containing all 262
gaṇas from the Gaṇapāṭha PLUS the 14 Māheśvara-sūtras (Śiva-sūtras) as a special gaṇa.
Then run `tools/ingest_pratyahara.py` to produce `data/pratyahara.tsv`.

---

## Source Files (confirmed by inspection)

```
github.com/ashtadhyayi-com/data
  ├── ganapath/data.txt      ← 262 gaṇas
  └── shivasutra/data.txt    ← 14 Māheśvara-sūtras with full Kāśikā commentary
  └── pratyahara/data.txt    ← 52 pratyāhāras with expansions
```

### ganapath/data.txt schema
```json
{ "name": "ganapath", "data": [
    { "ind": 1,
      "name": "सर्वादिः",    ← gaṇa name (Devanāgarī)
      "sutra": "1.1.27",    ← governing sūtra address
      "words": "सर्व । विश्व । उभ ...",  ← members, separated by । (daṇḍa)
      "type": "P"
    }, ...  262 total
  ]
}
```

### shivasutra/data.txt schema
```json
{ "name": "shivasutra", "data": [
    { "id": "1",
      "sutra": "अइउण्",      ← Devanāgarī text of the Māheśvara-sūtra
      "kashika": "...",       ← Kāśikā commentary (Sanskrit)
      "vyakhya": "..."        ← Modern Sanskrit explanation with dialogue
    }, ...  14 total
  ]
}
```

**The 14 Śiva-sūtras** (confirmed from live data):

| id | Devanāgarī | It-letter | Pratyāhāra enabled |
|----|-----------|-----------|-------------------|
| 1 | अइउण् | ण् | aṆ, aR |
| 2 | ऋऌक् | क् | aK, iK, uK |
| 3 | एओङ् | ङ् | eṄ |
| 4 | ऐऔच् | च् | aC, iC, eC, aiC |
| 5 | हयवरट् | ट् | aT |
| 6 | लण् | ण् | aṆ (extended), iṆ, yṆ |
| 7 | ञमङणनम् | म् | aM, yM, ṅM, ñM |
| 8 | झभञ् | ञ् | yÑ |
| 9 | घढधष् | ष् | jhaS, BaS |
| 10 | जबगडदश् | श् | aŚ, haŚ, vaŚ, jaŚ, jhaŚ, baŚ |
| 11 | खफछठथचटतव् | व् | ChaV |
| 12 | कपय् | य् | yY, mY, jhaY, khaY, caY |
| 13 | शषसर् | र् | yaR, jhaR, khaR, caR, SaR |
| 14 | हल् | ल् | aL, hL, vaL, raL, jhaL, SaL |

### pratyahara/data.txt schema
```json
{ "name": "pratyahara", "data": [
    { "name": "अण्",
      "list": "अ, इ, उ",          ← comma-separated expansion (Devanāgarī)
      "sutra": "<<sūtra text>>",
      "sutranum": "[[6.3.111]]"
    }, ...  52 total
  ]
}
```

**Important member counts** (from actual data):
- `ac` (अच्) = 9 members: a i u ṛ ḷ e o ai au
- `hal` (हल्) = 34 members: all consonants except anusv./visarga
- `ik` (इक्) = 4 members: i u ṛ ḷ

---

## Tasks

### 1. Run ganapatha ingestion

```bash
python3 tools/ingest_ganapatha.py
```

Expected:
```
Generated data/ganapatha.tsv: 251 gaṇas, 5639 members
  Māheśvara-sūtras: 14 entries ✓
```

### 2. Run pratyahara ingestion

```bash
python3 tools/ingest_pratyahara.py
```

Expected:
```
Generated data/pratyahara.tsv: 52 pratyāhāras
```

### 3. Validate both

```bash
python3 tools/ingest_ganapatha.py --validate
python3 tools/ingest_pratyahara.py --validate
```

### 4. Spot-check the Māheśvara-sūtras in ganapatha.tsv

```bash
grep 'mAheSvarasUtra' data/ganapatha.tsv | wc -l   # should be 14
grep 'mAheSvarasUtra' data/ganapatha.tsv | head -3
```

Expected first row (after header):
```
1  mAheSvarasUtra  māheśvarasūtra  shivasutra  1  aiuR  अइउण्
```

### 5. Spot-check pratyahara expansions

```bash
# ac should have 9 members
grep $'\tac\t' data/pratyahara.tsv

# ik should have 4 members
grep $'\tik\t' data/pratyahara.tsv
```

---

## Story 1.2 Bridge Note

Story 1.2 (Pratyāhāra Engine) implements the **computation** of pratyāhāra expansions
from the Māheśvara-sūtra sequence in C. The `data/pratyahara.tsv` from this story
serves as a **cross-validation oracle** for Story 1.2's unit tests — not as the runtime
source (the C code computes expansions at compile time from a static table).

---

## Acceptance Criteria

- [ ] `python3 tools/ingest_ganapatha.py --validate` prints "PASS"
- [ ] `python3 tools/ingest_pratyahara.py --validate` prints "PASS"
- [ ] `data/ganapatha.tsv` has Māheśvara-sūtra gaṇa with exactly 14 entries
- [ ] `data/pratyahara.tsv` has 52 pratyāhāras
- [ ] `ac` pratyāhāra has 9 members; `ik` has 4 members
- [ ] `make validate-phase0` passes all 4 ingestion checks
