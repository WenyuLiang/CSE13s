#include <stdio.h>
#include <stdlib.h>
#include "word.h"
#include <inttypes.h>
#include <stdint.h>
#include "code.h"

Word *word_create(uint8_t *syms, uint32_t len) {
    // Allocate momory for struct
    Word *word = (Word *) malloc(sizeof(Word));
    // Fail-> ruturn NULL
    if (word == NULL) {
        return NULL;
    }
    // Allocate momory for syms
    word->syms = (uint8_t *) calloc(len, sizeof(uint8_t));
    // Fail-> ruturn NULL
    if (word->syms == NULL) {
        return NULL;
    }
    // Asign values
    for (uint32_t i = 0; i < len; i++) {
        word->syms[i] = syms[i];
    }
    word->len = len;
    return word;
}

Word *word_append_sym(Word *w, uint8_t sym) {
    // create new one and increase the size
    Word *word = word_create(w->syms, w->len + 1);
    word->syms[w->len] = sym;
    return word;
}

void word_delete(Word *w) {
    if (w) // Early null check
    {
        free(w->syms);
        w->syms = NULL;
        free(w);
        w = NULL;
    }
}

WordTable *wt_create(void) {
    WordTable *wt = (WordTable *) calloc(MAX_CODE, sizeof(Word *));
    Word *empty_word = malloc(sizeof(Word));
    empty_word->syms = (uint8_t *) ""; // empty string
    empty_word->len = 0; // lenth 0
    wt[EMPTY_CODE] = empty_word;
    return wt;
}

void wt_reset(WordTable *wt) {
    wt[0] = NULL;
    for (uint32_t i = EMPTY_CODE + 1; i < MAX_CODE; i++) {
        wt[i] = NULL;
    }
}

void wt_delete(WordTable *wt) {
    free(wt);
    wt = NULL;
}
