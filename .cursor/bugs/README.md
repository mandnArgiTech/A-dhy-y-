# Bug-Fix Stories — Status

All Round 1 and Round 2 bugs are closed. Round 3 onward has been
delivered as feature work in the main implementation phases (β/γ/δ/ε/ζ)
rather than as separate bug rounds — see the main README and
`docs/API.md` for current coverage.

## Round 1 (closed) — 4 fixed, 2 partial-then-superseded

| ID | Title | Status |
|----|-------|--------|
| BUG-001 | Devanāgarī → SLP1 converter drops inherent vowel `a` | ✅ FIXED — 20-word corpus passes 20/20 |
| BUG-002 | Oracle normalize function rigs the match rate | ✅ FIXED — exact-equality on Devanāgarī |
| BUG-003 | Tiṅanta / subanta paths return hardcoded literals | ✅ FIXED — all paths use real derivation now |
| BUG-004 | pipeline_tinanta synthesises a fixed trace | ✅ FIXED — real before/after recorded per step |
| BUG-005 | Oracle validation samples are too small | ✅ FIXED — 712-root LIT survey, 80-stem subanta survey |
| BUG-006 | README overclaims coverage | ✅ FIXED — README rewritten with measured rates |

## Round 2 (closed) — 6 fixed

| ID | Title | Status |
|----|-------|--------|
| BUG-007 | ingest_unadipatha.py uses synthetic rows | ✅ FIXED — 748 real entries, schema reworked |
| BUG-008 | lat_bhvadi over-applies 7.3.77 to all gaṇa-1 -am roots | ✅ FIXED — list-based 7.3.77 + 7.3.76 + 7.3.75 |
| BUG-009 | prakriya_log writes current form to both before/after | ✅ FIXED — direct prakriya_log_transition |
| BUG-010 | lat_bhvadi does not strip anubandhas from input dhātu | ✅ FIXED — anubandha_strip + 6.1.65 ṇo naḥ |
| BUG-011 | a-stem subanta missing 7.3.101 (ato dīrgho yaṅi) | ✅ FIXED — stem→ending boundary handler |
| BUG-012 | Tiṅ ending final `s` not visarga-converted | ✅ FIXED — 8.2.66 + 8.3.15 in final-form sandhi |

## Beyond Round 2 — feature work delivered as Phases β/γ/δ/ε/ζ

What used to be tracked as "Round 3 bugs" has been delivered:

| Theme | Status |
|-------|--------|
| Complete tiṅanta — LIT, LUT, LRT, LOT, LAN, VIDHILIM, ASHIRLIM, LRN, LUN | ✅ all 10 lakāras wired |
| Subanta i/u-stem, consonant-stem, ṛ-stem, an/as-stem, vat-stem | ✅ all wired |
| j-final consonant stems (AKuBuj-class) | ✅ added |
| Sarvanāma full paradigm (22 stems + idam/adas/asmad/yuṣmad) | ✅ 100% oracle |
| Numerals (dvi/tri/catur/paJcan/ṣaṣ/saptan-daśan) | ✅ 100% oracle |
| Kṛt suffixes (target was the closed 129-set) | ✅ 59 wired with rule logic; remainder incremental |
| Taddhita expansion | ✅ 35 suffixes (up from 15) |
| Samāsa sub-types | ✅ 15 (up from 6) |
| Paribhāṣā conflict resolver | ✅ apavāda/nitya/antaraṅga/paratva ladder |
| CI pipeline | ✅ GitHub Actions matrix + oracle regression gate |
| Public API documentation | ✅ docs/API.md |

## Known incremental work (not blocking; feature, not bug)

- Remaining kṛt suffixes (~70 of the closed 129-set) — extensible by
  adding entries to `krit_suffix_upadesa()` / `krit_suffix_clean()`.
- Edge-case polish for some LIT roots (e.g., gam-class anudāttopadeśa
  is fixed, but more anudātta dhātus could be added if oracle gaps
  show up).
- Periphrastic LIT alternates (-mAsa / -baBūva) — currently emitting
  the first alternate (-AYcakAra) which matches the oracle's primary.

If a real bug is found, file a new BUG-### in this folder.
