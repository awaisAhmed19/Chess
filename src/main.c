#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include "locale.h"
#define TEST "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "

int main(void)
{
    AllInit();
    // int d = 3;
    S_BOARD board[1];
    S_MOVELIST list[1];
    FENDATA data[1];
    getFen("C:\\Users\\awais\\Documents\\Chess\\src\\test.epd", data);
    accuracyTest(data, board);
    // ParseFen(TEST, board);
    // PrintBoard(board);

    // // Perft(d, board);
    // PerftTest(3, board);
    return 0;
}