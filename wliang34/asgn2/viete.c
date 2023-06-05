#include "mathlib.h"

#include <stdint.h>
#include <stdio.h>
static int k = 0;
static double last_nom = 0;
static double value = 1;
double last_term;
double pi_viete() {
    last_nom = sqrt_newton(last_nom + 2);
    last_term = last_nom / 2;
    value *= last_term;
    k += 1;
    return 2 / value;
}

int pi_viete_factors() {
    return k - 1;
}
