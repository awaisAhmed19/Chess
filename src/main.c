#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include "locale.h"

int main(void)
{
    AllInit();

    S_BOARD board[1];
    int move = 0;
    int from = A2;
    int to = H7;
    int cap = wR;
    int prom = wR;

    move = ((from) | (to << 7) | (cap << 14) | (prom << 20));
    printf("decimal of the  move %d and hex %X\n", move, move);
    printf("from: %d \nto: %d \ncap: %d \nprom: %d\n", FROMSQ(move), TOSQ(move), CAPTURED(move), PROMOTED(move));
    printf("algebraic from %s\n", PrSq(from));
    printf("algebraic to %s\n", PrSq(to));
    printf("algebraic move %s\n", PrMove(move));
    return 0;
}