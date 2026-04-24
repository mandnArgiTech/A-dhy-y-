/* varna.c — stub; implemented in Story 1.1 */
#include "varna.h"
Varna    varna_from_slp1(char c)      { (void)c; Varna v={0}; return v; }
uint32_t varna_flags(char slp1)       { (void)slp1; return 0; }
bool varna_is_savarna(char a, char b) { (void)a;(void)b; return false; }
bool varna_is_vowel(char c)      { return c=='a'||c=='A'||c=='i'||c=='I'||c=='u'||c=='U'||
                                          c=='f'||c=='F'||c=='x'||c=='X'||c=='e'||c=='E'||
                                          c=='o'||c=='O'; }
bool varna_is_consonant(char c)  { return varna_is_valid_slp1(c) && !varna_is_vowel(c) &&
                                          c!='M' && c!='H'; }
bool varna_is_short(char c)      { return c=='a'||c=='i'||c=='u'||c=='f'||c=='x'; }
bool varna_is_long(char c)       { return c=='A'||c=='I'||c=='U'||c=='F'||c=='X'||
                                          c=='e'||c=='E'||c=='o'||c=='O'; }
bool varna_is_nasal(char c)      { return c=='N'||c=='Y'||c=='R'||c=='n'||c=='m'; }
bool varna_is_semivowel(char c)  { return c=='y'||c=='r'||c=='l'||c=='v'; }
bool varna_is_sibilant(char c)   { return c=='S'||c=='z'||c=='s'||c=='h'; }
bool varna_is_voiced(char c)     { (void)c; return false; }
bool varna_is_voiceless(char c)  { (void)c; return false; }
bool varna_is_aspirate(char c)   { return c=='K'||c=='G'||c=='C'||c=='J'||c=='W'||c=='Q'||
                                          c=='T'||c=='D'||c=='P'||c=='B'; }
bool varna_is_valid_slp1(char c) {
  return c=='a'||c=='A'||c=='i'||c=='I'||c=='u'||c=='U'||c=='f'||c=='F'||
         c=='x'||c=='X'||c=='e'||c=='E'||c=='o'||c=='O'||c=='M'||c=='H'||
         c=='k'||c=='K'||c=='g'||c=='G'||c=='N'||c=='c'||c=='C'||c=='j'||
         c=='J'||c=='Y'||c=='w'||c=='W'||c=='q'||c=='Q'||c=='R'||c=='t'||
         c=='T'||c=='d'||c=='D'||c=='n'||c=='p'||c=='P'||c=='b'||c=='B'||
         c=='m'||c=='y'||c=='r'||c=='l'||c=='v'||c=='S'||c=='z'||c=='s'||c=='h';
}
char varna_savarna_rep(char c)   { return c; }
char varna_lengthen(char c) {
  switch(c){ case 'a':return 'A'; case 'i':return 'I'; case 'u':return 'U';
             case 'f':return 'F'; case 'x':return 'X'; default:return c; }
}
char varna_shorten(char c) {
  switch(c){ case 'A':return 'a'; case 'I':return 'i'; case 'U':return 'u';
             case 'F':return 'f'; case 'X':return 'x'; default:return c; }
}
char varna_guna(char c) {
  switch(c){ case 'i':case 'I':return 'e'; case 'u':case 'U':return 'o';
             case 'f':case 'F':return 'a'; /* ar — caller handles 2-char */
             default:return c; }
}
char varna_vrddhi(char c) {
  switch(c){ case 'a':case 'A':return 'A'; case 'i':case 'I':return 'E';
             case 'u':case 'U':return 'O'; case 'f':case 'F':return 'A';
             default:return c; }
}
