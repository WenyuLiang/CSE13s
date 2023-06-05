#include "heap.h"

#include "stats.h"

#include <stdio.h>
uint32_t max_child(Stats *stats, uint32_t *A, uint32_t first, uint32_t last) {
    uint32_t left = 2 * first;
    uint32_t right = left + 1;
    //cmp(stats,right,last);
    if (right <= last) {
        cmp(stats, A[right - 1], A[left - 1]);
    }
    //cmp(stats,A[right - 1],A[left - 1]);
    if (right <= last && A[right - 1] > A[left - 1]) {
        return right;
    }
    return left;
}

void fix_heap(Stats *stats, uint32_t *A, uint32_t first, uint32_t last) {
    uint32_t found = 0;
    uint32_t mother = first;
    uint32_t great = max_child(stats, A, mother, last);
    //cmp(stats,mother,last);
    //if (mother <= last / 2){cmp(stats,found,0);}
    //cmp(stats,found,0);
    while (mother <= last / 2 && found == 0) {
        cmp(stats, A[mother - 1], A[great - 1]);
        if (A[mother - 1] < A[great - 1]) {
            swap(stats, &A[mother - 1], &A[great - 1]);
            mother = great;
            great = max_child(stats, A, mother, last);
        } else {
            found = 1;
        }
    }
}

void build_heap(Stats *stats, uint32_t *A, uint32_t first, uint32_t last) {
    for (uint32_t father = last / 2; father > first - 1; father--) {
        fix_heap(stats, A, father, last);
    }
}

void heap_sort(Stats *stats, uint32_t *A, uint32_t n) {
    uint32_t first = 1;
    uint32_t last = n;
    build_heap(stats, A, first, last);
    //for (int i = 0; i < 6; i++){printf("%d\t", A[i]);}
    for (uint32_t leaf = last; leaf > first; leaf--) {
        swap(stats, &A[first - 1], &A[leaf - 1]);
        fix_heap(stats, A, first, leaf - 1);
    }
}
