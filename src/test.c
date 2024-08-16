#include "defs.h"
#include "stdio.h"

void showAttacks(const int side, S_BOARD *pos)
{
    for (int rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (int file = FILE_A; file <= FILE_H; ++file)
        {
            int sq = FR2SQ(file, rank);
            if (sqAttacked(sq, side, pos) == TRUE)
            {
                printf(" x ");
            }
            else
            {
                printf(" - ");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}
