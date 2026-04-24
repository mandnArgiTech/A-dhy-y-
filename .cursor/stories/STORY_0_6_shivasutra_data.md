# Story 0.6 — Śiva-Sūtra Source Data Reference

**Phase:** 0 — Data Reference  
**Difficulty:** None (documentation only)  
**Depends on:** Story 0.4

---

## The 14 Māheśvara (Śiva) Sūtras — Confirmed from Source

Source: `github.com/ashtadhyayi-com/data/shivasutra/data.txt`

The file is JSON with schema:
```json
{ "name": "shivasutra", "data": [
    { "id": "1",
      "sutra": "अइउण्",
      "kashika": "...(full Kāśikā commentary)...",
      "vyakhya": "...(modern Sanskrit explanation)..."
    },
    ...14 entries total
]}
```

### The 14 Sūtras (Devanāgarī → SLP1)

| # | Devanāgarī | SLP1 | Real phonemes | It-marker | Pratyāhāras enabled |
|---|-----------|------|--------------|-----------|---------------------|
| 1 | अइउण् | aiuR | a i u | Ṇ | aṆ, aK |
| 2 | ऋऌक् | fxk | ṛ ḷ | K | iK, uK, aK extension |
| 3 | एओङ् | eoN | e o | Ṅ | eṄ |
| 4 | ऐऔच् | EOc | ai au | C | aC, iC, eC, aiC |
| 5 | हयवरट् | hyvRw | h y v r | Ṭ | aṬ |
| 6 | लण् | lR | l | Ṇ | aṆ extension, iṆ, yṆ |
| 7 | ञमङणनम् | YmNRnm | ñ m ṅ ṇ n | M | aM, yM, ṅM, ñM |
| 8 | झभञ् | JBY | jh bh | Ñ | yÑ |
| 9 | घढधष् | GQDz | gh ḍh dh | Ṣ | jhṢ, bhṢ |
| 10 | जबगडदश् | jbgqdS | j b g ḍ d | Ś | aŚ, haŚ, vaŚ, jaŚ, jhŚ, baŚ |
| 11 | खफछठथचटतव् | KPCWTcwtv | kh ph ch ṭh th c ṭ t | V | chaV |
| 12 | कपय् | kpy | k p | Y | yY, mY, jhY, khaY, caY |
| 13 | शषसर् | Szsr | ś ṣ s | R | yaR, jhaR, khaR, caR, śaR |
| 14 | हल् | hl | h | L | aL, haL, vaL, raL, jhaL, śaL |

### SLP1 Notes

In the source data, the SLP1 of the Devanāgarī sūtras uses the Devanāgarī→SLP1 table
from `AGENTS.md`. Key mappings to verify:

- `ञ` → `Y` (not `J` — ñ is palatal nasal)
- `ङ` → `N` (velar nasal)  
- `ण` → `R` (retroflex nasal)
- `ऋ` → `f`
- `ऌ` → `x`
- `ष` → `z`
- `श` → `S`

### Inferred Pratyāhāra Members

From the sequence (real phonemes only, in order):
```
a i u  ṛ ḷ  e o  ai au  h y v r  l  ñ m ṅ ṇ n  jh bh  gh ḍh dh  j b g ḍ d
kh ph ch ṭh th c ṭ t  k p  ś ṣ s  h
```

(h appears twice: once in sūtra 5 for aT, aŚ, haŚ; once in sūtra 14 for haL, jhaL, etc.)

### Cross-validation against data/pratyahara.tsv

The ingested pratyāhāra data shows:
- `ac` = 9 members (a i u f x e o E O)
- `hl` = 34 members
- `jS` = 5 members (j b g q d)
- `Kr` = 13 members

These come directly from the ashtadhyayi.com editorial team's computation
and serve as the authoritative expansion for Story 1.2's C implementation.

---

## Implementation Note for Story 1.2

The C pratyāhāra engine must produce expansions that match `data/pratyahara.tsv`.
The Māheśvara-sūtra sequence above is the algorithm; the TSV is the oracle.
Run this test after implementing:

```bash
python3 -c "
import csv, subprocess
with open('data/pratyahara.tsv') as f:
    reader = csv.DictReader(f, delimiter='\t')
    for row in reader:
        label = row['label_slp1']
        expected_n = int(row['count'])
        # Call C binary or check compiled test
        print(f'{label}: expected {expected_n} members')
"
```
