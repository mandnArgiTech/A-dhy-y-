# Story 0.4 — Gaṇapāṭha Ingestion

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Medium  
**Estimated time:** 1.5 hours  
**Depends on:** Story 0.2

---

## Objective

Parse the Gaṇapāṭha — the named lists of words referenced by sūtras using notations like "svarādiṣu paṭhitam" (listed in the svarādi group). These are auxiliary to the Aṣṭādhyāyī; Pāṇini refers to ~260 named lists without giving their contents inline.

---

## Background

When a sūtra says "ādyantau ṭakitau" or references "gargādi", it is pointing to a named list in the Gaṇapāṭha. The sūtra engine (Story 2.1) needs to resolve these references at runtime.

Key named gaṇas include:
- **svarādi**: words that take accent independently
- **gargādi**: gotra (lineage) names for taddhita
- **śivasūtrāṇi**: the 14 Māheśvara-sūtras (phoneme groups)
- **akādi**: roots taking certain suffixes
- **pratipadika-specific lists**: hundreds of specific word groups

---

## Tasks

### 1. Fetch Gaṇapāṭha data

Source:
```
https://raw.githubusercontent.com/ashtadhyayi-com/data/master/ganapatha/ganapatha.json
```

Fall back to `vendor/ganapatha_fallback.json`.

### 2. Create `tools/ingest_ganapatha.py`

Output `data/ganapatha.tsv` with columns:
```
gana_id  gana_name_slp1  gana_name_iast  serial  member_slp1  member_iast  note
```

- `gana_id`: unique integer per named list
- `gana_name_slp1`: e.g. "svarAdi", "gargAdi"
- `serial`: 1-based position within the list
- `member_slp1`: the list member in SLP1
- `note`: traditional note if any (e.g. "iti vaktavyam" = "should be stated")

### 3. Validate

`--validate` must assert:
- At least 260 distinct `gana_name` values
- "svarAdi" gaṇa present with ≥ 20 members
- "gargAdi" gaṇa present
- No empty `member_slp1` fields

### 4. Māheśvara-sūtras as special gaṇa

Add the 14 Māheśvara-sūtras as a gaṇa named `"mAheSvarasUtra"`:
```
1  a i u N
2  f x k
3  e o N
4  E O c
5  h y v r w
6  l N
7  Y m G R n m
8  J B Y
9  G Q z
10 j b g q d S
11 K P C W T c w t v
12 k p y
13 S z s r
14 h l
```
(Each sūtra is one member, space-separated phonemes.)

### 5. Output files

```
data/ganapatha.tsv           ← Primary output
vendor/ganapatha_fallback.json
```

---

## Acceptance Criteria

- [ ] `python3 tools/ingest_ganapatha.py --validate` prints "PASS: N gaṇas, M members"
- [ ] ≥ 260 distinct gaṇa names
- [ ] Māheśvara-sūtra gaṇa present with exactly 14 members
- [ ] "svarAdi" gaṇa has ≥ 20 members
