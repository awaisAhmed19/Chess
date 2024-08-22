#include "defs.h"
#include "stdio.h"

char *PrSq(const int sq)
{
    static char SqStr[3];

    int file = FilesBrd[sq];
    int rank = RanksBrd[sq];

    // printf("%d %d", file, rank);
    sprintf(SqStr, "%c%c", ('a' + file), ('1' + rank));

    return SqStr;
}

char *PrMove(const int move)
{
    static char MvStr[6];

    int ff = FilesBrd[FROMSQ(move)];
    int rf = RanksBrd[FROMSQ(move)];
    int ft = FilesBrd[TOSQ(move)];
    int rt = RanksBrd[TOSQ(move)];

    int promoted = PROMOTED(move);
    // printf("ff:%d rf:%d  ft:%d rt:%d\n", ff, rf, ft, rt);
    if (promoted)
    {
        char pChar = 'q';
        if (isKn(promoted))
        {
            pChar = 'n';
        }
        else if (isRQ(promoted) && !isBQ(promoted))
        {
            pChar = 'r';
        }
        else if (!isRQ(promoted) && isBQ(promoted))
        {
            pChar = 'b';
        }
        sprintf(MvStr, "%c%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt), pChar);
    }
    else
    {

        sprintf(MvStr, "%c%c%c%c", ('a' + ff), ('1' + rf), ('a' + ft), ('1' + rt));
    }
    return MvStr;
}

int ParseMove(char *ptrChar, S_BOARD *pos)
{
    if (ptrChar[1] > '8' || ptrChar[1] < '1')
        return NOMOVE;
    if (ptrChar[3] > '8' || ptrChar[3] < '1')
        return NOMOVE;
    if (ptrChar[0] > 'h' || ptrChar[0] < 'a')
        return NOMOVE;
    if (ptrChar[2] > 'h' || ptrChar[2] < 'a')
        return NOMOVE;

    int from = FR2SQ(ptrChar[0] - 'a', ptrChar[1] - '1');
    int to = FR2SQ(ptrChar[2] - 'a', ptrChar[3] - '1');

    ASSERT(SqOnBoard(from) && SqOnBoard(to));

    S_MOVELIST list[1];

    GenerateAllMoves(pos, list);
    int move = 0;
    int moveNum = 0;
    int promoPce = EMPTY;

    for (moveNum = 0; moveNum < list->count; ++moveNum)
    {
        move = list->moves[moveNum].move;
        if (FROMSQ(move) == from && TOSQ(move) == to)
        {
            promoPce = PROMOTED(move);
            if (promoPce != EMPTY)
            {
                if (isRQ(promoPce) && !isBQ(promoPce) && ptrChar[4] == 'r')
                {
                    return move;
                }
                else if (!isRQ(promoPce) && isBQ(promoPce) && ptrChar[4] == 'b')
                {
                    return move;
                }
                else if (isRQ(promoPce) && isBQ(promoPce) && ptrChar[4] == 'q')
                {
                    return move;
                }
                else if (isKn(promoPce) && ptrChar[4] == 'n')
                {
                    return move;
                }
                continue;
            }
            return move;
        }
    }
    return NOMOVE;
}

void PrintMoveList(const S_MOVELIST *list)
{
    int index = 0;
    int score = 0;
    int move = 0;

    printf("Move list :\n", list->count);

    for (index = 0; index < list->count; ++index)
    {
        move = list->moves[index].move;
        // printf(" %d\n", move);
        score = list->moves[index].score;
        printf("Move: %d > %s (score: %d)\n", index + 1, PrMove(move), score);
    }
    printf("MovesList total %d moves\n", list->count);
}