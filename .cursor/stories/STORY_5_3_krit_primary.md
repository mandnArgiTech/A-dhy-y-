# Story 5.3 — Kṛt Primary Derivatives

**Phase:** 5 — Pipeline, Samāsa, Kṛt, Taddhita  
**Difficulty:** High  
**Estimated time:** 5 hours  
**Depends on:** Story 5.1

---

## Objective

Implement the nine most common primary kṛt (verbal) suffix derivatives — nominal and participial forms derived directly from verb roots.

---

## Background

Kṛt suffixes (3.1.93–3.4.76) attach to a verb root to form primary nominal derivatives. Unlike tiṅanta (finite verbs), kṛt derivatives are treated as prātipadikas and then declined as nouns.

Key sūtra: 3.1.93 `kṛdatiṅ` — "kṛt [suffixes] are not tiṅ." This defines the entire class.

---

## The Nine Kṛt Types to Implement

### 1. kta — Past Passive Participle (PPP)
- **Sūtra:** 3.2.102 `niṣṭhā` (defines kta and ktavat as a class)
- **Suffix:** ta (sometimes na)
- **Example:** bhū + kta → bhūta (been/become), gam + kta → gata (gone)
- **Rules:** 8.2.52 (conjunct simplification), 7.2.10 (iṭ augment for some roots)
- **Produces:** adjective/participial form (e.g., "gata" = "gone")

### 2. ktavat — Past Active Participle
- **Sūtra:** 3.2.102 (same class as kta)
- **Suffix:** tavat (= ta + vat)
- **Example:** gam + ktavat → gatvavat (one who has gone)
- **Note:** ktavat = kta + vat suffix; derives adjective from PPP

### 3. śatṛ — Present Active Participle
- **Sūtra:** 3.2.124 `latḥ śatṛśānacau`
- **Suffix:** at (before consonant-initial endings), ant (stem form)
- **Example:** bhū + śatṛ → bhavat (being, present in all senses)
- **Rules:** Same vikaraṇa as laṭ applies; suffix is SJ_SAT

### 4. śānac — Present Middle Participle
- **Sūtra:** 3.2.124 (same as śatṛ for ātmanepada)
- **Suffix:** āna (stem: āna, in ablative etc.)
- **Example:** lab + śānac → labhāna (obtaining, middle)

### 5. tavya — Gerundive (obligation)
- **Sūtra:** 3.1.96 `tavyattavyānīyaraḥ`
- **Suffix:** tavya
- **Example:** bhū + tavya → bhavatavya (to be/should be)
- **Rules:** iṭ augment may apply (7.2.10)

### 6. anīya — Gerundive (alternative)
- **Sūtra:** 3.1.96 (same)
- **Suffix:** anīya
- **Example:** bhū + anīya → bhavanīya (to be)

### 7. ya — Gerundive (third type)
- **Sūtra:** 3.1.97 `acaḥ`
- **Suffix:** ya (only for vowel-final roots)
- **Example:** (specialized usage)

### 8. ktvā — Absolutive (gerund, no prefix)
- **Sūtra:** 3.4.21 `samānakartṛkayoḥ pūrvakāle`
- **Suffix:** tvā (after root)
- **Example:** gam + ktvā → gatvā (having gone)
- **Rules:** iṭ augment (7.2.10); root may take guṇa before tvā

### 9. lyap — Absolutive (with prefix)
- **Sūtra:** 3.4.22
- **Suffix:** ya (after prefix+root)
- **Example:** āgam + lyap → āgamya (having come, with prefix ā)

---

## Files to Create

### `prakriya/krit/krit_primary.h`

```c
#ifndef KRIT_PRIMARY_H
#define KRIT_PRIMARY_H

#include "ashtadhyayi.h"
#include "context.h"
#include "sutra.h"

/**
 * Derive a primary kṛt form.
 * @param db         Loaded database
 * @param root_slp1  Dhātu in SLP1 (e.g. "gam", "BU")
 * @param gana       Gaṇa (1–10), 0 = auto-detect
 * @param krit       Kṛt suffix type
 * @return ASH_Form — caller must ash_form_free()
 */
ASH_Form krit_derive(const SutraDB *db, const char *root_slp1, int gana,
                      ASH_KritType krit);

/**
 * Derive kṛt form with a verbal prefix (upasarga).
 * @param prefix_slp1  Upasarga (e.g. "A", "pra", "vi")
 */
ASH_Form krit_derive_with_prefix(const SutraDB *db, const char *root_slp1,
                                   int gana, ASH_KritType krit,
                                   const char *prefix_slp1);

/** Is the iṭ augment applicable to this root + suffix combination? (7.2.10) */
bool krit_needs_it_augment(const char *root_slp1, ASH_KritType krit);

#endif /* KRIT_PRIMARY_H */
```

### `prakriya/krit/krit_primary.c`

#### Core derivation algorithm

