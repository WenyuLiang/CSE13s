#include "universe.h"

#include <getopt.h>
#include <inttypes.h>
#include <ncurses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define OPTIONS "tsn:i:o:h"
#define DELAY   50000

//-fantasize=true

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "    Conway's Game of Life\n"
        "\n"
        "USAGE\n"
        "    %s -tsn:i:o:h\n"
        "\n"
        "OPTIONS\n"
        "    -t             Create your universe as a toroidal\n"
        "    -s             Silent - do not use animate the evolution\n"
        "    -n {number}    Number of generations [default: 100]\n"
        "    -i {file}      Input file [default: stdin]\n"
        "    -o {file}      Output file [default: stdout]\n"
        "    -h             display program help and usage\n",
        exec);
}

void swap(Universe **a, Universe **b) {
    Universe *temp = *a;
    *a = *b;
    *b = temp;
}

void update(Universe *a, Universe *b) {
    uint32_t rows = uv_rows(a);
    uint32_t cols = uv_cols(b);
    uint32_t census = 0;
    bool live = false;
    for (uint32_t i = 0; i < rows; ++i) {
        for (uint32_t j = 0; j < cols; ++j) {
            live = uv_get_cell(a, i, j);
            census = uv_census(a, i, j);
            if ((live && census == 2) || (live && census == 3)) {
                uv_live_cell(b, i, j);
            } else if (live == false && census == 3) {
                uv_live_cell(b, i, j);
            } else {
                uv_dead_cell(b, i, j);
            }
        }
    }
}

void display(Universe *u) {
    uint32_t rows = uv_rows(u);
    uint32_t cols = uv_cols(u);
    curs_set(FALSE);
    for (uint32_t i = 0; i < rows; ++i) {
        for (uint32_t j = 0; j < cols; ++j) {
            if (uv_get_cell(u, i, j)) {
                mvprintw(i, j, "o");
            }
        }
    }
    refresh();
    usleep(DELAY);
    clear();
}

int main(int argc, char **argv) {
    int opt = 0;
    bool silence = false;
    bool toroidal = false;
    uint16_t rows = 0;
    uint16_t cols = 0;
    char buffer[30];
    char infile[50];
    char outfile[50];
    FILE *input, *output;
    bool is_stdin = true;
    bool is_stdout = true;
    uint32_t generations = 100;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'n': generations = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 's': silence = true; break;
        case 'i':
            is_stdin = false;
            strcpy(infile, optarg);
            break;
        case 'o':
            is_stdout = false;
            strcpy(outfile, optarg);
            break;
        case 't': toroidal = true; break;
        case 'h': usage(argv[0]); break;
        }
    }
    if (is_stdin) {
        input = stdin;
    } else {
        input = fopen(infile, "r");
    }

    if (input == NULL) {
        perror("Error!!:");
        printf("%d\n", 110);
        exit(1);
    } else {
        if (fgets(buffer, sizeof(buffer), input) != NULL) {
            if (sscanf(buffer, "%" SCNu16 " %" SCNu16 " ", &rows, &cols) != 2) {
                fprintf(stderr, "Invalid file format\n");
                exit(1);
            }
        }
    }
    Universe *u1 = uv_create(rows, cols, toroidal);
    Universe *u2 = uv_create(rows, cols, toroidal);

    uv_populate(u1, input);
    //populate u2 as follows:
    for (uint32_t i = 0; i < uv_rows(u1); ++i) {
        for (uint32_t j = 0; j < uv_cols(u1); ++j) {
            if (uv_get_cell(u1, i, j)) {
                uv_live_cell(u2, i, j);
            } else {
                uv_dead_cell(u2, i, j);
            }
        }
    }

    if (!silence) {
        initscr();
    }
    for (uint32_t g = 0; g < generations; ++g) {
        update(u1, u2);
        swap(&u1, &u2);
        if (!silence) {
            display(u1);
        }
    }
    if (!silence) {
        endwin();
    }
    if (is_stdout) {
        output = stdout;
    } else {
        output = fopen(outfile, "w");
    }
    uv_print(u1, output);
    uv_delete(u1);
    uv_delete(u2);
}
