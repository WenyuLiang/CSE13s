#include "mathlib.h"

#include <stdint.h>
#include <stdio.h>
static int k = 0;
static double factor = 1.0 / 16.0;
static double value;
double pi_bbp() {
    double mul1 = 1.0;
    double K = k;
    double mul2 = (K * (120 * K + 151) + 47) / (K * (K * (K * (512 * K + 1024) + 712) + 194) + 15);
    for (int i = 0; i < k; i++) {
        mul1 *= factor;
    }
    value += mul1 * mul2;
    k += 1;
    return value;
}

int pi_bbp_terms() {
    return k;
}
