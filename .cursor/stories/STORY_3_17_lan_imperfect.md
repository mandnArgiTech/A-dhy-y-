# Story 3.17 — LAN (imperfect, "anadyatana past")

**Phase:** 3 — Pratyaya System & Tiṅanta
**Difficulty:** Medium
**Estimated time:** 5 hours
**Depends on:** Story 3.16 (LAT 100%), all sandhi work

---

## Objective

Implement the laṅ lakāra (imperfect, anadyatana past tense) for all
six gaṇas already at LAT-100% (1, 2, 4, 5, 6, 10). LAN re-uses the
same vikaraṇas as LAT but takes (a) the **a-augment** at the front
of the root (per 6.4.71 luṅ-laṅ-lṛṅ-kṣv aḍudāttaḥ) and (b) the
**secondary tiṅ endings** (3.4.78 substitutes laṅ → 9 secondary
endings).

After this story, the engine can derive `aBavat` for `BU + LAN +
prathama-eka-P`, `agacCat` for `gam`, etc.

---

## Background — paradigm scaffolding for bhū LAN parasmai

| | Eka | Dvi | Bahu |
|-|-|-|-|
| Prathama | aBavat | aBavatAm | aBavan |
| Madhyama | aBavaH | aBavatam | aBavata |
| Uttama | aBavam | aBavAva | aBavAma |

Key derivation steps (3.2.110–125, 3.4.99–112):
1. 3.2.111 anadyatane laṅ — laṅ replaces lṛ in anadyatana past
2. 3.4.99 nityaṃ ṅitaḥ — laṅ endings are ṅit (block guṇa via 1.1.5)
3. 6.4.71 luṅ-laṅ-lṛṅ-kṣv aḍudāttaḥ — `aṭ` augment prepended
4. 3.4.100 itaś ca — replace final `i` of secondary endings (in eka)
5. 7.1.13 ṅer yat — gives `vam`/`māṃ` etc. shapes

---

## Files

### `prakriya/tinanta/lan.h` (new)
```c
bool lan_derive_ctx(const char *dhatu_slp1, int gana, ASH_Purusha p,
                    ASH_Vacana v, ASH_Pada pd, PrakriyaCtx *ctx_out);
bool lan_derive(const char *dhatu_slp1, int gana, ASH_Purusha p,
                ASH_Vacana v, ASH_Pada pd, char *out_slp1, size_t out_len);
```

### `prakriya/tinanta/lan.c` (new, ~200 lines)
- New `LAN_PARASMAI[9]` and `LAN_ATMANE[9]` ting tables (3.4.99–104).
- Reuse existing apply_class_transform but bypass guṇa for ṅit endings
  in athematic gaṇas.
- Prepend `a-` augment at the start of the joined form (with vowel
  sandhi: a + a → A, a + i → e, etc.).
- Wire into `pipeline_tinanta` LAN branch.

### `prakriya/tinanta/lakara.c`
Extend `ting_get` to dispatch LAN endings. Keep LAT path unchanged.

---

## Acceptance Criteria

- [ ] `lan_derive("BU", 1, P, EKA, P)` → `aBavat`
- [ ] `lan_derive("gam", 1, P, BAHU, P)` → `agacCan`
- [ ] `lan_derive("ad", 2, P, EKA, P)` → `Adat` (augment + d → t before t)
- [ ] `lan_derive("cur", 10, P, EKA, P)` → `acorayat`
- [ ] All 9 LAN-P forms of bhū match the dhātuforms.tsv oracle
- [ ] `make validate-phase3` still green
- [ ] `run_vyakarana_oracle.py --lakara LAN` reports ≥ 80% match on a
      450-row sample
