#include "io.h"
#include "code.h"
#include "word.h"
#include <stdlib.h>
#include "trie.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define OPTIONS "i:vo:h"

// helper function to calculate the bitlength
static uint8_t bitlen(uint16_t code) {
    uint8_t bitlen = 0;
    while (code) {
        bitlen++;
        code >>= 1;
    }
    return bitlen;
}

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        " Compresses files using the LZ78 compression algorithm.\n"
        " Compressed files are decompressed with the corresponding decoder.\n"
        "\n"
        "USAGE\n"
        " %s [-vh] [-i input] [-o output]\n"
        "\n"
        "OPTIONS\n"
        " -v Display compression statistics\n"
        " -i input Specify input to compress (stdin by default)\n"
        " -o output Specify output of compressed input (stdout by default)\n"
        " -h Display program help and usage\n",
        exec);
}

int main(int argc, char **argv) {
    bool is_stdin = true;
    bool is_stdout = true;
    bool verbose = false;
    char infile[1024]; //save file name
    char outfile[1024]; //save file name
    struct stat instats, outstats; // file stats struct
    FileHeader header;
    int opt = 1;
    int help = 0;
    int infild, outfild; // input and output are both file descriptors
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i':
            is_stdin = false;
            strcpy(infile, optarg);
            break;
        case 'o':
            is_stdout = false;
            strcpy(outfile, optarg);
            break;
        case 'v': verbose = true; break;
        case 'h':
            usage(argv[0]);
            help = 1;
            break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }
    if (is_stdin) { //check if input file is given
        infild = open("stdin", O_RDONLY);
        if (infild == -1 && !help) {
            fprintf(stderr, "Error reading from standard input\n");
            exit(EXIT_FAILURE);
        } else {
            close(infild);
            return 0;
        }
    } else {
        infild = open(infile, O_RDONLY);
        if (infild == -1) { //error handling
            perror("Error opening input file");
            exit(EXIT_FAILURE);
        }
    }
    fstat(infild, &instats);
    header.magic = 0xBAADBAAC; // set magic number
    header.protection = instats.st_mode; // protection
    write_header(infild, &header);
    if (is_stdout) {
        outfild = 1;
    } else {
        outfild = open(outfile, O_WRONLY | O_CREAT, instats.st_mode);
        if (outfild == -1) { //error handling
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }
    }

    fchmod(outfild, header.protection); // keep access the same as infile
    write_header(outfild, &header);
    TrieNode *root = trie_create();
    TrieNode *curr_node = root;
    uint16_t next_code = START_CODE;
    TrieNode *prev_node, *next_node;
    uint8_t prev_sym, curr_sym;
    while (read_sym(infild, &curr_sym)) { // compression
        next_node = trie_step(curr_node, curr_sym);
        if (next_node) {
            prev_node = curr_node;
            curr_node = next_node;
        } else {
            write_pair(outfild, curr_node->code, curr_sym, bitlen(next_code));
            curr_node->children[curr_sym] = trie_node_create(next_code);
            curr_node = root;
            next_code += 1;
        }
        if (next_code == MAX_CODE) {
            trie_reset(root);
            curr_node = root;
            next_code = START_CODE;
        }
        prev_sym = curr_sym;
    }
    if (curr_node != root) {
        write_pair(outfild, prev_node->code, prev_sym, bitlen(next_code));
        next_code = (next_code + 1) % MAX_CODE;
    }
    write_pair(outfild, STOP_CODE, 0, bitlen(next_code));
    flush_pairs(outfild); // if buffer not full
    fstat(outfild, &outstats);
    if (verbose) {
        printf("Compressed file size: %ld bytes\n", total_bits / 8 + 1);
        printf("Uncompressed file size: %ld bytes\n", total_syms);
        printf("Space saving: %.2f%%\n",
            100.0 * (1.0 - ((double) (total_bits / 8 + 1) / (double) total_syms)));
    }
    close(infild);
    close(outfild);
}
