// offtarget_search.c
// Usage: offtarget_search <siRNA_list.txt> <CDS_fasta.fa> <max_mismatch>
//   Reads siRNA guides (21nt antisense, U alphabet) and whole-CDS FASTA,
//   computes sense (reverse-complement) of each guide, then scans each CDS
//   window for ≤ max_mismatch mismatches to the sense. Outputs TSV:
//     CDS_ID    Start    End    Mismatches    Guide(antisense)    TargetSeq

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE    1024
#define MAX_SEQ   200000
#define SIRNA_LEN    21

// reverse-complement: guide (antisense) -> sense
void revcomp_fixed(const char *in, char *out, int L) {
    for (int i = 0; i < L; i++) {
        char c = toupper((unsigned char)in[L-1-i]);
        switch (c) {
            case 'A': out[i] = 'U'; break;
            case 'U': out[i] = 'A'; break;
            case 'T': out[i] = 'A'; break;  // treat T as U
            case 'C': out[i] = 'G'; break;
            case 'G': out[i] = 'C'; break;
            default:  out[i] = 'N'; break;
        }
    }
    out[L] = '\0';
}

void print_help(const char *prog) {
    printf("Usage: %s <siRNA_list.txt> <CDS_fasta.fa> <max_mismatch>\n", prog);
    printf("  <siRNA_list.txt>: one 21nt antisense sequence per line (U alphabet or T/U mix)\n");
    printf("  <CDS_fasta.fa>:    FASTA of CDS (will convert T->U)\n");
    printf("  <max_mismatch>:    integer ≥0\n");
}

int main(int argc, char *argv[]) {
    if (argc != 4 || strcmp(argv[1], "--help") == 0) {
        print_help(argv[0]);
        return 0;
    }

    // Read siRNA guides
    FILE *fs = fopen(argv[1], "r");
    if (!fs) { perror("Opening siRNA file"); return 1; }
    char line[MAX_LINE];
    char **guides = NULL, **senses = NULL;
    int n_sirna = 0;
    while (fgets(line, sizeof(line), fs)) {
        // strip newline
        line[strcspn(line, "\r\n")] = '\0';
        if (line[0] == '>' || line[0] == '\0') continue;
        int L = strlen(line);
        if (L != SIRNA_LEN) continue;
        // normalize guide: uppercase + T->U
        char *guide = malloc(L+1);
        for (int i = 0; i < L; i++) {
            char c = toupper((unsigned char)line[i]);
            guide[i] = (c == 'T' ? 'U' : c);
        }
        guide[L] = '\0';
        // compute sense
        char *sense = malloc(L+1);
        revcomp_fixed(guide, sense, L);
        // store
        guides = realloc(guides, (n_sirna+1)*sizeof(char*));
        senses = realloc(senses, (n_sirna+1)*sizeof(char*));
        guides[n_sirna] = guide;
        senses[n_sirna] = sense;
        n_sirna++;
    }
    fclose(fs);
    if (n_sirna == 0) {
        fprintf(stderr, "No valid siRNA found in %s\n", argv[1]);
        return 1;
    }
    int max_mis = atoi(argv[3]);
    if (max_mis < 0) max_mis = 0;

    // Prepare to read CDS FASTA
    FILE *fc = fopen(argv[2], "r");
    if (!fc) { perror("Opening CDS file"); return 1; }
    char header[256] = "";
    static char seq[MAX_SEQ];
    int seqlen = 0;

    // Print TSV header
    printf("CDS_ID\tStart\tEnd\tMismatches\tGuide\tTargetSeq\n");

    // Process FASTA
    while (fgets(line, sizeof(line), fc)) {
        if (line[0] == '>') {
            // process previous record
            if (seqlen >= SIRNA_LEN) {
                seq[seqlen] = '\0';
                for (int i = 0; i <= seqlen - SIRNA_LEN; i++) {
                    for (int k = 0; k < n_sirna; k++) {
                        int mis = 0;
                        for (int j = 0; j < SIRNA_LEN; j++) {
                            char c1 = seq[i+j];       // already U alphabet
                            char c2 = senses[k][j];
                            if (c1 != c2 && ++mis > max_mis) break;
                        }
                        if (mis <= max_mis) {
                            // output
                            printf("%s\t%d\t%d\t%d\t%s\t",
                                   header, i+1, i+SIRNA_LEN, mis, guides[k]);
                            // target sequence
                            fwrite(seq+i, 1, SIRNA_LEN, stdout);
                            printf("\n");
                        }
                    }
                }
            }
            // new header
            sscanf(line+1, "%255s", header);
            seqlen = 0;
        } else {
            // accumulate sequence (normalize T->U)
            for (char *p = line; *p; ++p) {
                if (isalpha((unsigned char)*p)) {
                    char c = toupper((unsigned char)*p);
                    if (c == 'T') c = 'U';
                    seq[seqlen++] = c;
                    if (seqlen >= MAX_SEQ - SIRNA_LEN) break;
                }
            }
        }
    }
    // last record
    if (seqlen >= SIRNA_LEN) {
        seq[seqlen] = '\0';
        for (int i = 0; i <= seqlen - SIRNA_LEN; i++) {
            for (int k = 0; k < n_sirna; k++) {
                int mis = 0;
                for (int j = 0; j < SIRNA_LEN; j++) {
                    char c1 = seq[i+j];
                    char c2 = senses[k][j];
                    if (c1 != c2 && ++mis > max_mis) break;
                }
                if (mis <= max_mis) {
                    printf("%s\t%d\t%d\t%d\t%s\t",
                           header, i+1, i+SIRNA_LEN, mis, guides[k]);
                    fwrite(seq+i, 1, SIRNA_LEN, stdout);
                    printf("\n");
                }
            }
        }
    }
    fclose(fc);

    // free memory
    for (int i = 0; i < n_sirna; i++) {
        free(guides[i]);
        free(senses[i]);
    }
    free(guides);
    free(senses);
    return 0;
}