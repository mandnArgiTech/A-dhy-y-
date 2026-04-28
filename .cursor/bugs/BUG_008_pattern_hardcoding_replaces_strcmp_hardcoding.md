# BUG-008 — Suffix-pattern hardcoding replaces `strcmp` hardcoding (over-applies 7.3.77)

**Severity:** HIGH — produces wrong forms for any new gaṇa-1 root ending in `am`  
**Effort:** 8 hours (the real Pāṇinian refactor)  
**Files affected:** `prakriya/tinanta/lat_bhvadi.c`  
**Depends on:** BUG-009 (real trace), BUG-010 (anubandha stripping)

---

## Symptom

The previous BUG-003 fix removed the explicit `strcmp(dhatu_slp1, "gam")` etc.
chains. They were replaced with a structural-suffix pattern that *over-applies*
sūtra 7.3.77 (`iṣu-gami-yam-āṃ chaḥ`) to every gaṇa-1 root ending in `am`:

```c
if (gana == 1 && n >= 2 && stem[n - 2] == 'a' && stem[n - 1] == 'm') {
  stem[n - 2] = 'a';
  stem[n - 1] = 'c';
  stem[n] = 'C';
  stem[n + 1] = '\0';
  /* Now stem ends in "acC" instead of "am" */
}
```

Test results:

| Root | Engine output | Expected (oracle) | Comment |
|------|---------------|-------------------|---------|
| `gam` | `gacCati` | `gacchati` | ✅ Correct (7.3.77 applies — listed in iṣu-gam-yam) |
| `yam` | `yacCati` | `yacchati` | ✅ Correct (listed in iṣu-gam-yam) |
| `nam` | `nacCati` | `namati` | ❌ Wrong — `nam` not in iṣu-gam-yam list |
| `kram` | `kracCati` | `krāmati` | ❌ Wrong — `kram` follows 7.3.78 (vrddhi instead) |
| `bhram` | `BracCati` | `Bramati` | ❌ Wrong — `bhram` follows 7.3.78 |
| `ram` | `racCati` | `ramate` | ❌ Wrong — `ram` is ātmane only |

Same pattern for gaṇa 4 (`stem contains 'i' → I`) and gaṇa 10 (`stem ends 'u' → 'o'`):

| Root | Engine output | Expected | Issue |
|------|---------------|----------|-------|
| `div` (g4) | `dIvyati` | `dīvyati` | ✅ Correct |
| `siv` (g4) | `sIvyati` | `sīvyati` | ✅ Correct (matches by accident) |
| `nrt` (g4) | `nIrtyati` | `nṛtyati` | ❌ Wrong — `nrt` doesn't have `i` |
| `cur` (g10) | `corayati` | `corayati` | ✅ Correct |
| `sev` (g10) | not gaṇa-10 | n/a | The engine doesn't know it's actually gaṇa 1 |

## Root cause

The "fix" is a sequence of structural pattern guards in
`apply_class_transform()` that match the SLP1 surface of specific known
roots and rewrite them. This is fundamentally not Pāṇinian — sūtra 7.3.77
applies to a specific 4-root list (`iṣ`, `gam`, `yam`, `iyaṅ`), not to all
roots ending in `am`.

The Aṣṭādhyāyī's mechanism is:
- **Gaṇasūtras** define the membership of named lists (here `iṣu-gami-yam`)
- The sūtra body (7.3.77) refers to that list, not to a string pattern
- The engine should look up the dhātu's membership in the relevant gaṇa,
  not match its surface form

## Required fix — real Pāṇinian derivation pipeline

### Architecture

