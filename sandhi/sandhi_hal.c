/* sandhi_hal.c — Story 1.5 implementation
 *
 * Consonant sandhi (hal-sandhi) from Aṣṭādhyāyī 8.2–8.4.
 *
 * Main rules:
 *   8.4.53  jhalāṃ jaś jhaśi   — jhal (stops) before jaŚ → jaŚ (voice assim.)
 *   8.4.55  khari ca            — jhal before khar (voiceless) → car (devoicing)
 *   8.3.23  mo'nusvāraḥ          — final m before consonant → anusvāra (M)
 *   8.4.45  yaro 'nunāsike      — final consonant + nasal → nasal of own class
 *   8.4.62  jhayo ho 'nyatarasyām — etc.
 */

#include "sandhi_hal.h"
#include "varna.h"
#include <string.h>

#define SUTRA_8_3_23  830023
#define SUTRA_8_4_45  840045
#define SUTRA_8_4_53  840053
#define SUTRA_8_4_55  840055

/* ── jhal → jaś mapping (voice assimilation before voiced) ───────────── */
static char jhal_to_jash(char c) {
  switch (c) {
    /* voiced → voiced (identity for same class) */
    case 'g': case 'G': return 'g';  /* g, gh → g */
    case 'j': case 'J': return 'j';  /* j, jh → j */
    case 'q': case 'Q': return 'q';  /* ḍ, ḍh → ḍ */
    case 'd': case 'D': return 'd';  /* d, dh → d */
    case 'b': case 'B': return 'b';  /* b, bh → b */
    /* voiceless → voiced */
    case 'k': case 'K': return 'g';  /* k, kh → g */
    case 'c': case 'C': return 'j';  /* c, ch → j */
    case 'w': case 'W': return 'q';  /* ṭ, ṭh → ḍ */
    case 't': case 'T': return 'd';  /* t, th → d */
    case 'p': case 'P': return 'b';  /* p, ph → b */
    default: return c;
  }
}

/* ── jhal → car mapping (devoicing before voiceless) ──────────────────── */
static char jhal_to_car(char c) {
  switch (c) {
    /* voiced → voiceless */
    case 'g': case 'G': return 'k';
    case 'j': case 'J': return 'c';
    case 'q': case 'Q': return 'w';  /* ḍ → ṭ */
    case 'd': case 'D': return 't';
    case 'b': case 'B': return 'p';
    /* voiceless → voiceless (identity for same class unaspirated) */
    case 'k': case 'K': return 'k';
    case 'c': case 'C': return 'c';
    case 'w': case 'W': return 'w';
    case 't': case 'T': return 't';
    case 'p': case 'P': return 'p';
    default: return c;
  }
}

/* Is this phoneme in the jhal class (all stops + sibilants except semivowels/nasals)? */
static bool is_jhal(char c) {
  /* jhal = all stops + sibilants (everything in hal except: y r l v + ñ m ṅ ṇ n) */
  return (varna_is_consonant(c) || c == 'h') &&
         !varna_is_semivowel(c) && !varna_is_nasal(c);
}

/* ── Core: junction consonant sandhi ─────────────────────────────────── */

SandhiResult sandhi_hal_apply(char a_final, char b_initial) {
  SandhiResult r;
  memset(&r, 0, sizeof(r));
  r.changed = false;

  /* 8.3.23: final m + consonant → anusvāra (ṃ) */
  if (a_final == 'm' && varna_is_consonant(b_initial)) {
    r.result_slp1[0] = 'M';
    r.result_slp1[1] = b_initial;
    r.result_slp1[2] = '\0';
    r.changed = true;
    r.rule_applied = SUTRA_8_3_23;
    return r;
  }

  /* 8.4.45: final stop + nasal consonant → nasal of own class (optional) */
  if (is_jhal(a_final) && varna_is_nasal(b_initial)) {
    /* Use the nasal of same place as a_final */
    /* Map: k-row→N, c-row→Y, ṭ-row→R, t-row→n, p-row→m */
    char nasal;
    switch (a_final) {
      case 'k': case 'K': case 'g': case 'G': nasal = 'N'; break;
      case 'c': case 'C': case 'j': case 'J': nasal = 'Y'; break;
      case 'w': case 'W': case 'q': case 'Q': nasal = 'R'; break;
      case 't': case 'T': case 'd': case 'D': nasal = 'n'; break;
      case 'p': case 'P': case 'b': case 'B': nasal = 'm'; break;
      default: nasal = 0;
    }
    if (nasal) {
      r.result_slp1[0] = nasal;
      r.result_slp1[1] = b_initial;
      r.result_slp1[2] = '\0';
      r.changed = true;
      r.rule_applied = SUTRA_8_4_45;
      return r;
    }
  }

  /* 8.4.53: jhal + jaś (voiced stop) → jaś (voice assimilation) */
  if (is_jhal(a_final) && (b_initial == 'j' || b_initial == 'b' ||
                            b_initial == 'g' || b_initial == 'q' ||
                            b_initial == 'd' || varna_is_vowel(b_initial) ||
                            varna_is_semivowel(b_initial) ||
                            varna_is_nasal(b_initial) || b_initial == 'h')) {
    char voiced = jhal_to_jash(a_final);
    if (voiced != a_final) {
      r.result_slp1[0] = voiced;
      r.result_slp1[1] = b_initial;
      r.result_slp1[2] = '\0';
      r.changed = true;
      r.rule_applied = SUTRA_8_4_53;
      return r;
    }
  }

  /* 8.4.55: jhal + khar (voiceless) → car (devoicing) */
  if (is_jhal(a_final)) {
    /* b_initial is voiceless stop or sibilant? */
    if (varna_is_voiceless(b_initial) || b_initial == 'S' ||
        b_initial == 'z' || b_initial == 's') {
      char devoiced = jhal_to_car(a_final);
      if (devoiced != a_final) {
        r.result_slp1[0] = devoiced;
        r.result_slp1[1] = b_initial;
        r.result_slp1[2] = '\0';
        r.changed = true;
        r.rule_applied = SUTRA_8_4_55;
        return r;
      }
    }
  }

  /* No change — concatenate */
  r.result_slp1[0] = a_final;
  r.result_slp1[1] = b_initial;
  r.result_slp1[2] = '\0';
  return r;
}

bool sandhi_hal_join(const char *a, const char *b, char *out, size_t len) {
  if (!a || !b || !out || len < 2) return false;
  size_t la = strlen(a), lb = strlen(b);
  if (la == 0 || lb == 0) {
    size_t n = la + lb;
    if (n + 1 > len) return false;
    memcpy(out, a, la); memcpy(out + la, b, lb); out[n] = '\0';
    return false;
  }
  char af = a[la - 1], bi = b[0];
  SandhiResult sr = sandhi_hal_apply(af, bi);
  size_t prefix = la - 1;
  size_t junction = strlen(sr.result_slp1);
  size_t suffix = lb - 1;
  size_t total = prefix + junction + suffix;
  if (total + 1 > len) return false;
  memcpy(out, a, prefix);
  memcpy(out + prefix, sr.result_slp1, junction);
  memcpy(out + prefix + junction, b + 1, suffix);
  out[total] = '\0';
  return sr.changed;
}
