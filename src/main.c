#include <stdio.h>
#include "defs.h"
#include <stdlib.h>
#include "locale.h"
#define TEST "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1 "

int main(void)
{
    AllInit();
    // int d = 3;
    S_BOARD board[1];
    S_MOVELIST list[1];

    char input[6];
    ParseFen(START_FEN, board);
    while (TRUE)
    {
        PrintBoard(board);

        fgets(input, 6, stdin);
        if (input[0] == 'q')
        {
            break;
        }
        else if (input[0] == 't')
        {
            TakeMove(board);
        }
        else
        {
            int move = ParseMove(input, board);
            if (move != NOMOVE)
            {
                MakeMove(board, move);
                if (IsRepetition(board))
                {
                    printf("repeated");
                }
            }
        }
        fflush(stdin);
    }

    return 0;
}