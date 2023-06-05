#include "mathlib.h"

#include <stdint.h>
#include <stdio.h>
static int k = 1;
static double term = 1;
static double value = 1;
double e() {
    term *= k;
    value += 1 / term;
    k += 1;
    return value;
}

int e_terms() {
    return k - 1;
}
