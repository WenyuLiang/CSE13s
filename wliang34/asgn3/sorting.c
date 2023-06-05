#define OPTIONS "ahbsqn:p:r:H"
#include "batcher.h"
#include "gaps.h"
#include "heap.h"
#include "quick.h"
#include "set.h"
#include "shell.h"
#include "stats.h"

#include <getopt.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* fill_random function is mainly borrowed from Dr. Long*/
int fill_random(uint32_t *Array, const uint32_t length, uint32_t seed) {
    srandom(seed);
    for (uint32_t i = 0; i < length; i += 1) {
        Array[i] = (random() & 0x3FFFFFFF);
    }
    return 0;
}
/* print_array function is mainly borrowed from Dr. Long*/
int print_array(uint32_t *Array, const uint32_t length, uint32_t p) {
    for (uint32_t i = 0; i < (p > length ? length : p); i += 1) {
        printf("%13" PRIu32 " ", Array[i]);
        if ((i + 1) % 5 == 0) {
            printf("\n");
        }
    }
    return 0;
}

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        " A collection of comparison-based sorting algorithms.\n"
        "\n"
        "USAGE\n"
        " %s [-Hasbhqn:p:r:] [-n length] [-p elements] [-r seed]\n"
        "\n"
        "OPTIONS\n"
        " -H  Display program help and usage.\n"
        " -a  Enable all sorts.\n"
        " -s  Enable Shell Sort.\n"
        " -b  Enable Batcher Sort.\n"
        " -h  Enable Heap Sort.\n"
        " -q  Enable Quick Sort.\n"
        " -n  length Specify number of array elements (default: 100).\n"
        " -p  elements Specify number of elements to print (default: 100).\n"
        " -r  seed Specify random seed (default: 13371453).\n",
        exec);
}

int main(int argc, char **argv) {
    int opt = 0;
    Set set;
    set = set_empty();
    Stats stats = { 0, 0 };
    Stats *stats_ptr = &stats;
    //uint32_t a, h, b, s, q;
    uint32_t n = 100;
    uint32_t r = 13371453;
    uint32_t p = 100;
    uint8_t go = 1;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'a': set = set_insert(set, 0); break;
        case 'h': set = set_insert(set, 1); break;
        case 'b': set = set_insert(set, 2); break;
        case 's': set = set_insert(set, 3); break;
        case 'q': set = set_insert(set, 4); break;
        case 'r': r = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 'n': n = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 'p': p = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 'H': usage(argv[0]); break;
        }
    }

    uint32_t *Array = (uint32_t *) calloc(n, sizeof(uint32_t));
    fill_random(Array, n, r);
    //print_array(Array, n, p);
    if (set_member(set, 0) == 1) {
        for (uint32_t i = 1; i < 5; i++) {
            set = set_remove(set, i);
        }
        go = 0;
        shell_sort(stats_ptr, Array, n);
        printf("Shell Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
            stats_ptr->compares);
        print_array(Array, n, p);
        reset(stats_ptr);
        free(Array);

        Array = (uint32_t *) calloc(n, sizeof(uint32_t));
        fill_random(Array, n, r);
        batcher_sort(stats_ptr, Array, n);
        printf("Batcher Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
            stats_ptr->compares);
        print_array(Array, n, p);
        reset(stats_ptr);
        free(Array);

        Array = (uint32_t *) calloc(n, sizeof(uint32_t));
        fill_random(Array, n, r);
        heap_sort(stats_ptr, Array, n);
        printf("Heap Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
            stats_ptr->compares);
        print_array(Array, n, p);
        reset(stats_ptr);
        free(Array);

        Array = (uint32_t *) calloc(n, sizeof(uint32_t));
        fill_random(Array, n, r);
        quick_sort(stats_ptr, Array, n);
        printf("Quick Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
            stats_ptr->compares);
        print_array(Array, n, p);
        reset(stats_ptr);
        free(Array);
    }
    //printf("%d\n",set);
    uint8_t bit_move = 0;
    uint8_t check_opt = 1;
    while (bit_move < 5 && go) {
        check_opt = check_opt << 1;
        bit_move += 1;
        if (bit_move == 1 && set_member(set, 1)) {
            Array = (uint32_t *) calloc(n, sizeof(uint32_t));
            fill_random(Array, n, r);
            heap_sort(stats_ptr, Array, n);
            printf("Heap Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
                stats_ptr->compares);
            reset(stats_ptr);
            print_array(Array, n, p);
            free(Array);
        } else if (bit_move == 2 && set_member(set, 2)) {
            Array = (uint32_t *) calloc(n, sizeof(uint32_t));
            fill_random(Array, n, r);
            batcher_sort(stats_ptr, Array, n);
            printf("Batcher Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
                stats_ptr->compares);
            reset(stats_ptr);
            print_array(Array, n, p);
            free(Array);
        } else if (bit_move == 3 && set_member(set, 3)) {
            Array = (uint32_t *) calloc(n, sizeof(uint32_t));
            fill_random(Array, n, r);
            shell_sort(stats_ptr, Array, n);
            printf("Shell Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
                stats_ptr->compares);
            reset(stats_ptr);
            print_array(Array, n, p);
            free(Array);
        } else if (bit_move == 3 && set_member(set, 4)) {
            Array = (uint32_t *) calloc(n, sizeof(uint32_t));
            fill_random(Array, n, r);
            quick_sort(stats_ptr, Array, n);
            printf("Quick Sort, %d elements, %ld moves, %ld compares\n", n, stats_ptr->moves,
                stats_ptr->compares);
            reset(stats_ptr);
            print_array(Array, n, p);
            free(Array);
        }
    }

    return 0;
}
