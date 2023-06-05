#include "randstate.h"

#include <gmp.h>
#include <stdint.h>
#include <stdlib.h>

//define state
gmp_randstate_t state;

//initilize the seed
void randstate_init(uint64_t seed) {
    srandom(seed);
    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}

//clear state
void randstate_clear(void) {
    gmp_randclear(state);
}
