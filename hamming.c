#include "hamming.h"

#include "bm.h"
#include "bv.h"

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#define NIBBLE 4
#define BYTE   8

/* lookup table */
static int8_t lookup[16]
    = { 0, 4, 5, HAM_ERR, 6, HAM_ERR, HAM_ERR, 3, 7, HAM_ERR, HAM_ERR, 2, HAM_ERR, 1, 0, HAM_ERR };

/* CREDITS: Helper function from the lab document */
/* extracts lower 4 bits of a byte */
uint8_t lower_nibble(uint8_t val) {
    return val & 0xF;
}

/* CREDITS: Helper function from the lab document */
/* extracts upper 4 bits of a byte */
uint8_t upper_nibble(uint8_t val) {
    return val >> 4;
}

/* CREDITS: Helper function from the lab document */
/* packs two nibbles into a byte */
uint8_t pack_byte(uint8_t upper, uint8_t lower) {
    return (upper << 4) | (lower & 0xF);
}

/* encodes lower nibble of msg with Hamming (8, 4) even parity */
uint8_t ham_encode(BitMatrix *G, uint8_t msg) {
    if (!G)
        return 0;

    uint8_t lower = lower_nibble(msg); // get lower byte

    BitMatrix *lo_mat = bm_from_data(lower, NIBBLE); // convert to matrix

    BitMatrix *encoded = bm_multiply(lo_mat, G); // get encoded matrix

    lower = bm_to_data(encoded); // back to int

    bm_delete(&lo_mat);
    bm_delete(&encoded);

    return lower;
}

/* decodes the Hamming (8, 4) code, stores in msg, returns status */
HAM_STATUS ham_decode(BitMatrix *Ht, uint8_t code, uint8_t *msg) {

    BitMatrix *code_mat = bm_from_data(code, BYTE); // convert to matrix
    BitMatrix *decoded = bm_multiply(code_mat, Ht); // get decoded matrix

    HAM_STATUS ret_val; // to ignore repeating bm_delete every time
    uint8_t err = bm_to_data(decoded);

    if (!err) {
        if (msg)
            *msg = lower_nibble(bm_to_data(code_mat)); // store lower nibble to msg
        ret_val = HAM_OK;
    }

    /* look up the table */
    else {
        int8_t looked_up = lookup[err];

        /* can correct error */
        if (looked_up != HAM_ERR) {
            /* flip bit (cant use xor in bv [cant access code_mat->vector]) */
            if (bm_get_bit(code_mat, 0, looked_up))
                bm_clr_bit(code_mat, 0, looked_up);
            else
                bm_set_bit(code_mat, 0, looked_up);

            if (msg)
                *msg = lower_nibble(bm_to_data(code_mat));

            ret_val = HAM_CORRECT;
        }

        /* cannot be corrected */
        else {
            ret_val = HAM_ERR;
        }
    }

    bm_delete(&code_mat);
    bm_delete(&decoded);
    return ret_val;
}
