# Story 0.3 — Dhātupāṭha Ingestion

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Easy (script already implemented)**  
**Estimated time:** 30 min (run + verify)  
**Depends on:** Story 0.2

---

## Objective

Run `tools/ingest_dhatupatha.py`, verify output, and confirm the dhātu data is correct.

---

## Source

```
github.com/ashtadhyayi-com/data  →  dhatu/data.txt
```

**Real JSON schema** inside `dhatu/data.txt`:
```json
{ "name": "dhatu", "data": [
    { "i": "1010001",
      "baseindex": "01.0001",     ← gana.serial (e.g. "01.0001" = gaṇa 1, root 1)
      "dhatu": "भू",              ← display form (Devanāgarī)
      "aupadeshik": "भू",         ← UPADESA form ← USE THIS for SLP1 conversion
      "gana": "1",                ← gaṇa number 1–10
      "pada": "P",                ← P=parasmai A=ātmane U=ubhaya
      "settva": "S",              ← S=seṭ A=aniṭ V=veṭ
      "artha": "सत्तायाम्",       ← Sanskrit meaning
      "artha_english": "to exist, to become..."
    }, ...
  ]
}
```
Total: **2,259 dhātus** (this edition includes extended forms beyond traditional 1,943).

The vendor fallback is pre-committed at `vendor/dhatupatha_fallback.json`.

---

## Tasks

### 1. Run ingestion

```bash
python3 tools/ingest_dhatupatha.py
```

Expected:
```
Generated data/dhatupatha.tsv: 2259 dhātus
```

### 2. Validate

```bash
python3 tools/ingest_dhatupatha.py --validate
```

Expected:
```
PASS: 2259 dhātus loaded across 10 gaṇas ✓
```

### 3. Spot-check key roots

```bash
# bhū (gaṇa 1, root 1)
grep 'भू' data/dhatupatha.tsv | head -2

# gam (stored as गमॢँ in aupadeshik form)  
grep 'गमॢ' data/dhatupatha.tsv | head -2

# cur (gaṇa 10)
grep 'चुर्' data/dhatupatha.tsv | head -2
```

Key roots to verify:

| Devanāgarī | aupadeshik SLP1 | Gaṇa | Meaning |
|-----------|----------------|------|---------|
| भू | BU | 1 | to be, exist |
| गमॢँ | gamॢM (approx) | 1 | to go |
| दिव् | div | 4 | to shine, play |
| तुद | tud | 6 | to strike |
| चुर् | cur | 10 | to steal |

**Note:** `gam` is stored as `गमॢँ` (with ḷ vowel marker and nasal) in the aupadeshik column.
The SLP1 for this root that the C engine uses will be `gamxM` or similar depending on the
converter. The C library's `pipeline_find_dhatu()` must handle this — see Story 5.1.

---

## TSV Column Reference

| Column | Notes |
|--------|-------|
| `global_id` | Sequential 1-based |
| `gana` | 1–10 |
| `serial_in_gana` | Position within gaṇa |
| `upadesa_deva` | Devanāgarī upadesa (aupadeshik field) |
| `upadesa_slp1` | SLP1 of upadesa |
| `meaning_skt` | Sanskrit meaning (artha field) |
| `meaning_en` | English meaning |
| `pada_flag` | P / A / U |
| `settva` | S / A / V |

---

## Acceptance Criteria

- [ ] `python3 tools/ingest_dhatupatha.py` exits 0
- [ ] `python3 tools/ingest_dhatupatha.py --validate` prints "PASS"
- [ ] `data/dhatupatha.tsv` has 2259 rows
- [ ] bhū in gaṇa 1 found; cur in gaṇa 10 found
- [ ] `vendor/dhatupatha_count.h` defines `ASH_DHATU_COUNT 2259`
