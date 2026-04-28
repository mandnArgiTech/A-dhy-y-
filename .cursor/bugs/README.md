# Bug-Fix Stories — Implementation Order

These bugs are listed in **strict dependency order**. Each lists its own
dependencies in its header. Implement in numerical order.

## Round 1 (closed) — 4 done, 2 partial

| ID | Title | Status |
|----|-------|--------|
| BUG-001 | Devanāgarī → SLP1 converter drops inherent vowel `a` | ✅ FIXED (commit 2a642ca) — 20-word corpus passes 20/20 |
| BUG-002 | Oracle normalize function rigs the match rate | ✅ FIXED (commit 2a642ca) — exact-equality on Devanāgarī |
| BUG-003 | Tiṅanta / subanta paths return hardcoded literals | ⚠️ PARTIAL — strncpy literals removed, but suffix-pattern hardcoding introduced (see BUG-008) |
| BUG-004 | pipeline_tinanta synthesises a fixed trace | ⚠️ PARTIAL — pipeline now uses ctx_to_form, but trace transitions are still no-ops (see BUG-009) |
| BUG-005 | Oracle validation samples are too small | ✅ FIXED (commit 2a642ca) — 450 tiṅanta + 1200 subanta + 100 prakriyā |
| BUG-006 | README overclaims coverage | ✅ FIXED (commit 2a642ca) — honest opening |

After Round 1, honest rates measured (with rigging removed):
- Tinanta:  1.28% match (up from rigged 88.89%)
- Subanta:  7.08% match (up from rigged 100%)
- Prakriyā: 0.00% trace match

## Round 2 (open) — 6 new bugs

| ID | Title | Severity | Effort | Depends on |
|----|-------|----------|--------|-----------|
| BUG-007 | ingest_unadipatha.py uses synthetic rows; real Uṇādi data unused | LOW | 1h | — |
| BUG-008 | lat_bhvadi over-applies 7.3.77 (am→acch) to all gaṇa-1 roots ending `am` | HIGH | 8h | BUG-009 (trace) |
| BUG-009 | prakriya_log writes current form to both before/after, transitions are no-ops | HIGH | 2h | — |
| BUG-010 | lat_bhvadi does not strip anubandhas from input dhātu | HIGH | 1h | BUG-009 |
| BUG-011 | a-stem subanta missing 7.3.101 (ato dīrgho yaṅi): `gAyami` not `gAyAmi` | MEDIUM | 2h | BUG-009 |
| BUG-012 | Tiṅ ending final `s` not visarga-converted (8.2.66 + 8.3.15) | MEDIUM | 2h | BUG-009 |

**Round 2 estimated effort: 16 hours**

## How to work these

1. **BUG-009 first** — without proper trace recording, you cannot tell whether
   subsequent fixes actually do what they claim. Each step in PrakriyaCtx must
   show real before/after strings, not the same string twice.
2. **BUG-007** can be done in parallel with anything else (independent).
3. **BUG-010** before BUG-008: if the engine sees `RIva~` instead of cleaned
   `nI`, no amount of rule logic will produce correct forms.
4. **BUG-008** is the big refactor — replaces all suffix-pattern hardcoding
   with real anubandha-driven, gaṇa-table-driven, sandhi-engine-driven
   derivation.
5. **BUG-011 + BUG-012** are sandhi-glue fixes that should be done together;
   they wire the existing Phase-1 sandhi engine into the form-boundary calls.

After Round 2, expected honest rates:
- Tinanta: ≥ 50% on 450 rows (gaṇa-1 should be majority correct)
- Subanta a-stem: ≥ 90% on 24 forms × 10 stems = 240 rows
- Subanta overall: ~ 25-35% (i/u/ā/consonant stems are still Round 3 work)
- Prakriyā trace: ≥ 30% any-match on a-stem masculine sample

## Round 3 (planned, not yet written)

After Round 2 lands honestly:
- Story 4.3 — i/u-stem subanta (~8h)
- Story 4.4 — consonant-stem subanta (~8h)
- Story 3.6 — Classes 4/6/10 vikaraṇas (~6h)
- Story 5.3 — Kṛt primary suffixes (~8h)

These are **forward stories**, not bug fixes. Add them to `.cursor/stories/`
when Round 2 is closed.
