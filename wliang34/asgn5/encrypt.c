#include "numtheory.h"
#include "randstate.h"
#include "ss.h"
#include "sys/stat.h"

#include <getopt.h>
#include <gmp.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OPTIONS "n:i:vo:h"

//print help message
void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        " Encryption Program\n"
        "\n"
        "USAGE\n"
        " %s -i [input_file] -o [output_file] -n [public_key_file] -v -h\n"
        "\n"
        "OPTIONS\n"
        " -i {file} Specifies the input file to encrypt [default: stdin].\n"
        " -o {file} Specifies the output file to encrypt [default: stdout].\n"
        " -n {file} Specifies the file containing the public key [default: ss.pub].\n"
        " -v Enables verbose output.\n"
        " -h Displays program synopsis and usage.\n",
        exec);
}

int main(int argc, char **argv) {
    char pub_key[256] = "ss.pub";
    bool is_stdin = true;
    bool is_stdout = true;
    bool verbose = false;
    //_POSIX_LOGIN_NAME_MAX to avoid segmentation fault
    char username[_POSIX_LOGIN_NAME_MAX];
    //256 should be enough for file name
    char infile[256];
    char outfile[256];
    mpz_t key;
    mpz_init(key);
    int opt = 1;
    FILE *input, *output;
    //parse the augument
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
        case 'n': strcpy(pub_key, optarg); break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }
    FILE *pub = fopen(pub_key, "r");
    if (pub == NULL) {
        perror("Error!!:");
        exit(1);
    } else {
        ss_read_pub(key, username, pub);
    }
    if (verbose) { //if -v, print following information
        printf("user = %s\n", username);
        gmp_printf("n (%lld bits) = %Zd\n", mpz_sizeinbase(key, 2), key);
    }
    //check file open successfully
    if (is_stdin) {
        input = stdin;
    } else {
        input = fopen(infile, "r");
    }
    if (is_stdout) {
        output = stdout;
    } else {
        output = fopen(outfile, "w");
    }
    if (input == NULL) {
        perror("Input fail!!:");
        exit(1);
    }
    if (output == NULL) {
        perror("Output fail!!:");
        exit(1);
    }
    ss_encrypt_file(input, output, key);
    //clear up and close files
    mpz_clear(key);
    fclose(pub);
    fclose(input);
    fclose(output);
}
