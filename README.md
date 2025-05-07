# siRNA-predict
![License](https://img.shields.io/badge/license-MIT-yellow) 

A fast siRNA predictor in no model organisms. Files of sequences should be recongnized as FASTA format.

## Install

Just type

    git clone https://github.com/gotouerina/siRNA-predict.git
    make;

## Usage

### sirna_finder

    Usage: sirna_finder <input.fasta>

    Extract 21nt siRNA candidates satisfying Ui-Tei, Reynolds, and Amarzguioui rules.

### offtarget_search

    Usage: ./offtarget_search <siRNA_list.txt> <CDS_fasta.fa> <max_mismatch>

    <siRNA_list.txt>: one 21nt antisense sequence per line (U alphabet or T/U mix)
  
    <CDS_fasta.fa>:    FASTA of CDS (will convert T->U)
  
    <max_mismatch>:    integer ≥0
  
# Cite

Cite: Please cite website (https://github.com/gotouerina/siRNA-predict)) when you use this pipeline, there is no paper published now.
