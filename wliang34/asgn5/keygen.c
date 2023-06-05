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
#include <time.h>
#define OPTIONS "b:n:i:d:s:vh"

//print help message
void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        " Key Generator Program\n"
        "\n"
        "USAGE\n"
        " %s -b:n:i:d:s:v:h\n"
        "\n"
        "OPTIONS\n"
        " -b {number} Specifies the minimum bits needed for the public modulus n [default: 64].\n"
        " -i {number} Specifies the number of Miller-Rabin iterations for testing primes [default: "
        "50].\n"
        " -n {file} Specifies the public key file [default: ss.pub].\n"
        " -d {file} Specifies the private key file [default: ss.priv].\n"
        " -s {number} Specifies the random seed for the random state initialization [default: "
        "time(NULL)].\n"
        " -v Enables verbose output.\n"
        " -h Displays program synopsis and usage.\n",
        exec);
}

int main(int argc, char **argv) {
    int opt = 0;
    char *pub_key = "ss.pub";
    char *priv_key = "ss.priv";
    char *username = getenv("USER");
    bool verbose = false;
    //set default numbers for nbits and niters
    uint32_t nbits = 256;
    uint32_t niters = 100;
    //set default seed
    uint64_t seed = time(NULL);
    //parse the arguments
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': nbits = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 's': seed = (uint64_t) strtoul(optarg, NULL, 10); break;
        case 'i': niters = (uint32_t) strtoul(optarg, NULL, 10); break;
        case 'd': strcpy(priv_key, optarg); break;
        case 'n': strcpy(pub_key, optarg); break;
        case 'v': verbose = true; break;
        case 'h': usage(argv[0]); break;
        default:
            usage(argv[0]);
            exit(1);
            break;
        }
    }

    FILE *pub = fopen(pub_key, "w");
    if (!pub) {
        perror(pub_key);
        return EXIT_FAILURE;
    }
    FILE *priv = fopen(priv_key, "w");
    if (!priv) {
        perror(priv_key);
        return EXIT_FAILURE;
    }
    //initialize state;
    randstate_init(seed);

    //make and write public key files
    mpz_t p, q, n;
    mpz_inits(p, q, n, NULL);
    ss_make_pub(p, q, n, nbits, niters);
    ss_write_pub(n, username, pub);
    //make and write private key files
    mpz_t pq, d;
    mpz_inits(pq, d, NULL);
    mpz_mul(pq, p, q);
    ss_make_priv(d, pq, p, q);
    ss_write_priv(pq, d, priv);

    // Set permissions
    fchmod(fileno(priv), 0600);

    //close files
    fclose(pub);
    fclose(priv);

    //print information if -v is given
    if (verbose) {
        mpz_t large, small;
        if (mpz_cmp(p, q)) {
            mpz_set(large, p);
            mpz_set(small, q);
        } else {
            mpz_set(large, q);
            mpz_set(small, p);
        }
        printf("user = %s\n", username);
        gmp_printf("p  (%lld bits) = %Zd\n", mpz_sizeinbase(large, 2), large);
        gmp_printf("q  (%lld bits) = %Zd\n", mpz_sizeinbase(small, 2), small);
        gmp_printf("n  (%lld bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("pq (%lld bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_printf("d  (%lld bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
        mpz_clears(large, small, NULL);
    }

    //clear up
    mpz_clears(p, q, n, NULL);
    mpz_clears(pq, d, NULL);
    randstate_clear();
}
