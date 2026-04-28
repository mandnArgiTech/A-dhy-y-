# BUG-010 — `lat_bhvadi.c` does not strip anubandhas from input dhātu

**Severity:** HIGH — corrupts every derivation where the upadesa form has marker letters  
**Effort:** 1 hour  
**Files affected:** `prakriya/tinanta/lat_bhvadi.c`  
**Depends on:** BUG-009 (so we can see the strip in the trace)

---

## Symptom

Pass the standard upadesa form `RIva~` (the dhātupāṭha entry for `ṇī`, root 1.1049):

```
$ ash_demo tinanta 'RIva~' 1 LAT PRATHAMA EKAVACANA PARASMAI
RIvaati                # WRONG — should be nayati / nIvati
```

The engine takes `RIva~` literally — including the `~` nasalization marker
which is `it` per 1.3.2 and the leading `R` (cerebral ṇ) which represents
ṇ-it per 1.3.6 — and concatenates as if the whole string were the root.

The oracle entry expects (after stripping `R` and `~`):
- root SLP1: `nI`
- form: `nIvati` (per the dhatuforms.tsv oracle)

## Root cause

`prakriya/tinanta/lat_bhvadi.c::lat_bhvadi_derive_ctx` calls only:

```c
copy_clean_root(dhatu_slp1, clean_root, sizeof(clean_root));
```

`copy_clean_root` only strips `~` and `^`. It does NOT call `anubandha_strip()`
which would handle 1.3.2–1.3.9 properly:

- 1.3.5 ḍuṭu-paired initials (Yi, qu, wu) — strip both chars
- 1.3.6 ṣ initial in pratyaya — strip
- 1.3.7 cuṭū initials in pratyaya — strip
- 1.3.8 laśakvataddhite — l, ś initials in pratyaya — strip
- For dhātu, certain initial consonants are also it (e.g. R for ṇ before vowel
  in dhātupāṭha entries indicates the dhātu is in the "ṇi-class").

## Required fix

Replace the local `copy_clean_root` with the proper `anubandha_strip` call:

```c
#include "anubandha.h"
#include "samjna.h"

bool lat_bhvadi_derive_ctx(...) {
  AnubandhaResult ar;
  anubandha_strip(dhatu_slp1, SJ_DHATU, &ar);
  /* ar.clean_slp1 now has the stripped root, e.g. "nI" from "RIva~" */
  /* ar.it_chars  has the stripped letters, e.g. "R~" */
  /* ar.it_samjna has any derived saṃjñā bits */

  /* Use ar.clean_slp1 as the root for further processing */
  const char *clean_root = ar.clean_slp1;
  ...
}
```

For Phase-1 simplicity, only strip:
- Initial `R` (1.3.5/6 ṇi-it)
- Final `~` (1.3.2 anunāsika it)

Plus the existing `^` accent marker (which is not a strict-it but should not
appear in clean_root).

## Acceptance Criteria

- [ ] `lat_bhvadi.c` includes `anubandha.h` and `samjna.h`
- [ ] `lat_bhvadi_derive_ctx` calls `anubandha_strip(dhatu_slp1, SJ_DHATU, &ar)` first
- [ ] Subsequent processing uses `ar.clean_slp1`, not raw `dhatu_slp1`
- [ ] Trace shows the strip step: `RIva~ → nIv` with sūtra ids 103002 (1.3.2), 103006 (1.3.6)
- [ ] Test added: `RIva~ + LAT-prathama-eka` produces output starting with `n` not `R`
- [ ] Test added: `gamx~ + LAT-prathama-eka` (the `gam` upadesa with ḷ-marker) produces output starting with `g` not `gx` (the `~` ḷ-marker is stripped)
- [ ] Tinanta oracle rate (`run_vyakarana_oracle.py --validate`) increases — many of the current 1.28% mismatches are anubandha-not-stripped failures
- [ ] All 27 existing test suites still pass
