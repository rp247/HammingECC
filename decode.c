#include "bm.h"
#include "bv.h"
#include "hamming.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/* helper function to print usage */
static void usage(char *argv) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "A Hamming(8, 4) systematic code generator.\n"
        "\n"
        "USAGE\n"
        "%s [-h] [-v] [-i infile] [-o outfile]\n"
        "\n"
        "OPTIONS\n"
        "-h             Program usage and help.\n"
        "-v		Print statistics of decoding to stderr.\n"
        "-i infile      Input data to encode.\n"
        "-o outfile     Output of encoded data.\n",
        argv);
    return;
}

/* CREDITS: Helper function from the lab document */
/* packs two nibbles into a byte */
static uint8_t pack_byte(uint8_t upper, uint8_t lower) {
    return (upper << 4) | (lower & 0xF);
}

/* helper function to close files open in main */
static void main_err(FILE *in, FILE *out) {
    if (in)
        fclose(in);
    if (out)
        fclose(out);
    return;
}

/* helper function to initialize transpose matrix */
static void initialize_ht(BitMatrix *Ht) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 4; j++) {
            /* 1 in upper diagonal */
            if (i < 4 && i != j)
                bm_set_bit(Ht, i, j);
            /* 0 in lower diagonal */
            if (i > 3 && j == i - 4)
                bm_set_bit(Ht, i, j);
        }
    }
    return;
}

int main(int argc, char **argv) {

    int c;
    char *optlist = "hvi:o:";
    uint8_t verbose = 0; // just one flag so no create set

    /* default file values */
    FILE *infile = stdin;
    FILE *outfile = stdout;

    /* parse args and open file */
    while ((c = getopt(argc, argv, optlist)) != -1) {
        switch (c) {
        case 'h': usage(argv[0]); return 0;

        case 'v': verbose = 1; break;

        case 'i':
            infile = fopen(optarg, "rb");
            if (!infile) {
                fprintf(stderr, "Error: Cannot open input file.\n");
                return -1;
            }
            break;

        case 'o':
            outfile = fopen(optarg, "wb");
            if (!outfile) {
                fprintf(stderr, "Error: Cannot open ouput file.\n");
                return -1;
            }
            break;

        default: usage(argv[0]); return -1;
        }
    }

    /* CREDITS: Modified version (for err handling) of the code snippet in the lab documentation */
    /* file permission setting */
    struct stat statbuf;

    /* get infile stats */
    if (fstat(fileno(infile), &statbuf) != 0) {
        fprintf(stderr, "Failed to get input file stat.\n");
        main_err(infile, outfile);
        return -1;
    }

    /* change file mode */
    if (fchmod(fileno(outfile), statbuf.st_mode) != 0) {
        fprintf(stderr, "Could not change mode for output file.\n");
        main_err(infile, outfile);
        return -1;
    }

    /* create transpose (8x4) matrix */
    BitMatrix *Ht = bm_create(8, 4);
    initialize_ht(Ht); // set the transpose matrix

    int inp_byte; // gets the byte
    uint8_t lower, upper, og_byte;
    HAM_STATUS res;

    /* keeps count of statistics */
    /* 0: Bytes processed count. 1: Corrected, 2: Uncorrected, 3: okay (to make it less hacky) */
    uint64_t stat_table[4] = { 0, 0, 0, 0 };

    /* indexes to stat_table */
    enum stat_index { Bytes = 0, Correct, Uncorrected };

    while ((inp_byte = fgetc(infile)) != EOF) { // check for both (what if one byte only)
        /* decode lower byte */
        res = ham_decode(Ht, (uint8_t) inp_byte, &lower);
        stat_table[-1 * res]++; // update statistics (*-1 since ham_status < 0)

        /* get and decode upper byte */
        inp_byte = fgetc(infile);
        res = ham_decode(Ht, (uint8_t) inp_byte, &upper);
        stat_table[-1 * res]++;

        /* pack both */
        og_byte = pack_byte(upper, lower);

        /* write to outfile */
        fputc(og_byte, outfile);

        stat_table[Bytes] += 2; // atleast two bytes get processed
    }

    /* print statistics if possible */
    if (verbose) {
        fprintf(stderr, "Total bytes processed: %" PRIu64 "\n", stat_table[Bytes]);
        fprintf(stderr, "Uncorrected error: %" PRIu64 "\n", stat_table[Uncorrected]);
        fprintf(stderr, "Corrected errors: %" PRIu64 "\n", stat_table[Correct]);
        fprintf(
            stderr, "Error rate: %0.6lf\n", (double) stat_table[Uncorrected] / stat_table[Bytes]);
    }

    /* closing/freeing mem */
    main_err(infile, outfile);
    bm_delete(&Ht);
    return 0;
}
