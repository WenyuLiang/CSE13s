#include "mathlib.h"

#include <stdint.h>
#include <stdio.h>

static int k = 1;
static double nom = 1.0;
static double value = 1.0;

double pi_madhava() {
    double term;
    nom *= -(1.0 / 3.0);
    term = nom / (2 * k + 1);
    value += term;
    k += 1;
    return sqrt_newton(12) * value;
}
int pi_madhava_terms() {
    return k - 1;
}
