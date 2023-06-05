#include "shell.h"

#include "gaps.h"
#include "stats.h"

#include <stdio.h>
void shell_sort(Stats *stats, uint32_t *arr, uint32_t length) {
    for (uint32_t gap_i = 0; gap_i < GAPS; gap_i++) {
        for (uint32_t i = gaps[gap_i]; i < length; i++) {
            uint32_t j = i;
            uint32_t temp = arr[i];
            //cmp(stats, j, gaps[gap_i]);
            if (j >= gaps[gap_i]) {
                cmp(stats, temp, gaps[gap_i]);
            }
            while (j >= gaps[gap_i] && temp < arr[j - gaps[gap_i]]) {
                arr[j] = arr[j - gaps[gap_i]];
                move(stats, arr[j]);
                j -= gaps[gap_i];
            }
            arr[j] = temp;
            move(stats, arr[j]);
        }
    }
}
