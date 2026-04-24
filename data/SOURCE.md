# Data Source Reference

All files in this directory are generated from:
**github.com/ashtadhyayi-com/data** (master branch)

## File → Source mapping

| TSV file | Source file | Generator script |
|----------|------------|-----------------|
| `sutras.tsv` | `sutraani/data.txt` | `tools/ingest_source.py` |
| `dhatupatha.tsv` | `dhatu/data.txt` | `tools/ingest_dhatupatha.py` |
| `ganapatha.tsv` | `ganapath/data.txt` + `shivasutra/data.txt` | `tools/ingest_ganapatha.py` |
| `pratyahara.tsv` | `pratyahara/data.txt` | `tools/ingest_pratyahara.py` |

## Counts (as of ingestion)

| Dataset | Count | Notes |
|---------|-------|-------|
| Sūtras | 3,983 | BORI edition (traditional = 3,959) |
| Dhātus | 2,259 | Includes extensions beyond traditional 1,943 |
| Gaṇas | 262 (251 distinct names) | |
| Gaṇa members | 5,639 | Including Māheśvara-sūtra gaṇa |
| Pratyāhāras | 52 | Including sup and tiṅ suffix pratyāhāras |
| Māheśvara-sūtras | 14 | From `shivasutra/data.txt` |

## Re-generation

```bash
python3 tools/ingest_source.py
python3 tools/ingest_dhatupatha.py
python3 tools/ingest_ganapatha.py
python3 tools/ingest_pratyahara.py
```

## Vendor fallbacks

The `vendor/` directory contains copies of the source JSON files
so ingestion works offline. These are committed to git.

| Fallback file | Source |
|--------------|--------|
| `vendor/sutra_fallback.json` | `sutraani/data.txt` |
| `vendor/dhatupatha_fallback.json` | `dhatu/data.txt` |
| `vendor/ganapatha_fallback.json` | `ganapath/data.txt` |
| `vendor/shivasutra_fallback.json` | `shivasutra/data.txt` |
| `vendor/pratyahara_fallback.json` | `pratyahara/data.txt` |
