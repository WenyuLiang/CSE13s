#include "universe.h"

#include <inttypes.h>
#include <stdlib.h>
#define IN_BOUND(x, y, row, col) (x >= 0 && x < row && y >= 0 && y < col)

uint32_t toroidalIndex(uint32_t index, uint32_t bound) {
    return index % bound;
}

struct Universe {
    uint32_t rows;
    uint32_t cols;
    uint32_t **grid;
    bool toroidal;
};

Universe *uv_create(uint32_t rows, uint32_t cols, bool toroidal) {
    uint32_t **grid = (uint32_t **) calloc(rows, sizeof(uint32_t *));
    for (uint32_t r = 0; r < rows; r += 1) {
        grid[r] = (uint32_t *) calloc(cols, sizeof(uint32_t));
    }
    Universe *u = (Universe *) malloc(sizeof(Universe));
    u->rows = rows;
    u->cols = cols;
    u->grid = grid;
    u->toroidal = toroidal;
    return u;
}

void uv_delete(Universe *u) {
    for (uint32_t r = 0; r < u->rows; ++r) {
        free(u->grid[r]);
        u->grid[r] = NULL;
    }
    free(u->grid);
    u->grid = NULL;
    free(u);
    u = NULL;
}

uint32_t uv_rows(Universe *u) {
    return u->rows;
}

uint32_t uv_cols(Universe *u) {
    return u->cols;
}

void uv_live_cell(Universe *u, uint32_t r, uint32_t c) {
    if (!u->toroidal && IN_BOUND(r, c, u->rows, u->cols)) {
        u->grid[r][c] = true;
    } else if (u->toroidal) {
        u->grid[toroidalIndex(r, u->rows)][toroidalIndex(c, u->cols)] = true;
    }
}

void uv_dead_cell(Universe *u, uint32_t r, uint32_t c) {
    if (!u->toroidal && IN_BOUND(r, c, u->rows, u->cols)) {
        u->grid[r][c] = false;
    } else if (u->toroidal) {
        u->grid[toroidalIndex(r, u->rows)][toroidalIndex(c, u->cols)] = false;
    }
}

bool uv_get_cell(Universe *u, uint32_t r, uint32_t c) {
    if (u->toroidal) {
        if (u->grid[toroidalIndex(r, u->rows)][toroidalIndex(c, u->cols)]) {
            return true;
        } else {
            return false;
        }
    } else {
        if (IN_BOUND(r, c, u->rows, u->cols)) {
            if (u->grid[r][c]) {
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }
}

bool uv_populate(Universe *u, FILE *infile) {
    uint16_t x, y;
    char buffer[20];
    while (!feof(infile)) {
        if (fgets(buffer, sizeof(buffer), infile) != NULL) {
            if (sscanf(buffer, "%" SCNu16 " %" SCNu16 " ", &x, &y) == 2) {
                if (u->toroidal) {
                    u->grid[toroidalIndex(x, u->rows)][toroidalIndex(y, u->cols)] = true;
                } else {
                    if (IN_BOUND(x, y, u->rows, u->cols)) {
                        u->grid[x][y] = true;
                    } else {
                        return false;
                    }
                }
            } else {
                fprintf(stderr, "Invalid file format\n");
                return false;
            }
        }
        if (ferror(infile)) {
            perror("Error!!:");
            return false;
        }
    }
    fclose(infile);
    return true;
}

uint32_t uv_census(Universe *u, uint32_t r, uint32_t c) {
    uint32_t num_t = 0;
    uint32_t num_f = 0;
    if (u->toroidal) {
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i == 0 && j == 0) {
                    continue;
                } else {
                    if (u->grid[toroidalIndex(r + i + u->rows, u->rows)]
                               [toroidalIndex(c + j + u->cols, u->cols)]) {
                        num_t += 1;
                    }
                }
            }
        }
        return num_t;
    } else {
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i == 0 && j == 0) {
                    continue;
                } else {
                    if (IN_BOUND(r + i, c + j, u->rows, u->cols) && u->grid[r + i][c + j]) {
                        num_f += 1;
                    }
                }
            }
        }
    }
    return num_f;
}

void uv_print(Universe *u, FILE *outfile) {
    if (outfile == stdout) {
        for (uint32_t i = 0; i < u->rows; ++i) {
            for (uint32_t j = 0; j < u->cols; ++j) {
                printf("%s", u->grid[i][j] == true ? "o" : ".");
            }
            printf("\n");
        }
    } else {
        for (uint32_t i = 0; i < u->rows; ++i) {
            for (uint32_t j = 0; j < u->cols; ++j) {
                fprintf(outfile, "%s", u->grid[i][j] == true ? "o" : ".");
            }
            fprintf(outfile, "\n");
        }
    }
}
