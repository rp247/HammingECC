#include "bm.h"
#include "bv.h"
#include "hamming.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/* helper function to print usage (credits: idea from error c file) */
static void usage(char *argv) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "A Hamming(8, 4) systematic code generator.\n"
        "\n"
        "USAGE\n"
        "%s [-h] [-i infile] [-o outfile]\n"
        "\n"
        "OPTIONS\n"
        "-h             Program usage and help.\n"
        "-i infile      Input data to encode.\n"
        "-o outfile     Output of encoded data.\n",
        argv);
    return;
}

/* CREDITS: Helper function from the lab document */
/* extracts lower 4 bits of a byte */
static uint8_t lower_nibble(uint8_t val) {
    return val & 0xF;
}

/* CREDITS: Helper function from the lab document */
/* extracts upper 4 bits of a byte */
static uint8_t upper_nibble(uint8_t val) {
    return val >> 4;
}

/* helper function to close files open in main */
static void main_err(FILE *in, FILE *out) {
    if (in)
        fclose(in);
    if (out)
        fclose(out);
    return;
}

/* helper function to initialize generator matrix */
static void initialize_gm(BitMatrix *G) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            /* 1 in diagonal */
            if (j < 4 && j == i)
                bm_set_bit(G, i, j);
            /* 0 in diagonal */
            if (j > 3 && j != i + 4)
                bm_set_bit(G, i, j);
        }
    }
    return;
}

int main(int argc, char **argv) {

    int c;
    char *optlist = "hi:o:";

    /* default file values */
    FILE *infile = stdin;
    FILE *outfile = stdout;

    /* parse args and open file */
    while ((c = getopt(argc, argv, optlist)) != -1) {
        switch (c) {
        case 'h': usage(argv[0]); return 0;

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

    /* create generator (4x8) matrix */
    BitMatrix *G = bm_create(4, 8);
    initialize_gm(G); // set the generator matrix

    int8_t inp_byte;
    uint8_t lower, upper;
    while ((inp_byte = fgetc(infile)) != EOF) {

        /* get upper and lower nibbles */
        lower = lower_nibble((uint8_t) inp_byte); // credits: idea from the lab doc
        upper = upper_nibble((uint8_t) inp_byte);

        /* encode lower and upper nibbles */
        lower = ham_encode(G, lower);
        upper = ham_encode(G, upper);

        /* write to outfile */
        fputc(lower, outfile);
        fputc(upper, outfile);
    }

    /* closing/freeing mem */
    main_err(infile, outfile);
    bm_delete(&G);
    return 0;
}
