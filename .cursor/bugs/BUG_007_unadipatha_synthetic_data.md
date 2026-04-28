# BUG-007 — `ingest_unadipatha.py` uses synthetic rows; real Uṇādi data unused

**Severity:** LOW (no downstream code reads it yet)  
**Effort:** 1 hour  
**Files affected:** `tools/ingest_unadipatha.py`, `data/unadipatha.tsv`  
**Depends on:** —

---

## Symptom

`tools/ingest_unadipatha.py` does not read the real Uṇādi data already committed
at `vendor/unaadi_fallback.json` (748 sūtras with sūtra text, suffix, sk
commentary, sūtra references). Instead, it builds a TSV with:

- 6 hand-picked seed rows (`vAyu`, `jAnu`, `banDu`, `manas`, `tapas`, `smfti`)
- 220 synthetic placeholder rows: `r001us`, `r002as`, `r003ti`, …, `r220as`

```python
def build_rows():
    rows = list(SEED_ROWS)  # 6 seeds
    rid = len(rows) + 1
    for i in range(1, 221):  # synthetic extension
        root = f"r{i:03d}"
        suffix = "u" if i % 3 == 0 else ("as" if i % 3 == 1 else "ti")
        form = f"{root}{suffix}"
        meaning = f"synthetic entry {i}"
        sutra = f"unadi_x.{i}"
        rows.append((rid, root, suffix, form, meaning, sutra))
        rid += 1
    return rows
```

The `--validate` step checks `count >= 100`, which trivially passes.

This is a minor bug because nothing downstream currently consumes
`unadipatha.tsv` for real derivation. But it sets a bad precedent and the
Uṇādi sūtras will be needed for Story 5.3 (kṛt) and beyond.

## Required fix

Rewrite `tools/ingest_unadipatha.py` to actually parse `vendor/unaadi_fallback.json`:

```python
import json
from devanagari_slp1 import deva_to_slp1

def fetch_data():
    with open(FALLBACK, encoding='utf-8') as f:
        return json.load(f)

def generate(data):
    entries = data.get('data', [])
    with open(OUTPUT_TSV, 'w', encoding='utf-8', newline='') as f:
        writer = csv.writer(f, delimiter='\t')
        writer.writerow(['unadi_id', 'sutra_deva', 'sutra_slp1',
                          'pratyay_deva', 'pratyay_slp1',
                          'sk_commentary', 'sutra_ref'])
        for entry in entries:
            uid = int(entry['i'])
            sutra_deva = unicodedata.normalize('NFC', entry.get('sutra', ''))
            sutra_slp1 = deva_to_slp1(sutra_deva)
            pratyay_deva = unicodedata.normalize('NFC', entry.get('pratyay', ''))
            pratyay_slp1 = deva_to_slp1(pratyay_deva)
            sk = entry.get('sk', '')
            writer.writerow([uid, sutra_deva, sutra_slp1,
                              pratyay_deva, pratyay_slp1, sk, ''])
```

The real schema (verified by inspection):
```json
{ "name": "unaadi", "data": [
    { "i": "1001",
      "sutra": "कृवापाजिमिस्वदिसाध्यशूभ्य उण्",
      "pratyay": "उण्",
      "sk": "करोतीति कारुः ..."
    }, ...  748 entries
]}
```

## Acceptance Criteria

- [ ] Replace `SEED_ROWS` and synthetic-row builder with `fetch_data()` reading `vendor/unaadi_fallback.json`
- [ ] Use `from devanagari_slp1 import deva_to_slp1` (no duplicated tables)
- [ ] Output TSV has all 748 real Uṇādi entries
- [ ] Schema columns: `unadi_id, sutra_deva, sutra_slp1, pratyay_deva, pratyay_slp1, sk_commentary, sutra_ref`
- [ ] `--validate` passes against `len(data) == 748`, not `count >= 100`
- [ ] Spot-check: first row's pratyay should be `उण्` → SLP1 `uR`
- [ ] All 27 existing test suites still pass
- [ ] `make validate-phase0` passes
