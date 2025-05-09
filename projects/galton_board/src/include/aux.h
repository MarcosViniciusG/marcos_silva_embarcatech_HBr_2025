#include "stdlib.h"     
#include "time.h"

/*
    Initializes random function with
    a random seed (time(NULL))
*/
void init_rand();

/*
    Random function that returns
    a number between 0 and mod-1 
*/
int rand_mod(int mod);

/*
    Integer division
    that returns biggest x <= a / b 
*/
int intceil(int a, int b);