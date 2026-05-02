# Story 3.7 — Robust gaṇa 4 / 6 / 10 vikaraṇa coverage

**Phase:** 3 — Pratyaya System & laṭ Tinanta
**Difficulty:** High
**Estimated time:** 6 hours
**Depends on:** BUG-008, BUG-009, Story 3.5 (lat_bhvadi)

---

## Objective

After Round-2 BUG-008 replaced the over-applied suffix-patterns, the gaṇa
1/4/6/10 paths produce correct output for the *targeted* test roots
(BU, gam, nam, kram, div, tud, cur). This story extends the engine to
produce oracle-matching SLP1 output for **a representative set of 30
non-bhvādi roots** drawn from the dhātupāṭha — covering the systematic
sub-patterns that trip the current minimal pipeline.

Goal: tinanta oracle rate climbs from the current 40.6% to **≥ 65%** on
the 450-row sample, with most remaining mismatches isolated to gaṇa-2/3/5/7/8/9
(out of scope for this story) and a small residual of irregular gaṇa-1
roots.

---

## Scope (in)

- **gaṇa 4 (divādi)** — śyan vikaraṇa, ṅit so 1.1.5 blocks guṇa
  - Roots whose internal `i` lengthens to `I` before śyan: `div`, `siv`, `sriv`, `zWiv`
  - Roots whose final `a~` strips cleanly: `na~S` → `naSyati` (root 4.84)
  - Roots that take guṇa anyway despite ṅit (specific-root exceptions)
- **gaṇa 6 (tudādi)** — śa vikaraṇa, ṅit so 1.1.5 blocks guṇa
  - `tud → tudati`, `liS → liSati`, `nud → nudati`, `kfz → kfzati`
  - The ṛ-ḷ vowel handling: roots with `f` (ṛ) keep their `f` (no guṇa)
- **gaṇa 10 (curādi)** — ṇic vikaraṇa "aya", with guṇa of root vowel
  - `cur → corayati`, `taq → tAqayati` (guṇa + a→A?)
  - The `ay`/`Aya` lengthening for certain roots
- **gaṇa-1 special cases** still unhandled by Round-2:
  - Roots in 7.3.78 list (pā, ghrā, dhmā, sthā, mnā, dāṇ, dṛś, ṛc, sad)
    that get root-substitution (pā → pibati, sthā → tiṣṭhati, dṛś → paśyati)
  - Roots requiring ec → ay (Be → Bavati already covered by BU; check Smi → smayati)

## Scope (out)

- Other lakāras (LIT, LUT, LfT, LaN, LoT, etc.) — Phase-3.x extension
- Ātmanepada-only and ubhayapada quirks beyond LAT-parasmai sample
- gaṇas 2/3/5/7/8/9 (separate stories)

---

## Files

### `prakriya/tinanta/lat_bhvadi.c` (extension)

Add three explicit rule-list helpers and corresponding case branches:

```c
/* 7.3.78 pā-ghrā-dhmā-sthā-mnā-... — root substitution for these dhātus */
typedef struct { const char *root; const char *substitute; uint32_t sutra; } SubRule;
static const SubRule ROOT_SUBSTITUTIONS[] = {
  {"pA",   "piba",   703078},  /* pā → piba */
  {"GrA",  "jiGra",  703078},  /* ghrā → jighra */
  {"DmA",  "Dama",   703078},  /* dhmā → dhama */
  {"sTA",  "tizWa",  703078},  /* sthā → tiṣṭha */
  {"mnA",  "mana",   703078},  /* mnā → mana */
  {"dfS",  "paS",    703078},  /* dṛś → paś */
  {"sad",  "sId",    703078},  /* sad → sīd */
  {NULL, NULL, 0}
};

static const SubRule *root_substitute_lookup(const char *clean_root);
```

Then in `apply_class_transform` add a high-priority branch *before* the
existing 7.3.77 list, so substitution happens first:

```c
if (gana == 1) {
  const SubRule *sub = root_substitute_lookup(stem);
  if (sub) {
    strncpy(stem, sub->substitute, stem_len - 1);
    stem[stem_len - 1] = '\0';
    *class_sutra = sub->sutra;
    /* skip default guṇa for substituted roots */
    goto vikarana_append;
  }
  /* ... existing 7.3.77 / 7.3.76 / default guṇa branches ... */
}
```

For gaṇa 6, force `*used_guna = false` and skip the default-vowel-replacement
path (gaṇa 6 with śa vikaraṇa never applies guṇa to the root vowel).

For gaṇa 10, the existing guṇa-of-first-vowel path stays, but add
support for roots whose root vowel is `a` (e.g. `taq` — handled by 7.2.114
"mṛjer vṛddhiḥ" or 7.3.86 pugantalaghūpadhasya). Concrete: for `taq` (=
to strike) gaṇa 10, expected form is `tAqayati` — the `a` of root
lengthens. Add `taq` (and `kaT`) to a small `GANA10_ALENGTHEN` list.

### `prakriya/tinanta/vikaranas.c` (extension)

Fix `gana_uses_vrddhi(10)` returning `true` (it should be `false`; gaṇa 10
takes guṇa, not vṛddhi). Update the corresponding test in
`test_coverage_branches.c` to assert the corrected behaviour.

---

## Acceptance Criteria

- [ ] `lat_bhvadi_derive("pA", 1, ...)` → `pibati`
- [ ] `lat_bhvadi_derive("sTA", 1, ...)` → `tizWati`
- [ ] `lat_bhvadi_derive("dfS", 1, ...)` → `paSyati`
- [ ] `lat_bhvadi_derive("sad", 1, ...)` → `sIdati`
- [ ] `lat_bhvadi_derive("naS", 4, ...)` → `naSyati` (gaṇa-4 default)
- [ ] `lat_bhvadi_derive("kfz", 6, ...)` → `kfzati` (gaṇa-6 keeps ṛ)
- [ ] `lat_bhvadi_derive("nud", 6, ...)` → `nudati` (gaṇa-6 no guṇa of u)
- [ ] `lat_bhvadi_derive("taq", 10, ...)` → `tAqayati` (gaṇa-10 ālengthening)
- [ ] All BUG-008 / BUG-010 tests still pass
- [ ] Tinanta oracle rate ≥ 65% on the 450-row sample

---

## Notes

- Keep the substitution table small and explicit. Pāṇinian fidelity is
  better achieved through rule lookups than through phonological pattern
  guesses — this is the architectural lesson from BUG-008.
- The substitution table is a placeholder; the long-term plan is to
  drive substitutions from a TSV that mirrors the ganasūtra structure
  in the dhātupāṭha. That migration is out of scope here.
