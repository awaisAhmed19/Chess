#include "defs.h"
#include "stdio.h"

long leafNodes;

void Perft(int depth, S_BOARD *pos)
{
    ASSERT(CheckBoard(pos));

    if (depth == 0)
    {
        leafNodes++;
        return;
    }

    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int MoveNum = 0;
    for (MoveNum = 0; MoveNum < list->count; ++MoveNum)
    {
        if (!MakeMove(pos, list->moves[MoveNum].move))
        {
            continue;
        }
        Perft(depth - 1, pos);
        TakeMove(pos);
    }
    return;
}

long PerftTest(int depth, S_BOARD *pos)
{
    ASSERT(CheckBoard(pos));
    // PrintBoard(pos);
    // printf("\n staring test to depth: %d", depth);
    leafNodes = 0;
    // int start = getTimeMs();
    S_MOVELIST list[1];
    GenerateAllMoves(pos, list);

    int move;
    int MoveNum = 0;
    for (MoveNum = 0; MoveNum < list->count; ++MoveNum)
    {
        move = list->moves[MoveNum].move;
        if (!MakeMove(pos, move))
        {
            continue;
        }
        long cumNodes = leafNodes;
        Perft(depth - 1, pos);
        TakeMove(pos);
        long oldNodes = leafNodes - cumNodes;
        // printf("\nmove %d : %s : %ld ", MoveNum + 1, PrMove(move), oldNodes);
    }
    // printf("\n test Complete: %ld nodes visited", leafNodes);
    return leafNodes;
}