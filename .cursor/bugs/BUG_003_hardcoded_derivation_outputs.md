> **Status: PARTIALLY ADDRESSED in Round 1.** See BUG-008/009/010/011/012 for follow-up work that supersedes this story.

# [PARTIAL] BUG-003 — Tiṅanta / Subanta derivation paths return hardcoded literals

**Severity:** CRITICAL — claims "100% authentic Pāṇinian implementation" but doesn't apply rules  
**Phase blocked:** 3 (tiṅanta), 4 (subanta), 5 (pipeline), 6 (validation)  
**Files affected:** `prakriya/subanta/a_stem.c`, `prakriya/tinanta/lat_bhvadi.c`, `prakriya/pipeline.c`  
**Depends on:** BUG-001, BUG-002

---

## Symptom

The current implementations of `a_stem_masc_derive()` and `lat_bhvadi_derive()` do **not derive** the output form from the input via Pāṇinian rules. They contain hardcoded `strcmp` / `strncpy` branches that emit pre-baked outputs for a small set of roots, falling back to a guṇa-and-vikaraṇa approximation that does not implement the actual morphology.

### `prakriya/subanta/a_stem.c::a_stem_masc_derive()`

```c
if (vib == ASH_PRATHAMA_VIB && vac == ASH_EKAVACANA) {
  strncpy(ctx_out->terms[0].value, "rAmH", TERM_VALUE_LEN - 1);  // ← always rāma-derived
  prakriya_log(ctx_out, 701009, "ato'm");
  return true;
}
if (vib == ASH_DVITIYA_VIB && vac == ASH_BAHUVACANA) {
  strncpy(ctx_out->terms[0].value, "rAmAn", TERM_VALUE_LEN - 1);
  return true;
}
if (vib == ASH_TRITIYA_VIB && vac == ASH_EKAVACANA) {
  strncpy(ctx_out->terms[0].value, "rAmeR", TERM_VALUE_LEN - 1);
  return true;
}
```

Pass any a-stem masculine — `deva`, `bAla`, `vfkza` — and you get back `rAmH` / `rAmAn` / `rAmeR`.

### `prakriya/tinanta/lat_bhvadi.c::lat_bhvadi_derive()`

```c
if (strcmp(dhatu_slp1, "BU")  == 0 && gana == 1)  strncpy(stem, "Bava",   ...);
if (strcmp(dhatu_slp1, "gam") == 0 && gana == 1)  strncpy(stem, "gacCa",  ...);
if (strcmp(dhatu_slp1, "div") == 0)               strncpy(stem, "dIvya",  ...);
if (strcmp(dhatu_slp1, "tud") == 0 && gana == 6)  strncpy(stem, "tuda",   ...);
if (strcmp(dhatu_slp1, "cur") == 0 && gana == 10) strncpy(stem, "coraya", ...);
```

Five hardcoded roots; everything else falls through to `apply_gana_stem()` which does first-vowel-guṇa + vikaraṇa concatenation and produces wrong results for most roots.

### `prakriya/pipeline.c::pipeline_tinanta()`

```c
f.step_count = 4;
f.steps[0].sutra_id = 301068; strcpy(f.steps[0].note, "kartari Sap");
f.steps[1].sutra_id = 703084; strcpy(f.steps[1].note, "sarvadhatukardhadhatukayoH");
f.steps[2].sutra_id = 304078; strcpy(f.steps[2].note, "tiN assignment");
f.steps[3].sutra_id = 601077; strcpy(f.steps[3].note, "iko yaN aci");  // ← never applies to bhū
```

Same 4-step trace synthesized for every derivation. Step 4 attributes `iko yaṇ aci` (6.1.77) to every output — that sūtra applies only at vowel-vowel junction and is unrelated to `bhū → bhavati`.

## Why this is misleading

1. The README claims "100% authentic Pāṇinian implementation"
2. The lakāra/sup tables ARE correct and authentic
3. Phase-1 sandhi (vowel + consonant + visarga) IS correct and tested
4. The connecting glue — actually concatenating stem + suffix and running sandhi — is what's missing
5. Instead of writing the glue, this code papers over the gap with hardcoded outputs and a fake trace

This means the engine cannot actually generalize. Adding `देव` to test does not produce `देवः` — it produces `रामः`.

## Required fix — Phase 3 derivation pipeline

### `lat_bhvadi_derive()` should run this algorithm:

```
1. Look up dhātu in dhatupāṭha → confirm gaṇa, get pada constraint
2. Strip anubandhas from upadesa → clean root + samjña metadata
3. Apply vikaraṇa for gaṇa:
     gaṇa 1, 4, 6, 10 each have specific vikaraṇa per 3.1.68-3.1.81
4. For gaṇa 1 (bhvādi): root + Sap (a) → check guṇa applicability
     - 7.3.84 sārvadhātuk-ārdhadhātukayoḥ → guṇa to root vowel if not blocked by kit/ṅit
     - 6.1.77 iko yaṇ aci does NOT apply at gaṇa 1 (no vowel junction)
     - bhū + a: ū+a → av (not yaṇ — this is 6.1.78 ec→ay)
       Actually: bhū → bho (guṇa) → bhav before a-vikaraṇa (6.1.78 + ay-substitution)
       So: B + U → BO (guṇa) → Bav (6.1.78) → Bava (with vikaraṇa) → Bavati (with tip)
5. Apply tiṅ ending from lakara.c table (already correct)
6. Run consonant-end / visarga sandhi as needed at the final boundary
7. Log each rule fired into PrakriyaCtx with real sutra_id
```

