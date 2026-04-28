# BUG-012 — Final `s` of tiṅ endings not converted to visarga (8.2.66 + 8.3.15)

**Severity:** MEDIUM — affects most non-1st-person plural forms  
**Effort:** 2 hours  
**Files affected:** `prakriya/tinanta/lat_bhvadi.c`, `prakriya/subanta/a_stem.c`  
**Depends on:** BUG-009 (real trace)

---

## Symptom

For tiṅ endings ending in `s` (which is most of them at LAT-prathama-dvi,
LAT-madhyama-dvi, LAT-uttama-dvi/bahu), our engine emits the bare `s`
where it should be a visarga `H`:

| Form | Engine output | Oracle | Diff |
|------|--------------|--------|------|
| bhū LAT P-prathama-dvi | `Bavatas` | `BavataH` | s → H |
| bhū LAT P-madhyama-dvi | `BavaTas` | `BavaTaH` | s → H |
| bhū LAT P-uttama-dvi | `Bavavas` | `BavAvaH` | (Plus BUG-011) |
| bhū LAT P-uttama-bahu | `Bavamas` | `BavAmaH` | (Plus BUG-011) |

This is just the missing visarga sandhi at the end of the form.

## Rule reference

Two sūtras work together:

**8.2.66 sasaJuṣo ruḥ** — final `s` (and `j` in jūṣ) of a pada becomes `ru̇`.
**8.3.15 kharavasānayor visarjanīyaḥ** — `ru̇` becomes visarga `ḥ` before
khar (voiceless consonant) or at end of utterance.

Together: any final `s` of a finite verb form becomes `ḥ` at end of utterance.

The Phase-1 sandhi engine has `sandhi_visarga_apply()` already implemented
and tested. It just isn't being called at the form boundary.

## Required fix

After all rule application in `lat_bhvadi_derive_ctx`, before returning,
apply visarga conversion:

```c
/* 8.2.66 + 8.3.15: final s → H at end of pada */
size_t fl = strlen(form);
if (fl > 0 && form[fl - 1] == 's') {
  /* The character before final 's' is the "a" of the ending (or 'A', 'i', etc.).
     Per 8.2.66 the s is replaced by ru̇, and per 8.3.15 ru̇ → H at utterance end. */
  char before[64];
  strncpy(before, form, sizeof(before) - 1);
  form[fl - 1] = 'H';
  prakriya_log_transition(ctx, 802066, before, form, "sasaJuzo ruH");
  /* Note: in production we would emit two steps for 8.2.66 and 8.3.15,
     but for now collapse them since 'r' is intermediate state we don't render. */
}
```

The same rule fires for subanta — `a_stem_masc_derive` already does this
implicitly for prathama-eka by appending `H` directly. Verify it's logged
correctly with sūtra IDs 802066 and 803015.

## Acceptance Criteria

- [ ] `lat_bhvadi.c::lat_bhvadi_derive_ctx` applies final-s → H before returning
- [ ] Trace shows the visarga step with sūtra ID 802066 (or 803015)
- [ ] These derivations now produce correct forms:

| Form | Before fix | After fix |
|------|-----------|-----------|
| bhū LAT P-prathama-dvi | `Bavatas` | `BavataH` |
| bhū LAT P-madhyama-dvi | `BavaTas` | `BavaTaH` |
| bhū LAT P-uttama-dvi (with BUG-011) | `Bavavas` | `BavAvaH` |
| bhū LAT P-uttama-bahu (with BUG-011) | `Bavamas` | `BavAmaH` |
| bhū LAT A-prathama-eka | `Bavate` | `Bavate` (already correct, no regression) |

- [ ] All 9 LAT P forms of bhū produce oracle-matching output
      (`Bavati, BavataH, Bavanti, Bavasi, BavaTaH, BavaTa, BavAmi, BavAvaH, BavAmaH`)
- [ ] Tinanta oracle rate climbs significantly — most of the 1.28% mismatches
      involve the missing visarga
- [ ] All 27 existing test suites still pass
