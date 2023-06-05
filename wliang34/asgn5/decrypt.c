#include "numtheory.h"
#include "randstate.h"
#include "ss.h"
#include "sys/stat.h"

#include <getopt.h>
#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OPTIONS "n:i:vo:h"

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        " Decryption Program\n"
        "\n"
        "USAGE\n"
        " %s -i [input_file] -o [output_file] -n [private_key_file] -v -h\n"
        "\n"
        "OPTIONS\n"
        " -i {file} Specifies the input file to decrypt [default: stdin].\n"
        " -o {file} Specifies the output file to decrypt [default: stdout].\n"
        " -n {file} Specifies the file containing the privlic key [default: ss.priv].\n"
        " -v Enables verbose output.\n"
        " -h Displays program synopsis and usage.\n",
        exec);
}

int main(int argc, char **argv) {
    char priv_key[1024] = "ss.priv";
    bool is_stdin = true;
    bool is_stdout = true;
    bool verbose = false;
    char infile[1024];
    char outfile[1024];
    //initialization
    mpz_t key;
    mpz_init(key);
    mpz_t modulus;
    mpz_init(modulus);
    int opt = 1;
    FILE *input, *output;
    //parse the auguments
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
        case 'n': strcpy(priv_key, optarg); break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }
    FILE *priv = fopen(priv_key, "r");
    if (priv == NULL) { //read priv_key fail, exit
        perror("Error!!:");
        exit(1);
    } else {
        ss_read_priv(modulus, key, priv);
    }
    if (verbose) {
        gmp_printf("pq (%lld bits) = %Zd\n", mpz_sizeinbase(modulus, 2),
            modulus); //mpz_sizeinbase calculate nbits
        gmp_printf(
            "d (%lld bits) = %Zd\n", mpz_sizeinbase(key, 2), key); //mpz_sizeinbase calculate nbits
    }
    if (is_stdin) { //check if input file is given
        input = stdin;
    } else {
        input = fopen(infile, "r");
    }
    if (is_stdout) { //check if output file is given
        output = stdout;
    } else {
        output = fopen(outfile, "w");
    }
    if (input == NULL) { //read input fail, exit
        perror("Input fail!!:");
        exit(1);
    }
    if (output == NULL) { //read output fail, exit
        perror("Output fail!!:");
        exit(1);
    }
    //decrypt given file
    ss_decrypt_file(input, output, key, modulus);
    //clear up and close files
    mpz_clear(key);
    mpz_clear(modulus);
    fclose(priv);
    fclose(input);
    fclose(output);
}
