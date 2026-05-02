# Story 4.6 — Full i-stem and u-stem subanta paradigms

**Phase:** 4 — Subanta Declension
**Difficulty:** High
**Estimated time:** 8 hours
**Depends on:** Story 4.1–4.5 (existing partial subanta scaffolding), BUG-009

---

## Objective

The current `i_stem_derive` and `u_stem_derive` functions cover only 2-3
of the 24 vibhakti × vacana combinations and only one liṅga each. This
story expands them to **complete 24-form paradigms** for both masculine
and neuter i-stems and u-stems, plus feminine ī/ū-stems.

After this story, the subanta oracle rate for i/u-stems should match the
oracle's Devanāgarī forms with high accuracy.

---

## Background

The Aṣṭādhyāyī's case-and-number table for i-stems (e.g. `agni` "fire",
masculine):

| Vib | Eka | Dvi | Bahu |
|-----|-----|-----|------|
| 1 prathama | agniḥ | agnī | agnayaḥ |
| 2 dvitīyā | agnim | agnī | agnīn |
| 3 tṛtīyā | agninā | agnibhyām | agnibhiḥ |
| 4 caturthī | agnaye | agnibhyām | agnibhyaḥ |
| 5 pañcamī | agneḥ | agnibhyām | agnibhyaḥ |
| 6 ṣaṣṭhī | agneḥ | agnyoḥ | agnīnām |
| 7 saptamī | agnau | agnyoḥ | agniṣu |
| 8 sambodhana | agne | agnī | agnayaḥ |

For neuter i-stems (e.g. `vāri` "water"):
- prathama-eka, dvitīyā-eka: `vāri` (no suffix)
- prathama-bahu, dvitīyā-bahu: `vārīṇi`
- Other forms parallel masculine with neuter-specific changes.

For u-stems, the same skeleton with u→o (guṇa) where i→e applies.

For feminine ī/ū-stems (e.g. `nadī`, `vadhū`), the suffixes shift again
per 7.3.111–112 etc.

---

## Files

### `prakriya/subanta/aaiu_stems.h`

```c
/* Existing prototypes plus full-paradigm helpers. */

bool i_stem_masc_derive(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);

bool i_stem_neut_derive(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);

bool u_stem_masc_derive(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);

bool u_stem_neut_derive(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);

bool ii_stem_fem_derive(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);

bool uu_stem_fem_derive(const char *stem_slp1, ASH_Vibhakti vib,
                        ASH_Vacana vac, PrakriyaCtx *ctx_out);
```

### `prakriya/subanta/aaiu_stems.c`

Replace the stub with table-driven derivation. For each (linga, vibhakti,
vacana) tuple, look up the suffix and apply the standard rule chain:

```c
typedef struct {
  ASH_Vibhakti vib;
  ASH_Vacana   vac;
  const char  *suffix;     /* SLP1 with markers stripped */
  uint32_t     sutra_id;   /* primary sūtra responsible */
} SubantaSlot;

static const SubantaSlot I_STEM_MASC_SLOTS[24] = {
  {ASH_PRATHAMA_VIB, ASH_EKAVACANA,  "H",   401002}, /* agniH */
  {ASH_PRATHAMA_VIB, ASH_DVIVACANA,  "I",   702102}, /* agnI (vowel-fusion */
  {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "ayaH",703102}, /* agnayaH */
  /* ... 21 more slots ... */
};
```

The derivation kernel:
1. Look up the slot for (vibhakti, vacana, linga)
2. Compute the stem-with-junction-sandhi (e.g. for caturthi-eka i-stem
   masculine: `agni` + `e` → guṇa of i → `agne` then suffix `e` is
   absorbed → `agnaye` via ec→ay)
3. Append suffix (some suffixes start with vowels; apply 6.1.78 ec→ay
   or 6.1.101 savarṇa-dīrgha as needed)
4. Apply 8.2.66 + 8.3.15 final-s → visarga at form boundary (BUG-012)

Make the helpers split per liṅga so the table sizes stay tractable.

---

## Test Cases (`tests/unit/test_aaiu.c`)

```c
void test_i_stem_masc_full_paradigm(void) {
  /* agni — masculine i-stem, all 24 forms */
  static const struct { ASH_Vibhakti v; ASH_Vacana n; const char *expected; } cases[] = {
    {ASH_PRATHAMA_VIB, ASH_EKAVACANA,  "agniH"},
    {ASH_PRATHAMA_VIB, ASH_DVIVACANA,  "agnI"},
    {ASH_PRATHAMA_VIB, ASH_BAHUVACANA, "agnayaH"},
    {ASH_DVITIYA_VIB,  ASH_EKAVACANA,  "agnim"},
    {ASH_DVITIYA_VIB,  ASH_DVIVACANA,  "agnI"},
    {ASH_DVITIYA_VIB,  ASH_BAHUVACANA, "agnIn"},
    {ASH_TRITIYA_VIB,  ASH_EKAVACANA,  "agninA"},
    {ASH_TRITIYA_VIB,  ASH_DVIVACANA,  "agniByAm"},
    {ASH_TRITIYA_VIB,  ASH_BAHUVACANA, "agniBiH"},
    {ASH_CATURTHI_VIB, ASH_EKAVACANA,  "agnaye"},
    {ASH_PANCAMI_VIB,  ASH_EKAVACANA,  "agneH"},
    {ASH_SHASTHI_VIB,  ASH_EKAVACANA,  "agneH"},
    {ASH_SAPTAMI_VIB,  ASH_EKAVACANA,  "agnO"},
    {ASH_SAMBODHANA_VIB, ASH_EKAVACANA, "agne"},
  };
  for (size_t i = 0; i < sizeof(cases)/sizeof(cases[0]); i++) {
    PrakriyaCtx ctx = {0};
    bool ok = i_stem_masc_derive("agni", cases[i].v, cases[i].n, &ctx);
    TEST_ASSERT_TRUE(ok);
    char form[64] = {0};
    prakriya_current_form(&ctx, form, sizeof(form));
    TEST_ASSERT_EQUAL_STRING(cases[i].expected, form);
  }
}

void test_u_stem_masc_full_paradigm(void) {
  /* guru — masculine u-stem */
  /* prathama-eka: guruH; tritiya-eka: guruRA; saptami-eka: gurO */
  ...
}
```

---

## Acceptance Criteria

- [ ] `i_stem_masc_derive("agni", ...)` produces all 14 listed forms exactly
- [ ] `i_stem_neut_derive("vAri", PRATHAMA, EKA, ...)` → `vAri`
- [ ] `i_stem_neut_derive("vAri", PRATHAMA, BAHU, ...)` → `vArIRi`
- [ ] `u_stem_masc_derive("guru", ...)` paradigm matches oracle
- [ ] `u_stem_neut_derive("maDu", ...)` paradigm matches oracle
- [ ] `ii_stem_fem_derive("nadI", ...)` paradigm matches oracle
- [ ] `uu_stem_fem_derive("vaDU", ...)` paradigm matches oracle
- [ ] All 27 existing unit tests still pass
- [ ] Subanta oracle rate climbs ≥ 25% on the 1200-row sample

---

## Notes

- Keep tables small per file (AGENTS.md mandates < 500 lines per .c).
  Split into `i_stem.c`, `u_stem.c`, `feminine.c` if needed.
- Each table entry must cite a real sūtra global_id; avoid 0 as
  a placeholder. The sūtra IDs are findable in `data/sutras.tsv`.
