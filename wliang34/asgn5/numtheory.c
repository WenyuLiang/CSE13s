#include "randstate.h"

#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

//static because only used in this unit
static inline bool is_odd(mpz_t n) {
    return mpz_tstbit(n, 0) == 1;
}
static inline bool is_even(mpz_t n) {
    return mpz_tstbit(n, 0) == 0;
}
//following functions are learned from the given python implementation

void gcd(mpz_t g, mpz_t a, mpz_t b) {
    mpz_t t;
    mpz_init(t);
    //a_, b_ to avoid changing the given a,b
    mpz_t a_, b_;
    mpz_inits(a_, b_, NULL);
    mpz_set(a_, a);
    mpz_set(b_, b);
    while (mpz_cmp_ui(b_, 0) != 0) {
        mpz_set(t, b_);
        mpz_mod(b_, a_, b_);
        mpz_set(a_, t);
    }
    mpz_set(g, a_);
    mpz_clear(t);
    mpz_clears(a_, b_, NULL);
}

void mod_inverse(mpz_t o, mpz_t a, mpz_t n) {
    mpz_t r, rP, t, tP, q, rT, tT, rT2, tT2;
    mpz_inits(r, rP, t, tP, q, rT, tT, rT2, tT2, NULL);

    mpz_set(r, n);
    mpz_set(rP, a);
    mpz_set_ui(t, 0);
    mpz_set_ui(tP, 1);

    while (mpz_cmp_ui(rP, 0) != 0) {
        mpz_tdiv_q(q, r, rP);
        mpz_set(rT, r);
        mpz_set(r, rP);
        mpz_mul(rT2, q, rP);
        mpz_sub(rP, rT, rT2);

        mpz_set(tT, t);
        mpz_set(t, tP);
        mpz_mul(tT2, q, tP);
        mpz_sub(tP, tT, tT2);
    }

    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(o, 0);
        mpz_clears(r, rP, t, tP, q, rT, tT, rT2, tT2, NULL);
        return;
    }
    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(o, t, n);
        mpz_clears(r, rP, t, tP, q, rT, tT, rT2, tT2, NULL);
        return;
    } else {
        mpz_set(o, t);
        mpz_clears(r, rP, t, tP, q, rT, tT, rT2, tT2, NULL);
        return;
    }
}

void pow_mod(mpz_t o, mpz_t a, mpz_t d, mpz_t n) {
    mpz_set_ui(o, 1);
    mpz_t p, d_;
    mpz_init(p);
    mpz_init_set(d_, d);
    mpz_set(p, a);
    while (mpz_cmp_ui(d_, 0) > 0) {
        if (is_odd(d_)) {
            mpz_mul(o, o, p);
            mpz_mod(o, o, n);
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);
        mpz_tdiv_q_ui(d_, d_, 2);
    }
    mpz_clear(p);
    mpz_clear(d_);
}

void get_d_r(mpz_t n, mpz_t d, mpz_t r) {
    /* Factors n into the form d * 2 ** r. */
    mpz_set(d, n);
    mpz_set_ui(r, 0);
    while (is_even(d)) {
        mpz_tdiv_q_2exp(d, d, 1);
        mpz_add_ui(r, r, 1);
    }
}

bool witness(mpz_t a, mpz_t n) {
    /* The witness loop of the Miller-Rabin probabilistic primality test. */
    mpz_t d, r, x, y, n_minus_1;
    mpz_inits(d, r, x, y, n_minus_1, NULL);

    mpz_sub_ui(n_minus_1, n, 1);
    get_d_r(n_minus_1, d, r); // Factor n into d * 2**r + 1
    pow_mod(x, a, d, n);
    mpz_t two;
    mpz_init_set_ui(two, 2);
    mpz_t i;
    for (mpz_init(i); mpz_cmp(r, i) > 0; mpz_add_ui(i, i, 1)) {
        pow_mod(y, x, two, n);
        if (mpz_cmp_ui(y, 1) == 0 && mpz_cmp_ui(x, 1) != 0 && mpz_cmp(x, n_minus_1) != 0) {
            return true;
        }
        mpz_set(x, y);
    }
    mpz_clear(i);
    mpz_clear(two);
    return mpz_cmp_ui(x, 1) != 0;
}

bool is_prime(mpz_t n, uint64_t iters) {
    /* Miller-Rabin probabilistic primality test of n with confidence k. */
    if (mpz_cmp_ui(n, 2) < 0 || (mpz_cmp_ui(n, 2) != 0 && is_even(n))) {
        return false;
    }
    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0) {
        return true;
    }

    mpz_t n_minus_3;
    mpz_init(n_minus_3);
    mpz_sub_ui(n_minus_3, n, 3);
    for (uint64_t i = 0; i < iters; i++) {
        mpz_t a;
        mpz_init(a);
        mpz_urandomm(a, state, n_minus_3);
        mpz_add_ui(a, a, 2); // Euler witness (or liar)
        if (witness(a, n)) {
            mpz_clear(a);
            return false;
        }
        mpz_clear(a);
    }
    mpz_clear(n_minus_3);
    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
    while (1) {
        mpz_rrandomb(p, state, bits);
        if (is_prime(p, iters)) {
            break;
        }
    }
}
