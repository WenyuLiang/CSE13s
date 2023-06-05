#include "mathlib.h"

#include <stdint.h>
#include <stdio.h>
static int k = 0;
double sqrt_newton(double x) {
    double f = 1.0;
    double y = 1.0;
    while (x > 4) {
        x /= 4.0;
        f *= 2.0;
    }
    double guess = 0.0;
    while (absolute(y - guess) > EPSILON) {
        guess = y;
        y = (y + x / y) / 2.0;
        k += 1;
    }
    return f * y;
}
int sqrt_newton_iters() {
    return k;
}
