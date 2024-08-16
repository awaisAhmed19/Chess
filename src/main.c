#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include "locale.h"
#define FEN1 "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"
#define FEN3 "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"
#define PERFT "Q7/1P6/4P3/4R3/4P3/8/8/8 w - - 0 1 "

int main(void)
{
    AllInit();

    S_BOARD board[1];
    ParseFen(FEN1, board);
    // showAttacks(WHITE, board);
    // showAttacks(BLACK, board);
    // showAttacks(BOTH, board);

    // UpdateListMaterial(board);
    PrintBoard(board);
    // ASSERT(CheckBoard(board));
    return 0;
}