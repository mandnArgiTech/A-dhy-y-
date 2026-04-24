# Story 0.5 — Reference Oracle Ingestion (shabda, dhatuforms, shabdaprakriya)

**Phase:** 0 — Bootstrap & Data Ingestion  
**Difficulty:** Easy (scripts already implemented)**  
**Estimated time:** 30 min (run + verify)  
**Depends on:** Story 0.1

---

## Objective

The `ashtadhyayi-com/data` repository contains **three massive pre-computed reference
oracles** that are much higher quality than external oracles like `vyakarana` or `scl`.
These were computed by the same Pāṇinian rules our library implements, with the same
authoritative interpretations. We use them as the primary validation oracles for
Phases 3, 4, and the prakriyā trace format.

---

## The Three Reference Oracles

### 1. `shabda/data2.txt` → `data/shabda_forms.tsv`
**9,007 prātipadikas × 24 forms each = 216,168 pre-computed nominal forms**

This is the **primary oracle for Phase 4** (subanta declension). For every
prātipadika in Classical Sanskrit, the exact surface form for all 8 vibhaktis ×
3 vacanas is given, with linga, meaning in Sanskrit/Hindi/English, and grammatical
notes (e.g. `ङिति_ह्रस्वश्च: true`).

Example for रामः (rāma, masculine):
```
राम + प्रथमा-एकवचन  →  रामः
राम + प्रथमा-द्विवचन →  रामौ
राम + प्रथमा-बहुवचन  →  रामाः
राम + द्वितीया-एकवचन →  रामम्
...
```

Replaces the plan of using SCL as the subanta oracle.

### 2. `dhatu/dhatuforms_vidyut_shuddha_kartari.txt` → `data/dhatuforms.tsv`
**2,229 dhātus × 10 lakāras × 2 padas × 9 forms = 254,736 pre-computed tiṅanta forms**

This is the **primary oracle for Phase 3** (tiṅanta conjugation). For every dhātu
(identified by baseindex like `01.0001` for bhū), every lakāra (laT, liT, luT,
lṛT, loT, laṄ, vidhiliṄ, āśīrliṄ, luṄ, lṛṄ) is fully conjugated for both padas
(parasmai/ātmane) across all 9 person-number combinations.

Example for bhū laT parasmai (9 forms):
```
bhavati bhavataḥ  bhavanti   (prathama: 3rd person)
bhavasi bhavathaḥ bhavatha   (madhyama: 2nd person)
bhavāmi bhavāvaḥ  bhavāmaḥ   (uttama: 1st person)
```

Replaces the plan of using `vyakarana` as the tiṅanta oracle.

### 3. `shabda/shabdaprakriya.txt` → `data/shabdaprakriya.tsv`
**4,863 fully-annotated prakriyā traces — every derivation step with sūtra references**

This is the **primary oracle for prakriyā trace format** (Phase 3 + Phase 4
step logging). For each derived form, the exact step-by-step derivation is given,
and every step lists the precise sūtra(s) that fired.

Example for अः (a + prathama-ekavacana):
```
Step 1: अ + स्              sūtras: 1.3.2, 1.3.9        (anubandha stripping of sUP suffix)
Step 2: अ + रुँ             sūtras: 8.2.66              (s → ru̇ at pada-end)
Step 3: अ + र्              sūtras: 1.3.2, 1.3.9        (anubandha stripping of ru̇)
Step 4: अः                  sūtras: 8.3.15              (visarga conversion)
```

Our C engine's `ASH_Form.steps[]` must produce the same sutra_id per step as this oracle.

---

## Tasks

### 1. Run the three ingestion scripts

```bash
python3 tools/ingest_shabda.py
python3 tools/ingest_dhatuforms.py
python3 tools/ingest_shabdaprakriya.py
```

Expected outputs:
```
Generated data/shabda_forms.tsv:       9007 prātipadikas, 216168 forms
Generated data/dhatuforms.tsv:         2229 dhātus, 254736 forms
Generated data/shabdaprakriya.tsv:     4863 prakriyās, 13456 steps
```

### 2. Validate each

```bash
python3 tools/ingest_shabda.py --validate        # PASS: 216168 forms ✓
python3 tools/ingest_dhatuforms.py --validate    # PASS: 254736 forms ✓
python3 tools/ingest_shabdaprakriya.py --validate # PASS: 13456 steps ✓
```

### 3. Spot-check key lookups

```bash
# rāma nom sg should be रामः
grep $'\tराम\t' data/shabda_forms.tsv | grep prathama | grep ekavacana

# bhū present-tense parasmai prathama eka should be भवति
grep $'01.0001\t1\t1\tP\tLAT\tPRATHAMA\tEKA' data/dhatuforms.tsv

# अः derivation should reference sūtra 8.3.15 at the final step  
grep $'^अ\t01.001\tअः' data/shabdaprakriya.tsv | tail -1
```

---

## TSV Column References

### shabda_forms.tsv
```
stem_id | stem_deva | stem_slp1 | linga | vibhakti | vacana | form_deva | form_slp1 | artha_eng
```
- `linga`: PUMS / STRI / NAPUMSAKA / ALL
- `vibhakti`: prathama / dvitiya / tritiya / caturthi / pancami / shasthi / saptami / sambodhana
- `vacana`: ekavacana / dvivacana / bahuvacana

### dhatuforms.tsv
```
baseindex | gana | serial | pada | lakara | purusha | vacana | form_deva | form_slp1
```
- `baseindex`: "GG.NNNN" (gana.serial, e.g. "01.0001" = bhū)
- `pada`: P / A
- `lakara`: LAT / LIT / LUT / LRT / LOT / LAN / VIDHILIM / ASHIRLIM / LUN / LRN
- `purusha`: PRATHAMA / MADHYAMA / UTTAMA
- `vacana`: EKA / DVI / BAHU

### shabdaprakriya.tsv
```
stem_deva | baseindex | form_deva | vibhakti | vacana | step_num | step_form_deva | sutras
```
- `sutras`: comma-separated traditional addresses (e.g. "1.3.2,1.3.9")

---

## How These Oracles Are Used in Later Phases

### Phase 3 (Tiṅanta) validation
Story 6.1's `run_vyakarana_oracle.py` is **replaced** by a much simpler script that
reads `data/dhatuforms.tsv`. For each test form, our library's output is compared
directly against the Devanāgarī form from this TSV. Target: ≥99% match (these are
internal oracles, not external ones — mismatches mean our rules are wrong).

### Phase 4 (Subanta) validation
Story 6.2's `run_scl_oracle.py` is **replaced** by reading `data/shabda_forms.tsv`.
Target: ≥99% match on the most common 50 stems × 24 forms = 1,200 forms.

### Prakriyā trace validation (across Phases 3+4)
The `shabdaprakriya.tsv` provides the **correct sūtra IDs per step**. Our engine's
step log must match. This is tested in a new Story 6.2b:
- Pick 100 common subanta derivations
- For each, compare our step trace (sequence of sutra_ids) against the oracle
- Target: ≥90% exact sequence match (allowing for alternative valid derivations)

---

## Acceptance Criteria

- [ ] All three ingestion scripts run without error
- [ ] `data/shabda_forms.tsv` has ≥ 200,000 rows
- [ ] `data/dhatuforms.tsv` has ≥ 250,000 rows
- [ ] `data/shabdaprakriya.tsv` has ≥ 13,000 rows
- [ ] Spot-checks: rāmaḥ, bhavati, अः all resolve correctly
- [ ] `make validate-phase0` runs all 7 ingestion validators green
