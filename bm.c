#include "bm.h"

#include "bv.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

/* CREDITS: based on the definition provided in the lab doc */
struct BitMatrix {
    uint32_t rows;
    uint32_t cols;
    BitVector *vector;
};

/* constructor for a BitMatrix */
BitMatrix *bm_create(uint32_t rows, uint32_t cols) {
    BitMatrix *m = (BitMatrix *) malloc(sizeof(BitMatrix));

    if (m) {
        m->rows = rows;
        m->cols = cols;
        m->vector = bv_create(rows * cols); // create a bit vector of len rows * cols

        if (!m->vector) {
            bv_delete(&(m->vector));
            free(m);
            m = NULL;
        }
    }

    return m;
}

/* destructor for a BitMatrix */
void bm_delete(BitMatrix **m) {
    if (*m && (*m)->vector) {
        bv_delete(&((*m)->vector));
        free(*m);
        *m = NULL;
    }
    return;
}

/* return BM rows */
uint32_t bm_rows(BitMatrix *m) {
    if (!m)
        return 0; // no matrix
    return m->rows;
}

/* return BM cols */
uint32_t bm_cols(BitMatrix *m) {
    if (!m)
        return 0;
    return m->cols;
}

/* sets the bit at index [row, col] */
void bm_set_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    if (!m)
        return;
    bv_set_bit(m->vector, (r * m->cols) + c); // index = r*n+c. credits: from the lab doc.
    return;
}

/* clears the bit at index [row, col] */
void bm_clr_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    if (!m)
        return;
    bv_clr_bit(m->vector, (r * m->cols) + c); // index = r*n+c. credits: from the lab doc.
    return;
}

/* gets the bit at index [row, col] */
uint8_t bm_get_bit(BitMatrix *m, uint32_t r, uint32_t c) {
    if (!m)
        return 0;
    return bv_get_bit(m->vector, (r * m->cols) + c); // index = r*n+c. credits: from the lab doc.
}

/* converts from binary/int to matrix (ex: 1100 or 12 to [0 0 1 1]) */
BitMatrix *bm_from_data(uint8_t byte, uint32_t length) {
    if (length > 8)
        return NULL; // can only be max 1 BYTE
    BitMatrix *m = bm_create(1, length); // create matrix mxn where m = 1, n = length

    if (m) {
        for (uint32_t i = 0; i < length; i++) {
            if (byte & (1 << i)) { // if byte[i] == 1, set the matrix bit
                bm_set_bit(m, 0, i);
            }
        }
    }

    return m;
}

/* converts from matrix to binary/int (ex: [0 0 1 1] to 1100 or 12) */
uint8_t bm_to_data(BitMatrix *m) {
    if (!m)
        return 0; // no matrix

    uint8_t temp = 0; // to store value

    for (uint8_t i = bv_length(m->vector); i > 0; i--) { // start with last bit of matrix
        if (bm_get_bit(m, 0, i - 1)) {
            temp |= (1 << (i - 1)); // make it msb, cont. (just ret byte instead? rem comment)
        }
    }

    return temp;
}

/* multiplies matrix A (mxn) with matrix B (nxp) (mod 2) */
BitMatrix *bm_multiply(BitMatrix *A, BitMatrix *B) {
    if (!A || !B || bm_cols(A) != bm_rows(B))
        return NULL; // mxn != nxp. cant multiply

    BitMatrix *m = bm_create(bm_rows(A), bm_cols(B)); // resulting matrix = mxp

    if (m) {
        uint8_t n, p, length, mult; // n = B rows, p = B cols
        n = bm_cols(A);
        p = bm_cols(B);
        length = bv_length(m->vector); // length of final matrix

        /* would have to calculate length entries (for C matrix) */
        for (uint32_t total = 0; total < length; total++) {
            /* do that B row times */
            for (uint32_t min_times = 0; min_times < n; min_times++) {
                /* total/p to get row number, total%p to get column */
                mult = bm_get_bit(A, total / p, min_times) & bm_get_bit(B, min_times, total % p);
                bv_xor_bit(m->vector, total, mult); // xor bits
            }
        }
    }

    return m;
}

/* prints the BitMatrix */
void bm_print(BitMatrix *m) {
    for (uint32_t r = 0; r < bm_rows(m); r++) {
        for (uint32_t c = 0; c < bm_cols(m); c++) {
            fprintf(stdout, "%c ", bm_get_bit(m, r, c) ? '1' : '0');
        }
        fprintf(stdout, "\n");
    }
    return;
}
