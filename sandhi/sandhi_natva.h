/* sandhi_natva.h — unified 8.4.1 ṇatva and 8.3.59 ṣatva helpers.
   Replaces duplicate copies of these post-processes that previously
   lived in iu_stem_full.c, feminine_stems.c, a_stem_full.c, and
   lat_bhvadi.c. */
#ifndef SANDHI_NATVA_H
#define SANDHI_NATVA_H

#include <stddef.h>

/* 8.4.1 raṣābhyāṃ no ṇaḥ samānapade + 8.4.2 atkupvāṅnumvyavāye'pi:
   In the joined SLP1 form, the dental nasal `n` becomes ṇ (R) when
   preceded by a trigger r/f/z/F via only "allowed" intervening
   characters (vowels, k-class, p-class, y/v/h, anusvāra). Dentals,
   palatals, retroflex non-ṇ, ś/s, l, and ṇ itself act as blockers
   and clear the propagation flag. Mutates `form` in place. */
void sandhi_apply_natva(char *form);

/* 8.3.59 ādeśapratyayoḥ: A pratyaya-region `s` becomes ṣ (z) when
   preceded by an iṇ letter (i, ī, u, ū, ṛ, ṝ, ḷ, e, o, ai, au, k).
   Only `s`s at or after `stem_split` (the index where the suffix
   begins) are eligible. a/ā do NOT trigger this rule. Mutates
   `form` in place. */
void sandhi_apply_satva(char *form, size_t stem_split);

#endif /* SANDHI_NATVA_H */
