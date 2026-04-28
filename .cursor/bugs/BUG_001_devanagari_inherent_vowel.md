# BUG-001 — Devanāgarī → SLP1 converter drops inherent vowel `a`

**Severity:** CRITICAL — corrupts both reference oracles  
**Phase blocked:** 6 (validation), all of Phase 3/4 oracle comparisons  
**Files affected:** `tools/ingest_dhatuforms.py`, `tools/ingest_shabda.py`, `tools/ingest_shabdaprakriya.py`, `tools/ingest_source.py`, `tools/ingest_dhatupatha.py`, `tools/ingest_ganapatha.py`, `tools/ingest_pratyahara.py`, `tools/ingest_unadipatha.py`

---

## Symptom

The Devanāgarī → SLP1 converter shared across all `tools/ingest_*.py` scripts emits each consonant as bare (`k`, `g`, `t` …) without the inherent `a` that every Devanāgarī consonant carries unless followed by a virāma (्) or a vowel mātrā (ा, ि, ु, …).

```python
>>> from ingest_dhatuforms import deva_to_slp1
>>> deva_to_slp1('भवति')
'Bvti'        # WRONG — should be 'Bavati'
>>> deva_to_slp1('रामः')
'rAmH'        # WRONG — should be 'rAmaH'
>>> deva_to_slp1('रामेण')
'rAmeR'       # WRONG — should be 'rAmeRa'
```

This affects every TSV produced from Devanāgarī source, but the impact is most severe in the two reference oracles:

- `data/dhatuforms.tsv` — 254,736 tiṅanta forms, all SLP1 broken
- `data/shabda_forms.tsv` — 216,168 subanta forms, all SLP1 broken
- `data/shabdaprakriya.tsv` — 13,456 prakriyā steps, all step-form SLP1 broken
- `data/sutras.tsv`, `data/dhatupatha.tsv`, `data/ganapatha.tsv` — all impacted to a lesser degree (less critical because we mainly read `text_deva` from these)

## Root cause

In Devanāgarī orthography:
- A bare consonant letter (e.g. क) inherently includes the vowel `a` → SLP1 `ka`
- Virāma (् U+094D) suppresses the inherent vowel → क् → SLP1 `k`
- A vowel mātrā (ा, ि, ु, …) replaces the inherent vowel → का → SLP1 `kA`
- Two consonants in conjunct must be linked by virāma → क् + त = क्त → SLP1 `kta`

The current converter is character-by-character with no lookahead. It maps:
- `क → 'k'`  (drops the inherent `a`)
- `'् → ''`  (just deletes the virāma without using it for context)

## Correct algorithm (single-pass, two-character lookahead)

```
i = 0
while i < len(text):
  ch = text[i]
  next_ch = text[i+1] if i+1 < len(text) else None

  if ch is consonant:
    emit slp1_of(ch)         # e.g. 'k' for क
    if next_ch is virāma:
      i += 2                  # skip virāma; no inherent vowel
    elif next_ch is vowel_matra:
      emit slp1_of(matra)     # vowel replaces inherent a
      i += 2
    else:
      emit 'a'                # default: inherent a
      i += 1
  elif ch is vowel:
    emit slp1_of(ch)
    i += 1
  elif ch is anusvara/visarga/candrabindu:
    emit slp1_of(ch)
    i += 1
  else:
    # punctuation, danda, ZWNJ, etc. — pass through or skip
    i += 1
```

## Reference implementation

This algorithm is **already correctly implemented** in `encoding/encoding.c::enc_slp1_to_devanagari()` (the inverse direction), and `enc_devanagari_to_slp1()` should mirror it. A stand-alone Python module `tools/devanagari_slp1.py` should be created so all ingestion scripts share one implementation.

## Acceptance Criteria

- [ ] Create `tools/devanagari_slp1.py` exporting `deva_to_slp1(text: str) -> str`
- [ ] All 8 `tools/ingest_*.py` scripts import from this module (no duplicated tables)
- [ ] Round-trip tests for 20 representative words pass:
  - `भवति` → `Bavati`
  - `रामः` → `rAmaH`
  - `रामेण` → `rAmeRa`
  - `रामान्` → `rAmAn` (final virāma)
  - `कृष्ण` → `kfzRa` (consonant cluster)
  - `धर्म` → `Darma`
  - `तत्त्व` → `tattva`
  - `विष्णु` → `vizRu`
  - `नमस्कार` → `namaskAra`
  - `अग्नि` → `agni`
  - `मुहूर्त` → `muhUrta`
  - `सूत्र` → `sUtra`
  - `दृष्ट` → `dfzwa`
  - `पृथ्वी` → `pfTvI`
  - `भ्रातृ` → `BrAtf`
  - `स्वामिन्` → `svAmin`
  - `देव` → `deva`
  - `शिव` → `Siva`
  - `गृह` → `gfha`
  - `अहम्` → `aham`
- [ ] Re-run all 8 ingestion scripts; verify TSV outputs
- [ ] Spot-check `data/dhatuforms.tsv` row 1: bhū LAT P-PRATHAMA-EKA must be `Bavati` (not `Bvti`)
- [ ] Spot-check `data/shabda_forms.tsv`: rāma P-prathama-eka must be `rAmaH` (not `rAmH`)
- [ ] All 26 existing test suites still pass
- [ ] Add `tests/unit/test_devanagari_slp1.py` if Python tests are configured, otherwise add as `tools/test_devanagari_slp1.py` runnable via `python3 -m unittest`

## Estimated effort

2 hours
