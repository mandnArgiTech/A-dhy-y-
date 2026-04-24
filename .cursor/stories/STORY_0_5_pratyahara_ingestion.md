# Story 0.5 — Pratyāhāra Ingestion (Bonus Phase 0 story)

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Easy  
**Estimated time:** 30 minutes  
**Depends on:** Story 0.1

---

## Objective

Parse the pratyāhāra expansion data from the source repo into a TSV that
Story 1.2 can use as the ground-truth for its hardcoded table, and that
tests can validate against.

---

## Source

`github.com/ashtadhyayi-com/data` — `pratyahara/data.txt`

**Real schema** (confirmed):
```json
{ "name":"pratyahara", "data": [
    { "name": "अण्",
      "list": "अ, इ, उ",
      "sutra": "<<ढ्रलोपे...>>",
      "sutranum": "[[6.3.111]]"
    }, ...  52 entries
]}
```

The tool is already implemented at `tools/ingest_pratyahara.py`.

---

## All 52 Pratyāhāras (from actual repo data)

| SLP1 label | Members | Count |
|-----------|---------|-------|
| aR | a i u | 3 |
| ak | a i u f x | 5 |
| ik | i u f x | 4 |
| uk | u f x | 3 |
| eN | e o | 2 |
| ac | a i u f x e o E O | **9** (vowels) |
| ic | i u f x e o E O | 8 |
| ec | e o E O | 4 |
| Ec | E O | 2 |
| aw | a i u f x e o E O h y v r | 13 |
| aR | a i u f x e o E O h y v r l | 14 |
| iR | i u f x e o E O h y v r l | 13 |
| yR | y v r l | 4 |
| am | a...n (all vowels + antastha + nasals + h) | 19 |
| ym | y v r l Y m N R n | 9 |
| Nm | N R n | 3 |
| Ym | Y m N R n | 5 |
| yY | y v r l Y m N R n J B | 11 |
| Jz | J B G Q D | 5 |
| Bz | B G Q D | 4 |
| aS | (vowels + h + semivowels + nasals + voiced stops) | 29 |
| hS | h y v r l ... j b g q d | 20 |
| vS | v r l ... j b g q d | 18 |
| JS | J B G Q D j b g q d | 10 |
| jS | j b g q d | **5** (voiced non-aspirate stops = jaŚ) |
| bS | b g q d | 4 |
| Cv | C W T c w t | 6 |
| yy | all non-sibilant consonants + semivowels | 29 |
| my | m N R n ... k p | 24 |
| Jy | J B G Q D ... k p | 20 |
| Ky | K P C W T c w t k p | 10 |
| cy | c w t k p | 5 |
| yr | all consonants except sibilants and h | 32 |
| Jr | J B G Q D ... S z s | 23 |
| Kr | K P C W T c w t k p S z s | **13** (khar = voiceless) |
| cr | c w t k p S z s | **8** (car = voiceless non-aspirate + sibilants) |
| Sr | S z s | 3 |
| al | all phonemes | **43** |
| hl | all consonants | **34** (hal) |
| vl | all consonants except y | 32 |
| rl | all consonants except y v | 31 |
| Jl | J B G Q D ... h | 24 |
| Sl | S z s h | 4 |
| r | r l | 2 |
| sup | all 21 case endings | 21 |
| suw | first 5 sup | 5 |
| Ap | 15 oblique sup | 15 |
| tiN | all 18 personal endings | **18** |
| tN | 9 ātmanepada endings | 9 |
| kfY | kṛ bhū as | 3 |
| tfn | śānac śānan cānaś śatṛ tṛn | 5 |

**Key corrections vs prior assumptions:**
- `ac` = **9** members (not 14 — long vowels ā ī ū are NOT separate in this listing;
  the repo counts by phoneme classes, not by short/long pairs)
- `hal` = **34** members (not 38)
- `jS` (jaŚ) = **5** members: j b g q d ✓
- `Kr` (khar) = **13** members ✓
- `sup` and `tiN` are also listed here — the phoneme-class pratyāhāras merge with the suffix pratyāhāras

---

## Tasks

### 1. Run the ingestion script
```bash
python3 tools/ingest_pratyahara.py
python3 tools/ingest_pratyahara.py --validate
```

### 2. Update Story 1.2 implementation

Story 1.2 (`STORY_1_2_pratyahara_engine.md`) will implement the C engine.
That engine's internal table must match `data/pratyahara.tsv` exactly.
Write a unit test that loads `data/pratyahara.tsv` and asserts that
`pratyahara_get(label)` returns the same member count and same members
as the TSV for every row.

---

## Acceptance Criteria

- [ ] `python3 tools/ingest_pratyahara.py --validate` prints "PASS: 52 pratyāhāras"
- [ ] `data/pratyahara.tsv` committed to repo
- [ ] `ac` = 9 members, `jS` = 5 members, `hl` = 34 members, `tiN` = 18 members
