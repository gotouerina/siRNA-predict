# siRNA‑predict  
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)  
A high‑performance command‑line pipeline for siRNA design and off‑target screening in any organism with available CDS FASTA.

---

## Table of Contents

- [Features](#features)  
- [Requirements](#requirements)  
- [Installation](#installation)  
- [Usage](#usage)  
  - [1. `sirna_finder`](#1-sirna_finder)  
  - [2. `offtarget_search`](#2-offtarget_search)  
- [Examples](#examples)  
- [Configuration](#configuration)  
- [Contributing](#contributing)  
- [License](#license)  
- [Citation](#citation)  

---

## Features

- **`sirna_finder`**  
  - Scans any RNA/FASTA input for 21‑nt windows  
  - Applies **Ui‑Tei**, **Reynolds**, and **Amarzguioui** rules (as in siDirect 2.0)  
  - Outputs **antisense** (guide) strands in FASTA format with genomic coordinates  

- **`offtarget_search`**  
  - Takes a list of guide strands + CDS FASTA  
  - Converts T→U, computes reverse‑complement (sense) for each guide  
  - Scans all CDS windows for ≤ _N_ mismatches  
  - Reports **CDS_ID**, **start–end**, **mismatch count**, **guide**, **target sequence** in TSV  

- **Performance**  
  - Written in optimized C, single‑threaded but very fast on large genomes  
  - Minimal dependencies: only **GCC** and **make**

---

## Requirements

- **Unix‑like OS** (Linux, macOS, WSL)  
- **GCC** ≥ 4.8 (for C99 support)  
- **make**  
- Optional: git, for cloning

---

## Installation

```bash
git clone https://github.com/gotouerina/siRNA-predict.git
cd siRNA-predict
make
```

This produces two executables in the project root:

- `sirna_finder`  
- `offtarget_search`  

---

## Usage

### 1. `sirna_finder`

```bash
Usage: sirna_finder <input.fasta>  
```

- **`<input.fasta>`**: RNA or cDNA FASTA file (multi‑FASTA supported).  
- **Output**: FASTA of 21‑nt antisense guides that pass all three design rules.  
- **Header format**: `>siRNA_<ORIG_HEADER>_<start>_<end>` gives positions in the original sequence.

#### Example

```bash
./sirna_finder transcripts.fa > candidates.fa
```

---

### 2. `offtarget_search`

```bash
Usage: offtarget_search <siRNA_list.txt> <CDS_fasta.fa> <max_mismatch>  
```

- **`<siRNA_list.txt>`**: FASTA format siRNA sequences (one 21‑nt guide per block; U or T/U mix).  
- **`<CDS_fasta.fa>`**: Full set of CDS sequences (FASTA).  
- **`<max_mismatch>`**: Non‑negative integer (e.g., `0`, `1`, `2`).  

#### Output (TSV with header)

```
CDS_ID    Start    End    Mismatches    Guide    TargetSeq
```

- **`Guide`**: antisense (U‑alphabet) as provided.  
- **`TargetSeq`**: matching CDS window (U‑alphabet).  

#### Example

```bash
./offtarget_search candidates.fa all_CDS.fa 2 > off_targets.tsv
```

---

## Examples

1. **Design**  
   ```bash
   ./sirna_finder input_RNA.fa > siRNA_candidates.fa
   grep -v "^>" siRNA_candidates.fa | wc -l  # count candidates
   ```
2. **Off‑target screen**  
   ```bash
   ./offtarget_search siRNA_candidates.fa genome_CDS.fa 1 \
     | head -n 20
   ```

---

## Configuration

- You can adjust `SIRNA_LEN` in `sirna_finder.c` and `offtarget_search.c` for different guide lengths.  
- Tuning mismatch threshold (`max_mismatch`， default: 2) allows more or fewer off‑target hits.  

---

## Contributing

1. Fork the repo  
2. Create a feature branch (`git checkout -b feature/...`)  
3. Commit your changes (`git commit -am 'Add new feature'`)  
4. Push to the branch (`git push origin feature/...`)  
5. Open a Pull Request  

Please follow the existing **C coding style** and add tests if possible.

---

## License

This project is released under the [MIT License](LICENSE).  

---

## Citation

If you use **siRNA‑predict** in your work, please cite:

> “siRNA‑predict: a fast siRNA design and off‑target screening pipeline.”  
> https://github.com/gotouerina/siRNA‑predict  

No peer‑reviewed publication is available at this time.
