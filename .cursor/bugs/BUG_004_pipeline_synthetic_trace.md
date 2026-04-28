# BUG-004 — pipeline_tinanta synthesises a fixed 4-step trace for every derivation

**Severity:** HIGH — invalidates Phase 6.2b (prakriyā trace validation)  
**Phase blocked:** 6.2b (sūtra attribution match against shabdaprakriya.tsv)  
**Files affected:** `prakriya/pipeline.c::pipeline_tinanta()`, `prakriya/pipeline.c::pipeline_subanta()`  
**Depends on:** BUG-003 (the underlying derivation must be real before its trace can be real)

---

## Symptom

`pipeline_tinanta()` ignores whatever steps `lat_bhvadi_derive()` may have logged into the `PrakriyaCtx` and synthesises a fixed list of 4 sūtras:

```c
f.step_count = 4;
f.steps = calloc((size_t)f.step_count, sizeof(ASH_PrakriyaStep));
f.steps[0].sutra_id = 301068;  // 3.1.68 kartari Sap
f.steps[1].sutra_id = 703084;  // 7.3.84 sārvadhātukārdhadhātukayoḥ
f.steps[2].sutra_id = 304078;  // 3.4.78 tiṅstrīṇi…
f.steps[3].sutra_id = 601077;  // 6.1.77 iko yaṇ aci    ← does not apply to bhū+a
```

This trace is identical for every root, every gaṇa, every person/number — and it includes 6.1.77 which does not actually fire in the bhū → bhavati derivation.

## Why it's wrong

1. **Wrong sūtras attributed.** 6.1.77 governs `i+a → ya`, `u+a → va` etc. For `bhū + a`, the rule that fires is 6.1.78 (`ec → ay/av`). Reporting 6.1.77 in every trace is factually false.
2. **Different roots need different sūtras.** `tud → tudati` (gaṇa 6) uses 3.1.77 (tudādibhyaḥ + Sa vikaraṇa), not 3.1.68 (kartari Sap, which is gaṇa 1 only). The current trace ignores this.
3. **shabdaprakriya.tsv oracle has real per-step sūtra lists.** Phase 6.2b is supposed to verify our step traces match — but they can't, because our trace is fake.

## Required fix

After BUG-003 is implemented, `lat_bhvadi_derive()` and `a_stem_masc_derive()` will populate `ctx->steps[]` with real entries as each rule fires. Then `pipeline_tinanta()` and `pipeline_subanta()` should:

```c
ASH_Form pipeline_tinanta(...) {
  PrakriyaCtx ctx = {0};
  prakriya_init_tinanta(&ctx, ...);

  if (!lat_bhvadi_derive_with_trace(&ctx, ...)) {
    return make_error_form(...);
  }

  // Build ASH_Form FROM the populated ctx, not synthesised
  ASH_Form f = ctx_to_form(&ctx);   // existing helper
  return f;
}
```

The `ctx_to_form()` helper already exists and correctly copies steps from the context. The bug is that `lat_bhvadi_derive()` is called for the result string only, with the real-trace path never running.

## Acceptance Criteria

- [ ] `pipeline_tinanta()` no longer contains `f.step_count = 4` or any hardcoded `f.steps[i].sutra_id = ...`
- [ ] All trace entries come from `PrakriyaCtx` populated during derivation
- [ ] Two different roots produce different traces:
  - `bhū + LAT P-prathama-eka` → cites `3.1.68` (kartari Sap, gaṇa 1)
  - `tud + LAT P-prathama-eka` → cites `3.1.77` (tudādibhyaḥ, gaṇa 6)
- [ ] Two different person/number forms of the same root produce traces that differ in the tiṅ-assignment step:
  - `bhū P-prathama-eka` → tip assigned (3.4.78)
  - `bhū P-uttama-bahu` → mas assigned (3.4.78 with different cell)
- [ ] Match against `data/shabdaprakriya.tsv` ≥ 50% sūtra attribution on 20 sample derivations (acceptable initial bar; raise as engine matures)

## Estimated effort

2 hours (mostly removing the synthesis code and verifying the existing ctx_to_form path)

## Note

The same fix applies to `pipeline_subanta()` — verify it also uses `ctx_to_form` and does not have its own synthetic trace.
