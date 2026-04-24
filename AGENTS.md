# AGENTS.md — Cursor Story Execution Protocol

> **Read this before implementing any story.**

## Rules (Non-Negotiable)

1. **One story at a time.** Never start Story N+1 until Story N passes all its tests.
2. **No C source file > 500 lines.** Split into multiple files if needed.
3. **All internal strings in SLP1.** Convert to IAST/Devanāgarī only at output boundaries via `encoding/encoding.h`.
4. **Every function has a unit test** in `tests/unit/test_<module>.c` using the Unity test framework (bundled in `tests/unity/`).
5. **Never hardcode Sanskrit words.** All sūtras, dhātus, gaṇas load from TSV files generated in Phase 0.
6. **Prakriyā must be traceable.** Every derivation step records the sūtra `global_id` that caused it. No black-box operations.
7. **Validate each phase before proceeding.** Run `make validate-phaseN` and confirm it passes before starting Phase N+1.
8. **Source authority:** If `vyakarana` (Python) and `scl` (C/OCaml) disagree, prefer `scl` and add a note to `tests/KNOWN_DIFFERENCES.md` with the sūtra-level justification.
9. **Memory hygiene:** Every `_new()` / `_load()` function has a matching `_free()`. Run valgrind in CI.
10. **All symbols prefixed `ASH_` or `ash_`** in the public header. Internal symbols use lowercase module prefixes (`varna_`, `sutra_`, `prakriya_`, etc.).

## Story File Format

Each story lives in `.cursor/stories/STORY_<phase>_<num>_<slug>.md`.

A story is **done** when:
- [ ] All acceptance criteria pass
- [ ] `make validate-phase<N>` green
- [ ] No compiler warnings (`-Wall -Wextra -Werror`)
- [ ] Valgrind reports zero leaks on unit tests

## Encoding Reference (SLP1)

| Sound | SLP1 | IAST |
|-------|------|------|
| a | a | a |
| ā | A | ā |
| i | i | i |
| ī | I | ī |
| u | u | u |
| ū | U | ū |
| ṛ | f | ṛ |
| ṝ | F | ṝ |
| ḷ | x | ḷ |
| e | e | e |
| ai | E | ai |
| o | o | o |
| au | O | au |
| ṃ (anusvāra) | M | ṃ |
| ḥ (visarga) | H | ḥ |
| k | k | k |
| kh | K | kh |
| g | g | g |
| gh | G | gh |
| ṅ | N | ṅ |
| c | c | c |
| ch | C | ch |
| j | j | j |
| jh | J | jh |
| ñ | Y | ñ |
| ṭ | w | ṭ |
| ṭh | W | ṭh |
| ḍ | q | ḍ |
| ḍh | Q | ḍh |
| ṇ | R | ṇ |
| t | t | t |
| th | T | th |
| d | d | d |
| dh | D | dh |
| n | n | n |
| p | p | p |
| ph | P | ph |
| b | b | b |
| bh | B | bh |
| m | m | m |
| y | y | y |
| r | r | r |
| l | l | l |
| v | v | v |
| ś | S | ś |
| ṣ | z | ṣ |
| s | s | s |
| h | h | h |

## Directory Layout Quick Reference

```
.cursor/stories/     ← Cursor reads stories from here
core/                ← Core engine modules
ancillary/           ← Dhātupāṭha, Gaṇapāṭha, Uṇādi
prakriya/            ← Word derivation engine
sandhi/              ← Sandhi rules
samasa/              ← Compound formation
encoding/            ← Codec (SLP1/IAST/Devanāgarī/HK)
tests/unit/          ← Unity unit tests (one file per module)
tests/regression/    ← Phase-level regression tests vs oracles
tests/data/          ← Test matrices (TSV)
tools/               ← Python helper scripts (ingestion, oracle runners)
vendor/              ← Git submodule: mandnArgiTech/A-dhy-y- source data
data/                ← Generated TSV data files (output of Phase 0 scripts)
include/             ← Public header: ashtadhyayi.h
examples/            ← Demo programs
```
