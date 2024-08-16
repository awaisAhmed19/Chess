#include "defs.h"

const int KnDir[8] = {-8, -19, -21, -12, 8, 19, 21, 12};
const int RkDir[4] = {-1, -10, 1, 10};
const int BiDir[4] = {-9, -11, 11, 9};
const int KiDir[8] = {-1, -10, 1, 10, -9, -11, 11, 9};

// remember we use the 120 sq board

int sqAttacked(const int sq, const int side, const S_BOARD *pos)
{
    int pce, index, t_sq, dir;

    if (side == WHITE)
    {
        if (pos->pieces[sq - 11] == wP || pos->pieces[sq - 9] == wP) //(Right||left)
        {
            return TRUE;
        }
    }
    else
    {
        if (pos->pieces[sq + 11] == bP || pos->pieces[sq + 9] == bP)
        {
            return TRUE;
        }
    }

    // Check for knight attacks
    for (index = 0; index < 8; ++index)
    {
        pce = pos->pieces[sq + KnDir[index]];
        if (pce != OFFBOARD && isKn(pce) && PieceCol[pce] == side)
        {
            return TRUE;
        }
    }
    // Rooks
    for (index = 0; index < 4; ++index)
    {
        dir = RkDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while (pce != OFFBOARD)
        {
            if (pce != EMPTY)
            {
                if (isRQ(pce) && PieceCol[pce] == side)
                {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }

    // Bishop or Queen

    for (index = 0; index < 4; ++index)
    {
        dir = BiDir[index];
        t_sq = sq + dir;
        pce = pos->pieces[t_sq];
        while (pce != OFFBOARD)
        {
            if (pce != EMPTY)
            {
                if (isBQ(pce) && PieceCol[pce] == side)
                {
                    return TRUE;
                }
                break;
            }
            t_sq += dir;
            pce = pos->pieces[t_sq];
        }
    }

    for (index = 0; index < 8; ++index)
    {
        pce = pos->pieces[sq + KiDir[index]];
        if (pce != OFFBOARD && isKi(pce) && PieceCol[pce] == side)
        {
            return TRUE;
        }
    }

    return FALSE;
}