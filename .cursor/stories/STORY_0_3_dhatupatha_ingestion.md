# Story 0.3 — Dhātupāṭha Ingestion

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Medium  
**Estimated time:** 2 hours  
**Depends on:** Story 0.2 (ingestion tooling pattern established)

---

## Objective

Parse the complete Dhātupāṭha (list of ~1,943 verbal roots across 10 gaṇas) into a clean TSV that the C library loads at runtime. This is the primary input to all tiṅanta (verbal) derivation.

---

## Background

The Dhātupāṭha is a supplementary text to the Aṣṭādhyāyī listing all Sanskrit verb roots (dhātus), grouped into 10 classes (gaṇas) based on their present-stem formation. Each entry carries:
- The root itself in upadesa form (with anubandhas / it-letters)
- The gaṇa (class 1–10)
- A brief Sanskrit meaning
- English gloss (from secondary sources)

The 10 gaṇas:
1. **Bhvādi** (like bhū "to be") — a-class, guṇa
2. **Adādi** (like ad "to eat") — athematic
3. **Juhvādi** (like hu "to sacrifice") — reduplicating
4. **Divādi** (like div "to shine") — ya-class
5. **Svādi** (like su "to press") — nu/nv-class
6. **Tudādi** (like tud "to push") — a-class, no guṇa
7. **Rudhādi** (like rudh "to obstruct") — nasal infix
8. **Tanādi** (like tan "to stretch") — u-class
9. **Kryādi** (like krī "to buy") — nā-class
10. **Curādi** (like cur "to steal") — aya-class (causative-like)

---

## Tasks

### 1. Fetch Dhātupāṭha data

Primary source (JSON, ashtadhyayi.com):
```
https://raw.githubusercontent.com/ashtadhyayi-com/data/master/dhatupatha/dhatupatha.json
```

Secondary cross-reference:
```
https://sanskrit.inria.fr/DATA/DHAT.zip  (Gerard Huet's Heritage site)
```

Fall back to bundled `vendor/dhatupatha_fallback.json` if remote unavailable.

### 2. Create `tools/ingest_dhatupatha.py`

```python
#!/usr/bin/env python3
"""
ingest_dhatupatha.py — Parse Dhātupāṭha into TSV.

Usage:
  python3 tools/ingest_dhatupatha.py           # generate data/dhatupatha.tsv
  python3 tools/ingest_dhatupatha.py --validate # validate existing TSV
"""
```

Output `data/dhatupatha.tsv` with columns:
```
global_id  gana  serial_in_gana  upadesa_iast  upadesa_slp1  root_clean_slp1  it_flags  meaning_skt  meaning_en  pada_flag
```

Column definitions:
- `global_id`: 1-based monotonic across all gaṇas
- `gana`: 1–10
- `serial_in_gana`: 1-based within the gaṇa
- `upadesa_iast`: full upadesa form with anubandhas (IAST)
- `upadesa_slp1`: full upadesa in SLP1
- `root_clean_slp1`: upadesa with all it-letters stripped (Story 2.3 implements the strip; here just copy upadesa_slp1 for now and mark TODO)
- `it_flags`: comma-separated it-letters found (e.g. "a,n" for anit; "s" for set; blank for anit)
- `meaning_skt`: Sanskrit meaning (short)
- `meaning_en`: English gloss
- `pada_flag`: `P` (parasmaipada only), `A` (ātmanepada only), `U` (ubhaya)

### 3. Validate output

`--validate` mode must assert:
- Total rows = 1943 (traditional count) ± 5 (edition variance)
- All `gana` values 1–10
- Gaṇa 1 (bhvādi) has ≥ 1000 roots
- Gaṇa 10 (curādi) has ≥ 300 roots
- Root "BU" (bhū) present in gaṇa 1 with meaning "sattāyām" (existence)
- Root "gam" present in gaṇa 1
- Root "kf" (kṛ) present in gaṇa 8 (tanādi)
- Root "cur" present in gaṇa 10

### 4. Create C header

Output `vendor/dhatupatha_count.h`:
```c
/* Auto-generated — do not edit */
#define ASH_DHATU_COUNT <N>
#define ASH_GANA_COUNT  10
```

---

## Spot-Check Table

| global_id approx | Gaṇa | Upadesa SLP1 | Meaning EN |
|-----------------|------|-------------|-----------|
| 1 | 1 | BU | to be, exist |
| ~20 | 1 | gam | to go |
| ~100 | 1 | pat | to fall |
| ~700 | 2 | ad | to eat |
| ~800 | 3 | hu | to sacrifice |
| ~900 | 4 | dIv | to shine, play |
| ~1000 | 5 | su | to press, extract |
| ~1100 | 6 | tud | to strike, push |
| ~1200 | 7 | ruD | to obstruct |
| ~1300 | 8 | tan | to stretch |
| ~1400 | 9 | krI | to buy |
| ~1500 | 10 | cur | to steal |

---

## Output Files

```
data/dhatupatha.tsv        ← Primary output (tracked in git)
vendor/dhatupatha_count.h  ← C header with count constants
vendor/dhatupatha_fallback.json  ← Bundled source (committed)
```

---

## Acceptance Criteria

- [ ] `python3 tools/ingest_dhatupatha.py` completes without exceptions
- [ ] `python3 tools/ingest_dhatupatha.py --validate` prints "PASS: N dhātus loaded across 10 gaṇas"
- [ ] `data/dhatupatha.tsv` has 1938–1948 rows
- [ ] All spot-check roots present with correct gaṇa and meaning
- [ ] `vendor/dhatupatha_fallback.json` committed so ingestion works offline
