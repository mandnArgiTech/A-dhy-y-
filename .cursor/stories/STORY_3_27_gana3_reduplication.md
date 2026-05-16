# Story 3.27 — Gaṇa-3 reduplication (juhotyādi)

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** High
**Estimated time:** 6 hours
**Depends on:** Story 3.16 (LAT 100%), Story 3.18 (LIT/reduplication infra)

---

## Objective

Implement gaṇa-3 (juhotyādi class) which uses **root reduplication
in the present stem** instead of a vikaraṇa. This is the same
reduplication mechanism that LIT (perfect) uses, so the reduplicate()
helper from Story 3.18 powers both.

Examples:
- `hu` (juhoti, "to sacrifice") gaṇa-3 LAT prathama-eka-P → `juhoti`
- `dhā` (dadhāti, "to put") gaṇa-3 → `dadhāti`
- `dā` (dadāti, "to give") gaṇa-3 → `dadāti`
- `bhī` (bibheti, "to fear") gaṇa-3 → `bibheti`

---

## Background

Gaṇa-3 has roughly 20 roots in the dhātupāṭha. The pattern:
1. 6.1.10 ślau — reduplication (called śliṣṭa) inserted by 3.1.35
2. 7.4.59 hrasvaḥ — abhyāsa vowel shortens
3. 7.4.60 halādiḥ śeṣaḥ — only the first consonant of root retained
4. 7.4.62 kuhoś cuḥ — k/g/gh of abhyāsa become c/j/jh
5. Strong/weak pattern: only ekavacana endings get strong stem (with
   guṇa); dual/plural use weak (no guṇa).

For `hu`:
- abhyāsa = `ju` (h → j by 7.4.62? Actually h → j is special)
- root = `hu`
- strong = `juhau` (vrddhi of u? actually guṇa: u → o)
- prathama-eka strong: ju + ho + ti = juhoti
- prathama-bahu weak: ju + hu + ati = juhvati (wait, atus form?)

Actually 3.4.109 sici saMprasāraṇam: jhi → ati for athematic
classes. So juhu + ati → juhvati (with u → v before ati? or saṃ-
prasāraṇa).

---

## Files

### `prakriya/tinanta/gana3.h` / `gana3.c` (new, ~200 lines)

```c
bool gana3_derive_ctx(const char *dhatu_slp1, ASH_Purusha p,
                      ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
```

Pipeline:
1. Strip anubandhas → clean root.
2. Call reduplicate() to get abhyāsa.
3. Apply 7.4.59-62 rules (abhyāsa cleanup):
   - Shorten abhyāsa vowel
   - Drop second/later consonants in abhyāsa
   - Apply velar→palatal (k/g/gh/h → c/j/jh) shift
4. Concatenate abhyāsa + root.
5. Apply strong/weak: in singular, guṇa root vowel; weak in dual/plural.
6. Apply ending (3.4.109 → ati for jhi).

### `prakriya/tinanta/lakara.c`
Wire gaṇa-3 case in `apply_class_transform` to call
`gana3_derive_ctx` directly (skipping the regular vikaraṇa path).

### `prakriya/tinanta/reduplication.c` (shared with LIT)
Implementing 6.1.10 reduplication + 7.4.59-66 cleanup as standalone
helpers usable by both gaṇa-3 and LIT.

---

## Acceptance Criteria

- [ ] `gana3_derive("hu", P, EKA, P)` → `juhoti`
- [ ] `gana3_derive("hu", P, BAHU, P)` → `juhvati`
- [ ] `gana3_derive("DA", P, EKA, P)` → `daDAti`
- [ ] `gana3_derive("dA", P, EKA, P)` → `dadAti`
- [ ] `gana3_derive("BI", P, EKA, P)` → `biBeti`
- [ ] All 9 LAT-P forms of hu match oracle
- [ ] `make validate-phase3` still green
- [ ] gaṇa-3 oracle reaches ≥ 70%
