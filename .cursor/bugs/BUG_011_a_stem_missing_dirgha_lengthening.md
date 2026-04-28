# BUG-011 — a-stem subanta missing 7.3.101 (`ato dīrgho yaṅi`)

**Severity:** MEDIUM — incorrect for 1st-person endings  
**Effort:** 2 hours  
**Files affected:** `prakriya/subanta/a_stem.c`, `prakriya/tinanta/lat_bhvadi.c`  
**Depends on:** BUG-009 (real trace)

---

## Symptom

For tiṅanta endings starting with `m` or `v` (the 1st-person endings):

| Form | Engine output | Oracle | Difference |
|------|--------------|--------|------------|
| bhū LAT P-uttama-eka | `Bavami` | `BavAmi` | Missing ā lengthening |
| bhū LAT P-uttama-dvi | `Bavavas` | `BavAvaH` | Missing ā lengthening; missing visarga |
| bhū LAT P-uttama-bahu | `Bavamas` | `BavAmaH` | Missing ā lengthening; missing visarga |

The visarga issue is BUG-012; this bug is specifically the missing
ā-lengthening of stem-final `a` before `m`/`v` initial endings.

## Rule reference

Pāṇini 7.3.101 `ato dīrgho yaṅi` —
> The `a` of the aṅga's final is lengthened to `ā` when followed by `mi`, `vas`, or `mas`
> (the so-called "yaṅi" ending — actually the dental-nasal/voiced family).

Specifically, when the tiṅ ending begins with `m` or `v` (per the traditional
formulation: `yaṅ` covers `y, v, r, l` + extends to certain consonants in this
context), the stem-final `a` lengthens.

The rule is sārvadhātuka-context only (it doesn't fire for vibhakti endings
in subanta).

## Required fix

Add the rule application in `lat_bhvadi.c` after the tiṅ ending is concatenated
but before the form is finalized:

```c
/* 7.3.101 ato dīrgho yaṅi:
   In sārvadhātuka context, stem-final 'a' becomes 'A' before 'mi', 'vas',
   'mas' (and 'vahe', 'mahe' for ātmane). */
static void apply_730101(char *form, ASH_Pada pd) {
  size_t n = strlen(form);
  if (n < 4) return;
  /* Look at last 2-3 chars to see if it's a yaṅ-initial ending */
  bool is_yan_ending = false;
  if (pd == ASH_PARASMAI) {
    if (strcmp(form + n - 2, "mi") == 0) is_yan_ending = true;
    else if (strcmp(form + n - 3, "vas") == 0) is_yan_ending = true;
    else if (strcmp(form + n - 3, "mas") == 0) is_yan_ending = true;
  } else { /* atmane */
    if (strcmp(form + n - 4, "vahe") == 0) is_yan_ending = true;
    else if (strcmp(form + n - 4, "mahe") == 0) is_yan_ending = true;
  }
  if (!is_yan_ending) return;
  /* Find the position of the stem-final 'a' before the ending */
  size_t ending_start = ?  /* compute from above match */;
  if (ending_start > 0 && form[ending_start - 1] == 'a') {
    form[ending_start - 1] = 'A';
  }
}
```

Better: since the engine builds `stem + ending` step-by-step, apply the rule at
the boundary BEFORE concatenation:

```c
const char *ending = t->clean;  /* e.g. "mi", "vas", "mas" */
bool yan_initial = (ending[0] == 'm' || ending[0] == 'v') && pd == ASH_PARASMAI;
char modified_stem[64];
strcpy(modified_stem, stem);
if (yan_initial) {
  size_t sn = strlen(modified_stem);
  if (sn > 0 && modified_stem[sn - 1] == 'a') {
    modified_stem[sn - 1] = 'A';
    prakriya_log_transition(ctx, 703101, stem, modified_stem,
                            "ato dIrgho yaNi");
  }
}
strcat(form, modified_stem);
strcat(form, ending);
```

## Acceptance Criteria

- [ ] `lat_bhvadi.c` applies 7.3.101 before the tiṅ ending is appended for
      uttama-puruṣa endings (mi, vas, mas, vahe, mahe)
- [ ] Trace shows the lengthening step explicitly: `Bava → BavA` with sūtra ID 703101
- [ ] These derivations now produce correct forms:

| Form | Before fix | After fix |
|------|-----------|-----------|
| bhū LAT P-uttama-eka | `Bavami` | `BavAmi` |
| bhū LAT P-uttama-dvi | `Bavavas` | `BavAvas` (still missing visarga; BUG-012) |
| bhū LAT P-uttama-bahu | `Bavamas` | `BavAmas` (still missing visarga; BUG-012) |
| jñā LAT P-uttama-eka | `jAnAmi` | `jAnAmi` (already correct, no regression) |

- [ ] Tinanta oracle rate climbs proportional to the share of uttama-puruṣa rows
- [ ] All 27 existing test suites still pass
