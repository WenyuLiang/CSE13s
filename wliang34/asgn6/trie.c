#include <stdlib.h>
#include "trie.h"
#include "code.h"

TrieNode *trie_node_create(uint16_t index) {
    TrieNode *node = (TrieNode *) malloc(sizeof(TrieNode)); //allocate memory
    node->code = index;
    for (uint16_t i = 0; i < ALPHABET; ++i) {
        node->children[i] = NULL; // initialize to null pointet
    }
    return node;
}

void trie_node_delete(TrieNode *n) {
    // Avoid double free
    if (n == NULL) {
        return;
    }
    free(n);
    n = NULL;
}

TrieNode *trie_create(void) {
    // Initialization
    TrieNode *node = trie_node_create(EMPTY_CODE);
    return node == NULL ? NULL : node;
}

void trie_reset(TrieNode *root) {
    for (uint16_t i = 0; i < ALPHABET; i++) {
        trie_delete(root->children[i]);
        root->children[i] = NULL;
    }
}

void trie_delete(TrieNode *n) {
    // Avoid double free
    if (n == NULL) {
        return;
    }
    for (uint16_t i = 0; i < ALPHABET; i++) {
        trie_delete(n->children[i]);
        n->children[i] = NULL;
    }
    trie_node_delete(n);
}

TrieNode *trie_step(TrieNode *n, uint8_t sym) {
    return n->children[sym] != NULL ? n->children[sym] : NULL;
}
