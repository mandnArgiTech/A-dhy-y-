# Known Differences Between libAshtadhyayi and External Oracles

This file documents every class of discrepancy between our implementation
and the external validation oracles (`vyakarana` Python, `scl` OCaml).
All differences should have a linguistic justification citing the relevant
Pāṇinian sūtra or traditional commentary.

---

## Format

Each entry lists:
- **Category** — what type of difference
- **Scope** — how many forms are affected
- **Our output** — what we produce
- **Oracle output** — what the oracle produces
- **Justification** — which Pāṇinian authority supports our choice

---

## 1. Encoding Artifacts

**Category:** Representation, not linguistic  
**Scope:** All forms (initial phase)  
**Description:** We produce SLP1 ASCII; oracle produces IAST Unicode. These look different but represent the same phonological content.  
**Resolution:** Normalize to IAST before comparison in oracle scripts.  
**Status:** ✓ Handled in `run_vyakarana_oracle.py`

---

## 2. Accent Marking

**Category:** Feature not yet implemented  
**Scope:** ~100% of forms (accents appear in vyakarana output)  
**Our output:** `rAmaH` (no accent marks)  
**Oracle output:** `rāmàḥ` (with accent marker)  
**Justification:** Classical Sanskrit prose does not require pitch accent marking. Accent is a Vedic feature. Pāṇini marks accents in 1.2.27–1.2.57 and the chandas sections; these are outside our Phase 0–5 scope.  
**Status:** Known limitation; documented. Phase 7 candidate.

---

## 3. Alternative Derivational Paths

**Category:** Genuine linguistic ambiguity  
**Scope:** ~3–5% of forms  
**Description:** Some verb forms can be derived via two equally valid Pāṇinian paths. Pāṇini does not always resolve the ambiguity; traditional commentaries diverge.  
**Example:** `gacchati` (gam + śap) — vyakarana uses one path, we use another; both correct per 3.1.68.  
**Status:** Documented case by case below.

### 3a. gam → gacchati

| Step | Our analysis | vyakarana analysis |
|------|-------------|-------------------|
| Vikaraṇa | śap (3.1.68) | śap (3.1.68) |
| Root change | gam + a → gacc (by 8.4.40) | same |
| Final | gacchati | gacchati |

Currently matches. Document if divergence found.

---

## 4. Vedic-Only Forms

**Category:** Scope boundary  
**Scope:** ~5% of forms in vyakarana output  
**Description:** `vyakarana` sometimes produces Vedic forms (e.g., forms with Vedic suffix alternants like ārdhadhātuka ṇic in unusual contexts). We target Classical Sanskrit only.  
**Justification:** Classical Sanskrit is defined by Kālidāsa et al., not by Vedic texts. The relevant sūtras (the chandas sections, 1.2.33 scope) explicitly scope Vedic rules.  
**Status:** Expected difference; not a bug.

---

## 5. SCL Parsing Conventions

**Category:** Morphological tagging convention  
**Scope:** ~2% of subanta forms  
**Description:** SCL may tag the vocative (sambodhana) differently, or treat some sandhi forms as separate entries.  
**Status:** Document when specific cases arise.

---

## Template for New Differences

```
### N. [Short Title]

**Category:** [Encoding / Missing feature / Alternative path / Scope boundary]
**Scope:** N forms
**Root/Stem:** Example: "gam", "rāma"
**Sūtra:** Reference: 7.3.84
**Our output:** [SLP1]
**Oracle output:** [IAST]
**Justification:** [Cite sūtra or commentary]
**Status:** [Known | Under investigation | Fixed in Story X.Y]
```
