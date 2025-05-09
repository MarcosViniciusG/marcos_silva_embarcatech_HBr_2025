#include "aux.h"

void init_rand() {
    srand(time(NULL));
}

int rand_mod(int mod) {
    int r = rand() % mod;
    return r;
}

int intceil(int a, int b) {
    return (a / b) + (a % b != 0);
}