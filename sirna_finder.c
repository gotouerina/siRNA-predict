#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_SEQ 200000
#define SIRNA_LEN 21

void revcomp(const char *sense, char *anti) {
    for (int i = 0; i < SIRNA_LEN; i++) {
        char c = toupper((unsigned char)sense[SIRNA_LEN - 1 - i]);
        switch (c) {
            case 'A': anti[i] = 'U'; break;
            case 'U':
            case 'T': anti[i] = 'A'; break;
            case 'C': anti[i] = 'G'; break;
            case 'G': anti[i] = 'C'; break;
            default:  anti[i] = 'N'; break;
        }
    }
    anti[SIRNA_LEN] = '\0';
}

int count_AU(const char *s, int start, int len) {
    int cnt = 0;
    for (int i = 0; i < len; i++) {
        char c = toupper((unsigned char)s[start + i]);
        if (c == 'A' || c == 'U') cnt++;
    }
    return cnt;
}


int has_long_GC(const char *s, int max_run) {
    int run = 0;
    for (int i = 0; s[i]; i++) {
        char c = toupper((unsigned char)s[i]);
        if (c == 'G' || c == 'C') {
            run++;
            if (run > max_run) return 1;
        } else {
            run = 0;
        }
    }
    return 0;
}

// Ui-Tei 
int check_UiTei(const char *sense, const char *anti) {
    char a0 = anti[0];
    char s0 = toupper((unsigned char)sense[0]);
    if (!(a0 == 'A' || a0 == 'U')) return 0;
    if (!(s0 == 'G' || s0 == 'C')) return 0;
    if (count_AU(anti, 0, 7) < 4) return 0;
    if (has_long_GC(anti, 9)) return 0;
    return 1;
}

// Reynolds 
int check_Reynolds(const char *sense) {
    int gc = 0;
    for (int i = 0; i < SIRNA_LEN; i++) {
        char c = toupper((unsigned char)sense[i]);
        if (c == 'G' || c == 'C') gc++;
    }
    if (gc < 7 || gc > 11) return 0;
    if (toupper((unsigned char)sense[2]) != 'A') return 0;
    if (toupper((unsigned char)sense[9]) != 'U') return 0;
    if (toupper((unsigned char)sense[12]) == 'G') return 0;
    if (toupper((unsigned char)sense[18]) != 'A') return 0;
    if (count_AU(sense, 14, 5) < 3) return 0;
    return 1;
}

// Amarzguioui 
int check_Amarz(const char *anti) {
    if (count_AU(anti, 0, 3) <= count_AU(anti, 18, 3)) return 0;
    if (anti[0] == 'U') return 0;
    if (anti[18] == 'G') return 0;
    return 1;
}


void process_sequence(const char *hdr, const char *seq) {
    int len = (int)strlen(seq);
    char sense[SIRNA_LEN + 1], anti[SIRNA_LEN + 1];
    for (int i = 0; i <= len - SIRNA_LEN; i++) {
        memcpy(sense, seq + i, SIRNA_LEN);
        sense[SIRNA_LEN] = '\0';
        revcomp(sense, anti);

        if (check_UiTei(sense, anti)
         && check_Reynolds(sense)
         && check_Amarz(anti)) {
            printf(">siRNA_%s_%d_%d\n", hdr, i + 1, i + SIRNA_LEN);
            printf("%s\n", anti);
        }
    }
}

void print_help() {
    printf("Usage: sirna_finder <input.fasta>\n");
    printf("Extract 21nt siRNA candidates satisfying Ui-Tei, Reynolds, and Amarzguioui rules.\n");
    printf("Options:\n");
    printf("  --help    Show this message.\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2 || strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }

    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        perror("Error opening input file");
        return 1;
    }

    char line[MAX_LINE];
    char header[256] = "";
    static char seq[MAX_SEQ];
    int seqlen = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (line[0] == '>') {
            if (seqlen > 0) {
                seq[seqlen] = '\0';
                process_sequence(header, seq);
                seqlen = 0;
            }

            sscanf(line + 1, "%255s", header);
        } else {
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
    if (seqlen > 0) {
        seq[seqlen] = '\0';
        process_sequence(header, seq);
    }

    fclose(fp);
    return 0;
}