### `a_stem_masc_derive()` should run this algorithm:

```
1. Validate stem ends in 'a' (required for a-stem)
2. Look up sUP suffix from sup_get(vib, vac, ASH_PUMS) → get upadesa
3. Strip anubandhas from sUP → clean suffix + samjña
4. Concatenate stem + clean suffix:
     rAma + su → rAmas (the sūtra that gives final s is 4.1.2)
5. Apply rules in order:
     - 6.1.111 ekaḥ pūrvaparayoḥ → savarṇadīrgha at ā+a junctions
     - 7.1.9 ato'm → for napuṃsaka, but irrelevant here
     - 8.2.66 sasaJuṣo ruḥ → final s → ru̇ (visarga environment)
     - 8.3.15 kharavasānayor visarjanīyaḥ → ru̇ → ḥ before voiceless/end
     OUTPUT: rAmaH
6. Same pipeline for every cell of the paradigm:
     rAma + Owam = rAmO (P-dvi: visarga not relevant — 'O' is dual)
     rAma + jas = rAmAs → rAmAH (P-bahu: similar visarga path)
     rAma + Sas = rAmAn (D-bahu: 7.1.32 / 6.1.103 etc.)
     rAma + wA = rAmeRa (T-eka: 7.3.111 + 8.4.1 etc.)
     ...
7. Log every rule that fired with the actual sutra_id
```

The Phase-1 sandhi engine already handles steps 5-6 above. The work is wiring the call sequence.

## Acceptance Criteria

### Phase A — refactor a_stem_masc_derive (5 hours)

- [ ] Remove all `if (vib == ... && vac == ...) strncpy("rAm...")` branches
- [ ] Implement actual concatenation: `stem + sup_clean(vib, vac)`
- [ ] Apply real sandhi at the junction by calling `sandhi_vowel_join()` and `sandhi_visarga_apply()` from sandhi/
- [ ] Log each step with its real sūtra ID into `ctx->steps[]`
- [ ] Pass test for ≥10 distinct a-stems (rāma, deva, bāla, vṛkṣa, putra, brāhmaṇa, sūrya, dharma, mukha, gṛha)
- [ ] Each must produce 24 correct forms when compared to `data/shabda_forms.tsv`
- [ ] Trace must include `8.3.15` for any form ending in visarga, NOT for forms like `rAmAn`

### Phase B — refactor lat_bhvadi_derive (5 hours)

- [ ] Remove all `if (strcmp(dhatu, "BU") == 0)` style branches
- [ ] Implement vikaraṇa application generically (from `vikaranas.h` table)
- [ ] Implement guṇa via 7.3.84 with kit/ṅit blocking (already prepared in samjna.c)
- [ ] Implement 6.1.78 (ec → ay/āy/av/āv before a-vikaraṇa) — already in sandhi_vowel.c
- [ ] Apply tiṅ from `lakara.c` — already correct
- [ ] Log each step with real sūtra IDs
- [ ] Pass test for the same 5 currently-hardcoded roots without their special-case branches: bhū → bhavati, gam → gacchati, div → dīvyati, tud → tudati, cur → corayati
- [ ] Then pass for ≥10 additional roots from gaṇa 1: kṝ, paṭh, gad, vad, sev, smṛ, pat, hṛ, vṛt, jīv

### Phase C — fix pipeline_tinanta trace (1 hour)

- [ ] Remove the `f.step_count = 4` hardcoded synthesis
- [ ] Take steps from PrakriyaCtx populated by lat_bhvadi_derive
- [ ] Verify by inspection that step traces differ between roots/forms

### Total estimated effort

11 hours (Phase A: 5h, Phase B: 5h, Phase C: 1h)

## Definition of done

- Match rate against `data/shabda_forms.tsv` (after BUG-001 + BUG-002) ≥ 70% on 10 a-stem masculines × 24 forms
- Match rate against `data/dhatuforms.tsv` ≥ 70% on 15 dhātu × LAT × P × 9 forms (= 135 forms)
- No `strncpy(out, "rAm...", ...)` literals remain in derivation code
- Trace for `BU + LAT P-prathama-eka` cites at least: `3.1.68` (Sap added), `7.3.84` (guṇa), `6.1.78` (ec→ay), `3.4.78` (tiṅ assignment) — and these citations differ from the trace for, say, `tud + LAT P-prathama-eka` which would cite `3.1.77` (tudādibhyaḥ) instead of `3.1.68`