```c
ASH_Form krit_derive(const SutraDB *db, const char *root_slp1, int gana,
                      ASH_KritType krit) {
  PrakriyaCtx ctx = {0};

  /* Step 1: Initialize dhātu term */
  term_init(&ctx.terms[0], root_slp1, SJ_DHATU);
  ctx.term_count = 1;

  /* Step 2: Select suffix */
  const char *suffix_upadesa = krit_suffix_upadesa(krit); /* e.g. "ktvA", "Satf" */
  term_init(&ctx.terms[1], suffix_upadesa, SJ_KRT | SJ_PRATYAYA);
  ctx.term_count = 2;

  /* Step 3: Apply guṇa/vṛddhi if needed */
  bool kit = anubandha_has_it(suffix_upadesa, 'k', SJ_KRT);
  if (!kit) {
    guna_apply_to_final(&ctx.terms[0], &ctx.terms[1], &rule_id);
    prakriya_log(&ctx, rule_id, "guṇa of root vowel");
  }

  /* Step 4: iṭ augment (7.2.10) if applicable */
  if (krit_needs_it_augment(root_slp1, krit)) {
    /* Insert 'i' before suffix */
    char new_suffix[64] = "i";
    strcat(new_suffix, ctx.terms[1].value);
    term_substitute(&ctx.terms[1], new_suffix, sutra_id_7_2_10);
    prakriya_log(&ctx, sutra_id_7_2_10, "iṭ augment applied");
  }

  /* Step 5: Strip anubandhas from suffix */
  AnubandhaResult ar;
  anubandha_strip(ctx.terms[1].value, SJ_KRT | SJ_PRATYAYA, &ar);
  term_substitute(&ctx.terms[1], ar.clean_slp1, 0);

  /* Step 6: Apply sandhi at root+suffix junction */
  char joined[256];
  sandhi_vowel_join(ctx.terms[0].value, ctx.terms[1].value,
                     joined, sizeof(joined));
  /* Store result */

  return prakriya_build_result(&ctx);
}
```

#### Suffix upadesa table

```c
static const char *KRIT_SUFFIX_UPADESA[] = {
  [ASH_KRIT_KTA]    = "kta",   /* PPP: ta */
  [ASH_KRIT_KTAVAT] = "ktavat", /* Past active: tavat */
  [ASH_KRIT_SHATR]  = "Satf",  /* Pres act: at/ant */
  [ASH_KRIT_SHANAC] = "SAnac", /* Pres mid: āna */
  [ASH_KRIT_TAVYA]  = "tavya", /* Gerundive: tavya */
  [ASH_KRIT_ANIIYA] = "anIya", /* Gerundive: anīya */
  [ASH_KRIT_YA]     = "ya",    /* Gerundive: ya */
  [ASH_KRIT_LYAP]   = "lyap",  /* Absolutive+prefix: ya */
  [ASH_KRIT_KTVA]   = "ktvA",  /* Absolutive: tvā */
};
```

#### iṭ augment rules (7.2.10–7.2.35)

Roots that take iṭ before ktvā/ta:
- All aniṭ roots do NOT take iṭ (identified by `n` as it-letter)
- Set roots DO take iṭ (identified by `s` as it-letter)
- Veṭ roots may or may not (optional)

For Phase 5, implement:
- If root ends in consonant + ktvā: add iṭ if root is seṭ
- If root ends in short vowel + ktvā: no iṭ (root lengthened instead)

---

## Expected Forms Table

| Root | Gaṇa | Kṛt | Expected SLP1 | IAST |
|------|------|-----|--------------|------|
| gam | 1 | KTA | gata | gata |
| gam | 1 | KTVA | gatvA | gatvā |
| BU | 1 | KTA | BUta | bhūta |
| BU | 1 | KTA | Bavan | bhavan (masc nom sg of śatṛ) |
| BU | 1 | SHATR | Bavat | bhavat (stem) |
| BU | 1 | TAVYA | Bavatavya | bhavatavya |
| BU | 1 | ANIIYA | BavanIya | bhavanīya |
| dA | 3 | KTA | datta | datta (given) |
| kf | 8 | KTA | kfta | kṛta (done) |
| kf | 8 | KTVA | kftvA | kṛtvā |

---

## Unit Test: `tests/unit/test_krit.c`

```c
void test_kta_gam(void) {
  ASH_Form f = krit_derive(&db, "gam", 1, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gata", f.slp1);
  ash_form_free(&f);
}

void test_ktva_gam(void) {
  ASH_Form f = krit_derive(&db, "gam", 1, ASH_KRIT_KTVA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("gatvA", f.slp1);
  ash_form_free(&f);
}

void test_shatr_bhu(void) {
  ASH_Form f = krit_derive(&db, "BU", 1, ASH_KRIT_SHATR);
  TEST_ASSERT_TRUE(f.valid);
  /* bhavat stem form */
  TEST_ASSERT_EQUAL_STRING("Bavat", f.slp1);
  ash_form_free(&f);
}

void test_kta_kr(void) {
  ASH_Form f = krit_derive(&db, "kf", 8, ASH_KRIT_KTA);
  TEST_ASSERT_TRUE(f.valid);
  TEST_ASSERT_EQUAL_STRING("kfta", f.slp1);
  ash_form_free(&f);
}
```

---

## Acceptance Criteria

- [ ] All 9 kṛt types implemented (even if some are partial for edge cases)
- [ ] kta for gam, bhū, kṛ, dā correct
- [ ] ktvā absolutive for gam, bhū correct
- [ ] śatṛ (present active participle) stem form correct
- [ ] iṭ augment logic correct for seṭ vs aniṭ roots
- [ ] Each form's prakriyā logged with sūtra IDs
- [ ] Unit test 0 failures