```
lat_bhvadi_derive(dhatu_upadesa, gana, p, v, pd):
  1. anubandha_strip(dhatu_upadesa, SJ_DHATU)        [BUG-010]
     → clean_root, it_chars, it_samjna

  2. dhatu_class = dhatupatha_lookup(clean_root, gana)
     → entry has: root, gana, pada, settva, ganasutra_membership

  3. apply_special_rules(clean_root, dhatu_class):
     - 7.3.77 iṣu-gami-yam-āṃ chaḥ  → if root ∈ iṣu-gam-yam-iyaṅ list:
                                       replace final consonant with 'cC'
                                       and add 'C' before vowel suffix
     - 7.3.78 pā-ghrā-...-āṃ pibati-...     → root replacement table
     - 7.3.79 jñā-jano-jor-jām            → root replacement
     - 6.4.16 vā chandasi (other special handling)
     → modified_root

  4. apply_vikarana(modified_root, gana):
     - Sap (a) for gaṇa 1 — 3.1.68
     - Śyan (ya) for gaṇa 4 — 3.1.69
     - Śa (a) for gaṇa 6 — 3.1.77
     - Ṇic (i) for gaṇa 10 — 3.1.25
     → stem_with_vikarana

  5. apply_guna(stem, vikarana):
     - 7.3.84 sārvadhātukārdhadhātukayoḥ → guṇa to stem-final ik vowel
     - 1.1.5 kniti ca → blocks if vikarana is kit/ṅit
     → stem_with_guna

  6. apply_ec_to_ay(stem):
     - 6.1.78 eco'yavāyāvaḥ → e/o/ai/au + vowel → ay/āy/av/āv + vowel
     - sandhi-engine call: sandhi_vowel_apply(stem_final, vikarana_initial)
     → stem_phonologized

  7. apply_tinanta_ending(stem, p, v, pd):
     - lookup ting_get(LAT, p, v, pd) → suffix
     - concatenate stem + suffix
     - run sandhi at junction (mostly no-op for vowel-stem + consonant-suffix)
     → form_with_ending

  8. apply_final_sandhi(form):
     - 8.2.66 sasaJuṣo ruḥ → final 's' → 'r' (ru̇)
     - 8.3.15 kharavasānayor visarjanīyaḥ → final 'r' → visarga
     - 7.3.101 ato dīrgho yaṅi → 'a' before 'mi'/'mas'/'vas' → 'A'   [BUG-011]
     → final_form
```

### Concrete data needed

The dhātupāṭha entries already in `data/dhatupatha.tsv` need a column for
gaṇasūtra membership. Add columns: `iv_gam_yam_member`, `pa_ghra_member`,
`jna_jan_member`, etc.

For now (incremental fix), hardcode the 4-element lists in
`lat_bhvadi.c` itself as `static const char *IV_GAM_YAM[] = {"iz", "gam", "yam", "iyaN", NULL};`
and check membership via `is_in_list(clean_root, IV_GAM_YAM)`.

### Implementation steps

1. **Remove the suffix-pattern guards** in `apply_class_transform`:
   - `gana == 1 && stem ends "am" → "acC"`
   - `gana == 4 && stem contains "i" → "I"`
   - `gana == 10 && stem ends "u" → "o"`

2. **Add a small special-rules layer** that handles the 4 known root lists:
   - 7.3.77 iṣu-gam-yam-iyaṅ list → replace final consonant
   - 7.3.78 pā-ghrā-... → root replacement table

3. **Wire the existing Phase-1 sandhi engine** at the appropriate boundaries.
   `sandhi_vowel_apply()` already handles 6.1.77 (yaṇ), 6.1.78 (ec→ay), 6.1.87
   (guṇa), 6.1.88 (vṛddhi), 6.1.101 (savarṇa). Just call it at:
   - root + vikaraṇa boundary
   - stem + suffix boundary

## Acceptance Criteria

- [ ] All structural-suffix hardcoding in `apply_class_transform` removed
- [ ] Special-rules layer handles 7.3.77 iṣu-gam-yam-iyaṅ list (4 roots only)
- [ ] Sandhi engine called at root+vikaraṇa and stem+suffix boundaries
- [ ] These derivations all produce correct forms:

| Root | Gaṇa | LAT-prathama-eka | Status |
|------|------|------------------|--------|
| BU | 1 | Bavati | ✓ |
| gam | 1 | gacCati | ✓ (7.3.77 by list lookup, not pattern) |
| nam | 1 | namati | ✓ (NEW — was wrongly nacCati) |
| kram | 1 | krAmati | ✓ (NEW — uses 7.3.78 vṛddhi instead) |
| yam | 1 | yacCati | ✓ |
| div | 4 | dIvyati | ✓ |
| nrt | 4 | nftyati | ✓ (NEW — was wrongly nIrtyati) |
| pat | 1 | patati | ✓ |
| cur | 10 | corayati | ✓ |
| sev | 1 | sevati | ✓ (NEW — was treated as gaṇa-10) |

- [ ] Tinanta oracle rate (`run_vyakarana_oracle.py --validate`) climbs to ≥ 50% on the 450-row sample
- [ ] All 27 existing test suites still pass
- [ ] `test_lat_bhvadi.c` keeps its 5 known passing tests AND adds tests for nam, kram, nrt to prevent regression
