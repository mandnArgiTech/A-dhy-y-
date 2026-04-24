# Story 0.2 — Sutrapatha Source Ingestion

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Medium  
**Estimated time:** 2–3 hours  
**Depends on:** Story 0.1 (build system working)

---

## Objective

Parse the raw Sutrapatha text from the source repo and produce a clean, validated TSV file that all subsequent C code will read. This is the **single source of truth** for all 3,959 sūtras.

---

## Source Data

The primary text is in this repo itself. Download the canonical IAST text from GRETIL as the reference:

```
http://gretil.sub.uni-goettingen.de/gretil/1_sanskr/6_sastra/1_gram/paniniiu.htm
```

Additionally use the structured data from:
```
https://raw.githubusercontent.com/ashtadhyayi-com/data/master/sutras/sutra-details.json
```

---

## Tasks

### 1. Create `tools/ingest_source.py`

```python
#!/usr/bin/env python3
"""
ingest_source.py — Parse Ashtadhyayi sutrapatha into TSV.

Usage:
  python3 tools/ingest_source.py           # generate data/sutras.tsv
  python3 tools/ingest_source.py --validate # validate existing TSV
"""
```

The script must:

1. **Fetch** the ashtadhyayi-com JSON (or use a local copy in `vendor/`).
2. **Normalize** all text to Unicode NFC.
3. **Convert** IAST to SLP1 using the mapping in `AGENTS.md`.
4. **Output** `data/sutras.tsv` with columns:
   ```
   global_id  adhyaya  pada  num  text_iast  text_slp1  sutra_type
   ```
   - `global_id`: 1-based integer, canonical order
   - `adhyaya`: 1–8
   - `pada`: 1–4
   - `num`: 1–N within the pada
   - `text_iast`: IAST Unicode (UTF-8)
   - `text_slp1`: SLP1 ASCII transliteration
   - `sutra_type`: one of `SAMJNA|PARIBHASHA|ADHIKARA|VIDHI|NIYAMA|ATIDESHA|NISHEDHA`
     (classify based on known lists; default unknown to `VIDHI`)

5. **Output** `vendor/sutras_count.h`:
   ```c
   /* Auto-generated — do not edit */
   #define ASH_SUTRA_COUNT <N>
   #define ASH_SUTRA_TRADITIONAL_COUNT 3959
   #define ASH_SUTRA_BORI_COUNT 3996
   ```

6. **Validate mode** (`--validate`): read existing TSV and assert:
   - Row count is 3959 or 3996 (print which edition)
   - No empty `text_slp1` fields
   - `global_id` is strictly monotonically increasing
   - All `adhyaya` values 1–8
   - All `pada` values 1–4

### 2. Classify sūtra types

Use this known list (encode as a Python dict in the script):

**Adhikāra sūtras** (partial list — fill in from Laghu Siddhānta Kaumudī):
- 1.1.1 is `SAMJNA` (vṛddhi defined)
- 1.3.1 is `SAMJNA` (bhū-ādi = dhātu)
- 3.1.1 is `ADHIKARA` (pratyayaḥ)
- 3.4.1 is `ADHIKARA` (laḥ karmaṇi ca bhāve cākarmakebhyaḥ)
- 6.1.1 is `ADHIKARA` (ekaḥ pūrvaparayoḥ)
- 8.2.1 is `PARIBHASHA` (pūrvatrāsiddham — asiddha principle)

For all others, default to `VIDHI`.

### 3. Create `data/` directory and run

```bash
mkdir -p data vendor
python3 tools/ingest_source.py
```

### 4. Spot-check 10 specific sūtras

Verify these known sūtras appear correctly in the TSV:

| global_id | Address | Expected IAST (partial) | Expected SLP1 (partial) |
|-----------|---------|------------------------|------------------------|
| 1 | 1.1.1 | vṛddhirādaic | vfdDirAdEc |
| 2 | 1.1.2 | adeṅ guṇaḥ | adeN guRaH |
| 10 | 1.1.10 | nājhnamavidhau | nAjhnamaviDAu |
| 46 | 1.1.46 | ādyantau ṭakitau | AdyantO wAkitO |
| (auto) | 3.1.68 | kartari śap | kartari Sap |
| (auto) | 6.1.77 | iko yaṇ aci | iko yaN aci |
| (auto) | 7.3.84 | sārvadhātukārdhadhātukayoḥ | sArvaDAtukArDADAtukayoH |
| (auto) | 8.2.1 | pūrvatrāsiddham | pUrvAtrAsiddham |

---

## Output Files

```
data/sutras.tsv          ← Primary output (tracked in git)
vendor/sutras_count.h    ← C header with count constants
```

---

## Acceptance Criteria

- [ ] `python3 tools/ingest_source.py` completes without exceptions
- [ ] `python3 tools/ingest_source.py --validate` prints "PASS: N sūtras loaded"
- [ ] `data/sutras.tsv` has ≥ 3959 rows
- [ ] `vendor/sutras_count.h` defines `ASH_SUTRA_COUNT`
- [ ] All 8 spot-check sūtras present with correct content
- [ ] `make validate-phase0` target includes this check

---

## Notes for Cursor

- Use Python's `unicodedata.normalize('NFC', s)` for normalization.
- The SLP1 mapping table is in `AGENTS.md` — do not invent a different scheme.
- If the JSON source is unavailable at runtime, fall back to a bundled copy in `vendor/sutra_fallback.json` (download once and commit it).
- Tab-separate columns; quote fields containing tabs with double-quotes.
- The `sutra_type` column is best-effort for now; it will be refined in Story 2.1.
