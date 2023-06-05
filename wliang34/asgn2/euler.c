#include "mathlib.h"

#include <stdint.h>
#include <stdio.h>

static int k = 1;
static double value = 0;
double pi_euler() {
    value += 1.0 / (k * k);
    k += 1;
    return sqrt_newton(6 * value);
}

int pi_euler_terms() {
    return k - 1;
}
