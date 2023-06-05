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

// helper function to calculate the bit length
static uint8_t bitlen(uint16_t code) {
    uint8_t bitlen = 0;
    while (code) {
        bitlen++;
        code >>= 1; // when it becomes 0
    }
    return bitlen;
}

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        " Decompresses files using the LZ78 decompression algorithm.\n"
        " Decompressed files are decompressed with the corresponding decoder.\n"
        "\n"
        "USAGE\n"
        " %s [-vh] [-i input] [-o output]\n"
        "\n"
        "OPTIONS\n"
        " -v Display decompression statistics\n"
        " -i input Specify input to decompress (stdin by default)\n"
        " -o output Specify output of decompressed input (stdout by default)\n"
        " -h Display program help and usage\n",
        exec);
}

int main(int argc, char **argv) {
    bool is_stdin = true;
    bool is_stdout = true;
    bool verbose = false;
    char infile[1024]; //save file name
    char outfile[1024]; //save file name
    struct stat instats; ////save infile stats
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
        if (infild == -1) {
            perror("Error opening input file");
            exit(EXIT_FAILURE);
        }
    }
    fstat(infild, &instats);
    read_header(infild, &header); // If success, magic number=0xBAADBAAC
    if (is_stdout) {
        outfild = 1;
    } else {
        outfild = open(outfile, O_WRONLY | O_CREAT, header.protection);
        if (outfild == -1) {
            perror("Error opening output file");
            exit(EXIT_FAILURE);
        }
    }

    //fchmod(outfild, header.protection);
    WordTable *table = wt_create();
    uint8_t curr_sym = 0;
    uint16_t curr_code = 0;
    uint16_t next_code = START_CODE;
    /* if(read_pair(infild, &curr_code, &curr_sym, bitlen(next_code))){
      printf("yes\n");
    }*/
    while (read_pair(infild, &curr_code, &curr_sym, bitlen(next_code))) {
        table[next_code] = word_append_sym(table[curr_code], curr_sym);
        write_word(outfild, table[next_code]);
        next_code += 1;
        if (next_code == MAX_CODE) {
            wt_reset(table);
            next_code = START_CODE;
        }
    }
    //printf("curr_code: %d, curr_sym:%d, next_code:%d\n", curr_code, curr_sym, next_code);
    flush_words(outfild);
    if (verbose) { // print the verbose information
        printf("Compressed file size: %ld bytes\n", (total_bits + 7) / 8);
        printf("Uncompressed file size: %ld bytes\n", total_syms);
        printf("Space saving: %.2f%%\n",
            100.0 * (1.0 - ((double) ((total_bits + 7) / 8) / (double) total_syms)));
    }
    close(infild);
    close(outfild);
}
