# Story 5.7 — Full samāsa types (avyayībhāva, dvandva, bahuvrīhi, dvigu, karmadhāraya, tatpurusa)

**Phase:** 5 — Pipeline, Samāsa, Kṛt, Taddhita
**Difficulty:** Very High
**Estimated time:** 12 hours
**Depends on:** Story 4.10 (a-stem PUMS), Story 4.7 (consonant-stem PUMS)

---

## Objective

The current `samasa/samasa.c` recognises 6 compound types in name
(avyayībhāva, tatpuruṣa, karmadhāraya, dvandva, bahuvrīhi, dvigu)
but produces only string-concatenation of the two stems. This story
brings each type to oracle-matching surface forms by implementing
the rules that govern compound formation.

Each type has its own derivational rules:

| Type | Sūtra | Description | Example |
|---|---|---|---|
| Avyayībhāva | 2.1.5 | Indeclinable + word, neuter sg | `upakrṣṇam` |
| Tatpuruṣa | 2.1.24 | Determinative; second word governs | `rājapuruṣaḥ` |
| Karmadhāraya | 1.2.42 | Apposition; same-case | `nīlotpalam` |
| Dvandva | 2.2.29 | Coordinative; conjoined nouns | `rāmalakṣmaṇau` |
| Bahuvrīhi | 2.2.23 | Possessive; refers to external | `bahuvrīhi-` |
| Dvigu | 2.1.51 | Numeral + noun, neuter sg | `pañcaratnam` |

Each compound's final inflection follows specific rules:
- Avyayībhāva → always nominative neuter singular
- Dvigu → similar
- Dvandva → dual or plural (depending on sense)
- Tatpuruṣa/Karmadhāraya/Bahuvrīhi → inflect based on the second
  member's stem class and the discourse role

---

## Files

### `samasa/samasa.c` (rewrite, ~500 lines split across files)

```c
ASH_Form samasa_derive(const SubantaDB *db,
                       const char *first_stem,
                       const char *second_stem,
                       ASH_SamasaType type,
                       ASH_Linga external_linga,
                       const ASH_Form *external_subanta);
```

Per-type derivation:
1. Strip `sup` markers from each member (3.4.78 / 1.2.46 supo
   dhātuprātipadikayoḥ).
2. Apply samāsa-specific sandhi at the junction (vowel sandhi, visarga
   transformations).
3. Determine the resulting stem and apply subanta inflection on it.

### `tests/data/samasa_oracle.tsv` (new)
A small TSV with 100 expected compound forms across all 6 types.

---

## Acceptance Criteria

- [ ] avyayībhāva: `upa + krṣṇam` → `upakrṣṇam`
- [ ] tatpuruṣa: `rāja + puruṣaḥ` → `rājapuruṣaḥ` (with appropriate
      inflection based on the case the compound is being used in)
- [ ] karmadhāraya: `nīla + utpalam` → `nīlotpalam`
- [ ] dvandva: `rāma + lakṣmaṇaḥ` → `rāmalakṣmaṇau` (dual)
- [ ] bahuvrīhi: `bahu + vrīhi + suffix` produces possessive forms
- [ ] dvigu: `pañcan + ratnam` → `pañcaratnam`
- [ ] All 6 types have unit tests covering at least 5 examples each
- [ ] `make validate-phase5` still green
- [ ] New samāsa oracle reaches ≥ 70%
