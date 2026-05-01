# Story 5.6 — Kṛt primary suffixes (kta, ktavatu, tum, tvā, ana)

**Phase:** 5 — Pipeline, Samāsa, Kṛt, Taddhita
**Difficulty:** Very High
**Estimated time:** 8 hours
**Depends on:** Story 3.5, Story 5.3 (existing partial krit_primary)

---

## Objective

The current `krit_primary.c` covers a small hand-curated table of forms
(kṛta, gata, etc.) by hardcoded substitution. This story replaces that
with rule-driven derivation for the five highest-frequency kṛt suffixes:

- **kta** (3.2.102) — past passive participle: `kṛ + kta → kṛta`
- **ktavatu** (3.2.102) — past active participle: `kṛ + ktavat → kṛtavat`
- **tum** (3.4.65) — infinitive: `kṛ + tum → kartum`
- **tvā** (3.4.21) — gerund: `kṛ + tvā → kṛtvā`
- **lyuṭ / ana** (3.3.115) — action noun: `kṛ + ana → karaṇa`

After this story, the kṛt module produces oracle-matching forms for
~50 dhātus across these five suffixes (250 forms).

---

## Background

Each kṛt suffix has its own rule chain. For `kta` on a vowel-final root:

```
kṛ (root)         — gaṇa 8 (= "kf" in SLP1)
+ kta (suffix)    — k is anubandha (kit), strip → "ta"
                  — kit blocks guṇa (1.1.5)
→ kf + ta
6.1.108 ṛvarṇāt … : ṛ + t → r + t  (here keep f as-is, see saṃprasāraṇa)
7.4.40 dyatisyatimāsthām (specific roots)
→ kṛta (in SLP1: kfta)
```

For `kta` on consonant-final roots:
- Special rules apply: 7.2.10 `ekāca upadeśe'nudāttāt` etc.

For `tvA`:
- 1.1.26 `kta-ktavatū niṣṭhā` saṃjñā
- 7.2.50 `niṣṭhāyāṃ seṭ` — set-class roots take iṭ before tvā
- For `kṛ`: no iṭ, so result is `kṛtvā`
- For consonant-final roots in iṭ class: e.g. `pat + iṭ + tvā → patitvā`

For `tum`:
- Same iṭ logic as `tvā` but with vowel sandhi differences
- Plus 7.3.84 guṇa is enabled (tum is not kit/ṅit)

For `ana`/`lyuṭ`:
- 1.1.69 aṇudit savarṇasya — vṛddhi of penultimate when applicable
- For most dhātus: simple guṇa + concatenation
- `kṛ + ana → kar + aṇa → karaṇa` (the n→ṇ via 8.4.1 raṣābhyāṃ no ṇaḥ)

---

## Files

### `prakriya/krit/krit_primary.h`

```c
typedef enum {
  KRT_KTA = 1,        /* 3.2.102 — past passive */
  KRT_KTAVATU,        /* 3.2.102 — past active */
  KRT_TUM,            /* 3.4.65 — infinitive */
  KRT_TVA,            /* 3.4.21 — gerund */
  KRT_LYUT,           /* 3.3.115 — action noun ("ana") */
} KrtSuffix;

ASH_Form krit_derive(const char *dhatu_slp1, int gana, KrtSuffix suffix);

bool krit_derive_ctx(const char *dhatu_slp1, int gana, KrtSuffix suffix,
                     PrakriyaCtx *ctx_out);
```

### `prakriya/krit/krit_primary.c` (rewrite)

