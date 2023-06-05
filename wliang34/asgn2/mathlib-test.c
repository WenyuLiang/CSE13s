#include "mathlib.h"

#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define OPTIONS "aebmrvn:sh"

double pi = M_PI;
double E = M_E;
double epsilon = EPSILON;
double PI;
double ee;
double diff;
double number = 4.0;
double k_ = 1.0;

void usage(char *exec) {
    fprintf(stderr,
        "SYNOPSIS\n"
        "   Approximate Pi and e.\n"
        "\n"
        "USAGE\n"
        "   %s [-aebmrvn:sh] \n"
        "\n"
        "OPTIONS\n"
        "   -h         display program help and usage.\n"
        "   -a	 	run all tests.\n"
        "   -e         Runs e approximation test.\n"
        "   -b         Runs Bailey-Borwein-Plouffe π test.\n"
        "   -m         Runs Madhava π test.\n"
        "   -r         Runs Euler sequence π test.\n"
        "   -v         Runs Viète sequence π test.\n"
        "   -n         Runs Newton-Raphson square root test.\n"
        "   -s         Enable printing of statistics to see computed terms and factors for each "
        "tested function.\n",
        exec);
}

int main(int argc, char **argv) {
    int opt = 0;
    int s = 0;

    for (int i = 0; i < argc; i += 1) {
        if (strcmp(argv[i], "-s") == 0) {
            s = 1;
            break;
        }
    }

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'a':

            diff = 1.0;
            while (diff > epsilon) {
                PI = pi_bbp();
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_bbp() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi, diff,
                    pi_bbp_terms());
            } else {
                printf("pi_bbp() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }

            diff = 1.0;
            while (diff > epsilon) {
                ee = e();
                diff = absolute(E - ee);
            }
            if (s == 1) {
                printf("e() = %.15f, M_E = %.15f, diff = %.15f, e terms = %d\n", ee, E, diff,
                    e_terms());
            } else {
                printf("e() = %.15f, M_E = %.15f, diff = %.15f\n", ee, E, diff);
            }

            diff = 1.0;
            while (diff > epsilon) {
                PI = pi_madhava();
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_madhava() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi,
                    diff, pi_madhava_terms());
            } else {
                printf("pi_madhava() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }
            diff = 1.0;
            while (k_ > epsilon) {
                PI = pi_euler();
                k_ = 1.0 / (pi_euler_terms() * pi_euler_terms());
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_euler() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi, diff,
                    pi_euler_terms());
            } else {
                printf("pi_euler() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }

            diff = 1.0;
            while (diff > epsilon) {
                PI = pi_viete();
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_viete() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi, diff,
                    pi_viete_factors());
            } else {
                printf("pi_viete() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }

            /*number = (double)strtod(optarg,NULL);
		double result = sqrt_newton(number);
		int iter = sqrt_newton_iters();
		if (s == 1){printf("sqrt_newton(%.4f) = %.15f, terms = %d\n",number, result,iter);}
		else {printf("sqrt_newton(%.4f) = %.15f\n",number, result);}*/
            break;
        case 'b':
            diff = 1.0;
            while (diff > epsilon) {
                PI = pi_bbp();
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_bbp() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi, diff,
                    pi_bbp_terms());
            } else {
                printf("pi_bbp() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }
            break;

        case 'e':
            diff = 1.0;
            while (diff > epsilon) {
                ee = e();
                diff = absolute(E - ee);
            }
            if (s == 1) {
                printf("e() = %.15f, M_E = %.15f, diff = %.15f, e terms = %d\n", ee, E, diff,
                    e_terms());
            } else {
                printf("e() = %.15f, M_E = %.15f, diff = %.15f\n", ee, E, diff);
            }
            break;
        case 'm':
            diff = 1.0;
            while (diff > epsilon) {
                PI = pi_madhava();
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_madhava() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi,
                    diff, pi_madhava_terms());
            } else {
                printf("pi_madhava() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }
            break;
        case 'r':
            diff = 1.0;
            while (k_ > epsilon) {
                PI = pi_euler();
                k_ = 1.0 / (pi_euler_terms() * pi_euler_terms());
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_euler() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi, diff,
                    pi_euler_terms());
            } else {
                printf("pi_euler() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }
            break;
        case 'v':
            diff = 1.0;
            while (diff > epsilon) {
                PI = pi_viete();
                diff = absolute(pi - PI);
            }
            if (s == 1) {
                printf("pi_viete() = %.15f, M_PI = %.15f, diff = %.15f, terms = %d\n", PI, pi, diff,
                    pi_viete_factors());
            } else {
                printf("pi_viete() = %.15f, M_PI = %.15f, diff = %.15f\n", PI, pi, diff);
            }
            break;
        case 'n':;
            number = (double) strtod(optarg, NULL);
            double result = sqrt_newton(number);
            int iter = sqrt_newton_iters();
            if (s == 1) {
                printf("sqrt_newton(%.4f) = %.15f, terms = %d\n", number, result, iter);
            } else {
                printf("sqrt_newton(%.4f) = %.15f\n", number, result);
            }
            break;
        case 's': break;
        case 'h':
        default: usage(argv[0]); break;
        }
    }
}
