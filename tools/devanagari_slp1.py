"""Shared Devanagari <-> SLP1 conversion helpers for ingestion and oracles."""

from __future__ import annotations

import unicodedata

VIRAMA = "\u094d"

VOWELS = {
    "अ": "a", "आ": "A", "इ": "i", "ई": "I", "उ": "u", "ऊ": "U",
    "ऋ": "f", "ॠ": "F", "ऌ": "x", "ॡ": "X",
    "ए": "e", "ऐ": "E", "ओ": "o", "औ": "O",
}

CONSONANTS = {
    "क": "k", "ख": "K", "ग": "g", "घ": "G", "ङ": "N",
    "च": "c", "छ": "C", "ज": "j", "झ": "J", "ञ": "Y",
    "ट": "w", "ठ": "W", "ड": "q", "ढ": "Q", "ण": "R",
    "त": "t", "थ": "T", "द": "d", "ध": "D", "न": "n",
    "प": "p", "फ": "P", "ब": "b", "भ": "B", "म": "m",
    "य": "y", "र": "r", "ल": "l", "व": "v",
    "श": "S", "ष": "z", "स": "s", "ह": "h",
}

MATRAS = {
    "ा": "A", "ि": "i", "ी": "I", "ु": "u", "ू": "U",
    "ृ": "f", "ॄ": "F", "ॢ": "x", "ॣ": "X",
    "े": "e", "ै": "E", "ो": "o", "ौ": "O",
}

SIGNS = {"ं": "M", "ः": "H", "ँ": "~"}

SLP1_VOWELS = {v: k for k, v in VOWELS.items()}
SLP1_CONSONANTS = {v: k for k, v in CONSONANTS.items()}
SLP1_MATRAS = {v: k for k, v in MATRAS.items()}
SLP1_SIGNS = {v: k for k, v in SIGNS.items()}


def deva_to_slp1(text: str) -> str:
    """Convert Devanagari to SLP1, preserving inherent consonant vowels."""
    text = unicodedata.normalize("NFC", text or "")
    out: list[str] = []
    i = 0
    while i < len(text):
        ch = text[i]
        nxt = text[i + 1] if i + 1 < len(text) else ""

        if ch in CONSONANTS:
            out.append(CONSONANTS[ch])
            if nxt == VIRAMA:
                i += 2
            elif nxt in MATRAS:
                out.append(MATRAS[nxt])
                i += 2
            else:
                out.append("a")
                i += 1
            continue

        if ch in VOWELS:
            out.append(VOWELS[ch])
        elif ch in SIGNS:
            out.append(SIGNS[ch])
        elif ch == VIRAMA:
            pass
        elif ord(ch) < 128:
            out.append(ch)
        i += 1
    return "".join(out)


def slp1_to_devanagari(text: str) -> str:
    """Convert SLP1 to Devanagari for strict oracle comparison."""
    out: list[str] = []
    previous_consonant = False
    for ch in text or "":
        if ch in SLP1_VOWELS:
            if previous_consonant and ch != "a":
                out.append(SLP1_MATRAS[ch])
            elif not previous_consonant:
                out.append(SLP1_VOWELS[ch])
            previous_consonant = False
        elif ch in SLP1_CONSONANTS:
            if previous_consonant:
                out.append(VIRAMA)
            out.append(SLP1_CONSONANTS[ch])
            previous_consonant = True
        elif ch in SLP1_SIGNS:
            if previous_consonant:
                previous_consonant = False
            out.append(SLP1_SIGNS[ch])
        else:
            if previous_consonant:
                out.append(VIRAMA)
                previous_consonant = False
            out.append(ch)
    if previous_consonant:
        out.append(VIRAMA)
    return unicodedata.normalize("NFC", "".join(out))
