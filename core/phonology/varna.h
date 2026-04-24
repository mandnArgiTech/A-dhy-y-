/* varna.h — phoneme type system; implemented in Story 1.1 */
#ifndef VARNA_H
#define VARNA_H
#include <stdbool.h>
#include <stdint.h>
#define VF_VOWEL     (1u<<0)
#define VF_CONSONANT (1u<<1)
#define VF_SHORT     (1u<<2)
#define VF_LONG      (1u<<3)
#define VF_NASAL     (1u<<8)
#define VF_ASPIRATE  (1u<<9)
#define VF_VOICED    (1u<<10)
#define VF_VOICELESS (1u<<11)
#define VF_ANUSVARA  (1u<<12)
#define VF_VISARGA   (1u<<13)
#define VF_SEMIVOWEL (1u<<14)
#define VF_SIBILANT  (1u<<15)
#define VF_KANTYA    (1u<<16)
#define VF_TALAVYA   (1u<<17)
#define VF_MURDHANYA (1u<<18)
#define VF_DANTYA    (1u<<19)
#define VF_OSHTHYA   (1u<<20)
#define VF_NASIKA    (1u<<21)
typedef struct { char slp1; uint32_t flags; } Varna;
Varna    varna_from_slp1(char c);
uint32_t varna_flags(char slp1);
bool varna_is_savarna(char a, char b);
bool varna_is_vowel(char slp1);
bool varna_is_consonant(char slp1);
bool varna_is_short(char slp1);
bool varna_is_long(char slp1);
bool varna_is_nasal(char slp1);
bool varna_is_semivowel(char slp1);
bool varna_is_sibilant(char slp1);
bool varna_is_voiced(char slp1);
bool varna_is_voiceless(char slp1);
bool varna_is_aspirate(char slp1);
bool varna_is_valid_slp1(char c);
char varna_savarna_rep(char slp1);
char varna_lengthen(char short_vowel);
char varna_shorten(char long_vowel);
char varna_guna(char vowel);
char varna_vrddhi(char vowel);
#endif
