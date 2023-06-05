#include "stats.h"

#include <stdio.h>
void comparator(Stats *stats, uint32_t *A, uint32_t x, uint32_t y) {
    cmp(stats, A[x], A[y]);
    if (A[x] > A[y]) {
        swap(stats, &A[x], &A[y]);
    }
}

void batcher_sort(Stats *stats, uint32_t *A, uint32_t n) {
    if (n == 0) {
        return;
    }
    uint32_t t = 0;
    uint32_t l = 0;
    uint32_t mask = 1;
    while (l < 32) {
        mask = mask << 1;
        l += 1;
        if ((mask & n) != 0) {
            t = l;
        }
    }
    t = t + 1;
    uint32_t p = 1 << (t - 1);
    uint32_t q, r, d;
    while (p > 0) {
        q = 1 << (t - 1);
        r = 0;
        d = p;
        while (d > 0) {
            for (uint32_t i = 0; i < n - d; i++) {
                if ((i & p) == r) {
                    comparator(stats, A, i, i + d);
                }
            }
            d = q - p;
            q >>= 1;
            r = p;
        }
        p >>= 1;
    }
}
