# BUG-009 — `prakriya_log` records identical strings in `form_before` and `form_after`

**Severity:** HIGH — every step trace is a no-op; defeats prakriyā oracle  
**Effort:** 2 hours  
**Files affected:** `prakriya/context.c`, `prakriya/tinanta/lat_bhvadi.c`  
**Depends on:** —

---

## Symptom

For any tiṅanta derivation, the trace produced by `--trace` mode shows the
same string in both before and after columns of every step:

```
$ ash_demo trace-tinanta BU 1 LAT PRATHAMA EKAVACANA PARASMAI
FORM    Bavati  भवति
STEP    301068  Bava    Bava    vikaraRa assignment       <-- before == after
STEP    703084  Bava    Bava    sArvadhAtukArdhadhAtukayoH <-- before == after
STEP    601078  Bava    Bava    eco'yavAyAvaH              <-- before == after
STEP    304078  Bavati  Bavati  tiN assignment             <-- before == after
```

Real derivation should look like:
```
STEP  Bū    + a   = Bava   (vikaraṇa: B+vikaraṇa-stem)
STEP  Bū    → Bo  = Bo     (7.3.84 guṇa)
STEP  Bo    → Bav = Bav    (6.1.78 ec→ay)
STEP  Bav   + a   = Bava   (Sap concat)
STEP  Bava  + ti  = Bavati (3.4.78 tiṅ assignment)
```

## Root cause — two layers

### 1. `prakriya/context.c::prakriya_log()` always uses current form for both fields

```c
void prakriya_log(PrakriyaCtx *ctx, uint32_t sutra_id, const char *desc) {
  char current[128] = {0};
  if (!ctx) return;
  prakriya_current_form(ctx, current, sizeof(current));
  prakriya_log_transition(ctx, sutra_id, current, current, desc);
  /*                                       ^^^^^^^  ^^^^^^^
   *                              before == after — every step is no-op  */
}
```

### 2. `prakriya/tinanta/lat_bhvadi.c::log_single_term_change()` works around it incorrectly

```c
static void log_single_term_change(PrakriyaCtx *ctx, uint32_t sutra_id,
                                   const char *before, const char *after,
                                   const char *desc) {
  char old_value[TERM_VALUE_LEN] = {0};
  ...
  set_single_term(ctx, before);
  strncpy(ctx->terms[0].value, after, TERM_VALUE_LEN - 1);  // overwritten with `after`
  prakriya_log(ctx, sutra_id, desc);  // <-- reads `after` for both fields
  ...
}
```

The function takes `before` and `after` parameters but ignores them, because
`prakriya_log` (called inside) always reads the current term and writes it twice.

## Required fix

### Step 1: Make `prakriya_log` and `prakriya_log_transition` distinguish their roles

**`prakriya_log`** is fine for "I just made a change to ctx; record it" use cases —
the change is already in ctx, so before==after for the *snapshot* style of logging
is acceptable IF the snapshots are taken at different times. But for proper
transition tracking, use the explicit transition function.

**`prakriya_log_transition`** is already correct — it stores the explicit
before and after strings. The bug is that `lat_bhvadi.c` doesn't use it directly.

### Step 2: Refactor `lat_bhvadi.c::log_single_term_change` to use `prakriya_log_transition` directly

```c
static void log_single_term_change(PrakriyaCtx *ctx, uint32_t sutra_id,
                                   const char *before, const char *after,
                                   const char *desc) {
  if (!ctx || !before || !after) return;
  prakriya_log_transition(ctx, sutra_id, before, after, desc);
  /* Update ctx->terms[0].value to `after` AFTER logging, so subsequent
     prakriya_current_form() reflects the new state */
  set_single_term(ctx, after);
}
```

### Step 3: Verify in `prakriya/context.h` that `prakriya_log_transition` is exported

If not already declared, add it to the public header so call sites can use it.

### Step 4: Update lat_bhvadi to log REAL intermediate strings

```c
bool lat_bhvadi_derive_ctx(...) {
  ...
  copy_clean_root(dhatu_slp1, clean_root, sizeof(clean_root));   /* e.g. "BU" */

  // Step 1: vikaraṇa assignment
  char after_vikarana[64];
  apply_vikarana_concat(clean_root, gana, after_vikarana, ...); /* "BUa" */
  prakriya_log_transition(ctx, vik_sutra, clean_root, after_vikarana, "vikaraNa");
  set_single_term(ctx, after_vikarana);

  // Step 2: guṇa
  char after_guna[64];
  apply_guna(after_vikarana, gana, after_guna, ...);            /* "BOa" */
  if (strcmp(after_vikarana, after_guna) != 0) {
    prakriya_log_transition(ctx, 703084, after_vikarana, after_guna,
                            "sArvadhAtukArdhadhAtukayoH");
    set_single_term(ctx, after_guna);
  }

  // Step 3: ec → ay
  char after_ec[64];
  apply_ec_to_ay(after_guna, after_ec, ...);                    /* "Bava" */
  if (strcmp(after_guna, after_ec) != 0) {
    prakriya_log_transition(ctx, 601078, after_guna, after_ec, "eco'yavAyAvaH");
    set_single_term(ctx, after_ec);
  }

  // Step 4: tiṅ
  char final_form[64];
  snprintf(final_form, sizeof(final_form), "%s%s", after_ec, t->clean);
  prakriya_log_transition(ctx, 304078, after_ec, final_form, "tiN assignment");
  set_single_term(ctx, final_form);
}
```

## Acceptance Criteria

- [ ] `prakriya_log_transition` declared in `prakriya/context.h` (exported)
- [ ] `lat_bhvadi.c::log_single_term_change` rewritten to call `prakriya_log_transition` directly with non-equal `before`/`after`
- [ ] `lat_bhvadi.c::lat_bhvadi_derive_ctx` passes the actual intermediate strings, not `clean_root` for every step
- [ ] `ash_demo trace-tinanta BU 1 LAT PRATHAMA EKAVACANA PARASMAI` produces a trace where `before != after` for at least 2 of the 4 steps
- [ ] Same applies to `a_stem.c` (which already partially uses `prakriya_log_transition` — verify the before/after strings are real)
- [ ] Run `python3 tools/run_prakriya_oracle.py --validate` — any-match rate should be > 0% (it is currently 0% because every transition is a no-op)
- [ ] All 27 existing test suites still pass
