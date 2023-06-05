#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include "randstate.h"
#include "numtheory.h"
#include <string.h>
#include <limits.h>

//implement lcm for ss_make_priv
static void lcm(mpz_t p, mpz_t q, mpz_t o) {
    mpz_t t;
    mpz_init(t);
    gcd(t, p, q);
    mpz_mul(o, p, q);
    mpz_fdiv_q(o, o, t);
    mpz_clear(t);
}

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    uint64_t lower_p = nbits / 5;
    uint64_t upper_p = nbits * 2 / 5;
    //making sure the bits_p is in the given range
    uint64_t bits_p = random() % (upper_p - lower_p) + lower_p;
    make_prime(p, bits_p, iters);
    mpz_t p_sqr;
    mpz_init(p_sqr);
    mpz_mul(p_sqr, p, p);
    uint64_t bits_q = nbits - 2 * bits_p;
    make_prime(q, bits_q, iters);
    mpz_mul(n, p_sqr, q);
    mpz_clear(p_sqr);
}

void ss_make_priv(mpz_t d, mpz_t pq, mpz_t p, mpz_t q) {
    // Calculate lambda(n) = lcm(p-1, q-1)
    mpz_t p_minus_one, q_minus_one, 𝝺, n;
    mpz_inits(p_minus_one, q_minus_one, 𝝺, n, NULL);
    mpz_sub_ui(p_minus_one, p, 1);
    mpz_sub_ui(q_minus_one, q, 1);
    lcm(p_minus_one, q_minus_one, 𝝺);
    mpz_mul(pq, p, q);
    mpz_mul(n, pq, p);
    // Calculate d = inverse of n modulo lambda(n)
    mod_inverse(d, n, 𝝺);
    // Clean up
    mpz_clears(p_minus_one, q_minus_one, 𝝺, n, NULL);
}

void ss_write_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%s\n", n, username);
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    char n_str[1024], username_str[_POSIX_LOGIN_NAME_MAX];

    // Read in n and username as strings
    if (fgets(n_str, sizeof(n_str), pbfile) == NULL) {
        fprintf(stderr, "Error reading SS public key\n");
        exit(1);
    }
    if (fgets(username_str, sizeof(username_str), pbfile) == NULL) {
        fprintf(stderr, "Error reading username\n");
        exit(1);
    }

    // Parse n as a hexadecimal number
    if (gmp_sscanf(n_str, "%Zx", n) != 1) {
        fprintf(stderr, "unable to parse public key\n");
        exit(1);
    }

    // Copy username string to username argument
    strncpy(username, username_str, sizeof(username_str));
    username[sizeof(username_str) - 1] = '\0';
    // Remove trailing newline from username
    if (strlen(username) > 0 && username[strlen(username) - 1] == '\n') {
        username[strlen(username) - 1] = '\0';
    }
}

void ss_write_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d);
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    char pq_str[1024], d_str[1024];

    if (fgets(pq_str, sizeof(pq_str), pvfile) == NULL) {
        fprintf(stderr, "Error reading private modulus\n");
        exit(1);
    }
    if (fgets(d_str, sizeof(d_str), pvfile) == NULL) {
        fprintf(stderr, "Error reading private exponent\n");
        exit(1);
    }

    if (gmp_sscanf(pq_str, "%Zx", pq) != 1) {
        fprintf(stderr, "unable to parse private modulus\n");
        exit(1);
    }
    if (gmp_sscanf(d_str, "%Zx", d) != 1) {
        fprintf(stderr, "unable to parse private exponent\n");
        exit(1);
    }
}

void ss_encrypt(mpz_t c, mpz_t m, mpz_t n) {
    pow_mod(c, m, n, n);
}

//
// Encrypt an arbitrary file
//
// Provides:
//  fills outfile with the encrypted contents of infile
//
// Requires:
//  infile: open and readable file stream
//  outfile: open and writable file stream
//  n: public exponent and modulus
//
void ss_encrypt_file(FILE *infile, FILE *outfile, mpz_t n) {
    uint64_t k = ((mpz_sizeinbase(n, 2) - 1) * 0.5 - 1) / 8;
    //uint8_t is 1 byte
    uint8_t *block = (uint8_t *) malloc(k * sizeof(uint8_t));
    block[0] = 0xFF;
    while (!feof(infile)) {
        // Read at most k-1 bytes into block
        size_t j = fread(&block[1], sizeof(uint8_t), k - 1, infile);

        // Convert block to mpz_t m
        mpz_t m;
        mpz_init(m);
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);

        // Encrypt m with ss_encrypt()
        mpz_t c;
        mpz_init(c);
        ss_encrypt(c, m, n);
        // Write encrypted number to outfile
        gmp_fprintf(outfile, "%Zx\n", c);

        // Clear m and c
        mpz_clear(m);
        mpz_clear(c);
    }

    // Free block array
    free(block);
}

void ss_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t pq) {
    pow_mod(m, c, d, pq);
}

// Decrypt a file back into its original form.
//
// Provides:
//  fills outfile with the unencrypted data from infile
//
// Requires:
//  infile: open and readable file stream to encrypted data
//  outfile: open and writable file stream
//  d: private exponent
//  pq: private modulus
//
void ss_decrypt_file(FILE *infile, FILE *outfile, mpz_t d, mpz_t pq) {
    uint64_t k = (mpz_sizeinbase(pq, 2) - 2) / 8;
    uint8_t *block;
    block = (uint8_t *) malloc(k * sizeof(uint8_t));
    // Loop through each line in infile
    char hexstr[k * 10];
    while (fscanf(infile, "%s\n", hexstr) != EOF) {
        // Convert hexstring to mpz_t
        mpz_t c;
        mpz_init(c);
        mpz_set_str(c, hexstr, 16);
        // Decrypt c to get m
        mpz_t m;
        mpz_init(m);
        ss_decrypt(m, c, d, pq);
        // Export m to block
        size_t j;
        mpz_export(&block[0], &j, 1, 1, 0, 0, m);

        // Starting from 1 to remove 0xFF
        fwrite(&block[1], 1, j - 1, outfile);

        // Clean up
        mpz_clear(c);
        mpz_clear(m);
    }

    // Free block
    free(block);
}