```c
typedef struct {
  KrtSuffix    type;
  const char  *upadesa_slp1;
  const char  *clean_slp1;
  Samjna       it_samjna;     /* SJ_KIT for kta/ktavatu/tvā; none for tum/lyuṭ */
  uint32_t     primary_sutra;
} KrtSuffixInfo;

static const KrtSuffixInfo KRT_TABLE[] = {
  {KRT_KTA,     "kta",  "ta",  SJ_KIT, 302102},
  {KRT_KTAVATU, "ktavatu", "tavat", SJ_KIT, 302102},
  {KRT_TUM,     "tumun", "tum", SJ_NONE, 304065},
  {KRT_TVA,     "ktvA", "tvA", SJ_KIT, 304021},
  {KRT_LYUT,    "lyuw", "ana", SJ_NONE, 303115},
};

static const char *select_iT(const char *clean_root, KrtSuffix suffix);

bool krit_derive_ctx(const char *dhatu_slp1, int gana, KrtSuffix suffix,
                     PrakriyaCtx *ctx_out) {
  /* 1. anubandha-strip the dhātu (BUG-010-aware) */
  /* 2. Apply 6.1.65 ṇo naḥ if dhātu starts with R */
  /* 3. Look up the suffix info; record kit/ṅit blocking flags */
  /* 4. If suffix is set-class and root takes iṭ, insert iṭ */
  /* 5. If suffix is not kit/ṅit: apply 7.3.84 guṇa */
  /* 6. Apply junction sandhi (6.1.78 ec→ay, etc.) */
  /* 7. For lyuṭ/ana: apply 8.4.1 ṇatva */
  /* 8. Final-form sandhi (8.2.66 + 8.3.15 if final s) */
}
```

### iṭ-class membership

For now, encode a **closed list of seṭ-class roots** (those that take
iṭ before niṣṭhā/tum/tvā) in `krit_iset.h`:

```c
static const char *const SET_ROOTS[] = {
  "pat", "vad", "Bf", "kuq", "siv", ...
  NULL
};
```

Long-term this should come from the dhātupāṭha's `settva` column
(already in `data/dhatupatha.tsv`), but the closed list keeps Story 5.6
self-contained.

---

## Test Cases

```c
void test_krt_kta_kf(void) {
  ASH_Form f = krit_derive("kf", 8, KRT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("kfta", f.slp1);
  ash_form_free(&f);
}

void test_krt_tva_kf(void) {
  ASH_Form f = krit_derive("kf", 8, KRT_TVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("kftvA", f.slp1);
  ash_form_free(&f);
}

void test_krt_tum_kf(void) {
  /* kṛ + tum: tum is not kit, so guṇa fires: kf → kar; + tum → kartum */
  ASH_Form f = krit_derive("kf", 8, KRT_TUM);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("kartum", f.slp1);
  ash_form_free(&f);
}

void test_krt_lyut_kf(void) {
  /* kṛ + ana: guṇa kf → kar; +aṇa (with ṇatva from 8.4.1) → karaṇa */
  ASH_Form f = krit_derive("kf", 8, KRT_LYUT);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("karaRa", f.slp1);
  ash_form_free(&f);
}

void test_krt_tva_pat_seT(void) {
  /* pat is seṭ → patitvā */
  ASH_Form f = krit_derive("pat", 1, KRT_TVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("patitvA", f.slp1);
  ash_form_free(&f);
}
```

---

## Acceptance Criteria

- [ ] `krit_derive("kf", 8, KRT_KTA)` → `kfta`
- [ ] `krit_derive("kf", 8, KRT_TUM)` → `kartum`
- [ ] `krit_derive("kf", 8, KRT_TVA)` → `kftvA`
- [ ] `krit_derive("kf", 8, KRT_LYUT)` → `karaRa`
- [ ] `krit_derive("pat", 1, KRT_TVA)` → `patitvA`
- [ ] `krit_derive("BU", 1, KRT_KTA)` → `BUta`
- [ ] `krit_derive("BU", 1, KRT_LYUT)` → `Bavana`
- [ ] All five suffixes produce traced output where each step has a real
      sūtra global_id (no 0-id placeholders)
- [ ] All 27 existing unit tests still pass
- [ ] `make validate-phase5` green

---

## Notes

- The `KRT_TABLE` is a closed list of 5 suffixes. Story 5.7+ would
  expand it to the full ~129 kṛt suffixes from `vendor/krut_pratyay_fallback.json`.
- The set-class membership question (which roots take iṭ) is genuinely
  empirical — Pāṇini lists the *aniṭ* (non-iṭ) roots and treats every
  other root as iṭ. The `data/dhatupatha.tsv` `settva` column is the
  single source of truth; using a closed seed list here is a stepping
  stone, not the long-term design.
