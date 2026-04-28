#!/usr/bin/env python3
"""Unit tests for the shared Devanagari -> SLP1 converter."""

import unittest
import os
import sys

sys.path.insert(0, os.path.dirname(__file__))

from devanagari_slp1 import deva_to_slp1


class DevanagariSlp1Tests(unittest.TestCase):
    def test_representative_words(self):
        cases = {
            "भवति": "Bavati",
            "रामः": "rAmaH",
            "रामेण": "rAmeRa",
            "रामान्": "rAmAn",
            "कृष्ण": "kfzRa",
            "धर्म": "Darma",
            "तत्त्व": "tattva",
            "विष्णु": "vizRu",
            "नमस्कार": "namaskAra",
            "अग्नि": "agni",
            "मुहूर्त": "muhUrta",
            "सूत्र": "sUtra",
            "दृष्ट": "dfzwa",
            "पृथ्वी": "pfTvI",
            "भ्रातृ": "BrAtf",
            "स्वामिन्": "svAmin",
            "देव": "deva",
            "शिव": "Siva",
            "गृह": "gfha",
            "अहम्": "aham",
        }
        for deva, slp1 in cases.items():
            with self.subTest(deva=deva):
                self.assertEqual(slp1, deva_to_slp1(deva))


if __name__ == "__main__":
    unittest.main()
