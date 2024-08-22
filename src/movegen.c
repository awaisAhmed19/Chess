#include "stdio.h"
#include "defs.h"

#define MOVE(f, t, ca, prom, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((prom) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)

const int LoopSlideindex[2] = {0, 4};
const int LoopSlidePce[8] = {
    wB, wR, wQ, 0, bB, bR, bQ, 0};

const int LoopNonSlideindex[2] = {0, 3};
const int LoopNonSlidePce[8] = {
    wN, wK, 0, bN, bK, 0};

const int PceDir[13][8] = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12},
    {-9, -11, 11, 9, 0, 0, 0, 0},
    {-1, -10, 1, 10, 0, 0, 0, 0},
    {-1, -10, 1, 10, -9, -11, 11, 9},
    {-1, -10, 1, 10, -9, -11, 11, 9},
    {0, 0, 0, 0, 0, 0, 0},
    {-8, -19, -21, -12, 8, 19, 21, 12},
    {-9, -11, 11, 9, 0, 0, 0, 0},
    {-1, -10, 1, 10, 0, 0, 0, 0},
    {-1, -10, 1, 10, -9, -11, 11, 9},
    {-1, -10, 1, 10, -9, -11, 11, 9}};

const int NumDir[13] = {
    0, 0, 8, 4, 4, 8, 8, 0, 8, 4, 4, 8, 8};

static void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

static void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));

    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

static void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));
    ASSERT((RanksBrd[TOSQ(move)] == RANK_6 && pos->side == WHITE) || (RanksBrd[TOSQ(move)] == RANK_3 && pos->side == BLACK));
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

static void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    ASSERT(PieceValidEmpty(cap));
    if (RanksBrd[from] == RANK_7)
    {
        AddCaptureMove(pos, MOVE(from, to, cap, wQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wB, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, wN, 0), list);
    }
    else
    {
        AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}

static void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));

    if (RanksBrd[from] == RANK_2)
    {
        AddCaptureMove(pos, MOVE(from, to, cap, bQ, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bR, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bB, 0), list);
        AddCaptureMove(pos, MOVE(from, to, cap, bN, 0), list);
    }
    else
    {
        AddCaptureMove(pos, MOVE(from, to, cap, EMPTY, 0), list);
    }
}

static void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list)
{

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    if (RanksBrd[from] == RANK_7)
    {
        AddQuietMove(pos, MOVE(from, to, EMPTY, wQ, 0), list);
        AddQuietMove(pos, MOVE(from, to, EMPTY, wR, 0), list);
        AddQuietMove(pos, MOVE(from, to, EMPTY, wB, 0), list);
        AddQuietMove(pos, MOVE(from, to, EMPTY, wN, 0), list);
    }
    else
    {
        AddQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

static void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list)
{

    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    if (RanksBrd[from] == RANK_2)
    {
        AddQuietMove(pos, MOVE(from, to, EMPTY, bQ, 0), list);
        AddQuietMove(pos, MOVE(from, to, EMPTY, bR, 0), list);
        AddQuietMove(pos, MOVE(from, to, EMPTY, bB, 0), list);
        AddQuietMove(pos, MOVE(from, to, EMPTY, bN, 0), list);
    }
    else
    {
        AddQuietMove(pos, MOVE(from, to, EMPTY, EMPTY, 0), list);
    }
}

void GenerateAllMoves(const S_BOARD *pos, S_MOVELIST *list)
{
    ASSERT(CheckBoard(pos));
    list->count = 0;

    int pce = EMPTY;
    int side = pos->side;
    int sq = 0;
    int t_sq = 0;
    int pceNum = 0;
    int dir = 0;
    int index = 0;
    int pceIndex = 0;

    // pawn moves
    if (side == WHITE)
    {
        for (pceNum = 0; pceNum < pos->pcsNum[wP]; ++pceNum)
        {
            sq = pos->pList[wP][pceNum];

            ASSERT(SqOnBoard(sq));

            if (pos->pieces[sq + 10] == EMPTY)
            {
                AddWhitePawnMove(pos, sq, sq + 10, list);
                if (RanksBrd[sq] == RANK_2 && pos->pieces[sq + 20] == EMPTY)
                {
                    AddQuietMove(pos, MOVE(sq, (sq + 20), EMPTY, EMPTY, MFLAGPS), list);
                }
            }
            if (!SQOFFBOARD(sq + 9) && PieceCol[pos->pieces[sq + 9]] == BLACK)
            {
                AddWhitePawnCapMove(pos, sq, sq + 9, pos->pieces[sq + 9], list);
            }
            if (!SQOFFBOARD(sq + 11) && PieceCol[pos->pieces[sq + 11]] == BLACK)
            {
                AddWhitePawnCapMove(pos, sq, sq + 11, pos->pieces[sq + 11], list);
            }
            if (pos->enpas != NO_SQ)
            {
                if (sq + 9 == pos->enpas)
                {
                    AddEnPassantMove(pos, MOVE(sq, sq + 9, EMPTY, EMPTY, MFLAGEP), list);
                }
                if (sq + 11 == pos->enpas)
                {
                    AddEnPassantMove(pos, MOVE(sq, sq + 11, EMPTY, EMPTY, MFLAGEP), list);
                }
            }
        }
        if (pos->castlePerm & WKCA)
        {
            if (pos->pieces[F1] == EMPTY && pos->pieces[G1] == EMPTY)
            {
                if (!sqAttacked(E1, BLACK, pos) && !sqAttacked(F1, BLACK, pos))
                {
                    AddQuietMove(pos, MOVE(E1, G1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
        if (pos->castlePerm & WQCA)
        {
            if (pos->pieces[B1] == EMPTY && pos->pieces[C1] == EMPTY && pos->pieces[D1] == EMPTY)
            {
                if (!sqAttacked(E1, BLACK, pos) && !sqAttacked(D1, BLACK, pos))
                {
                    AddQuietMove(pos, MOVE(E1, C1, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
    }
    else
    {
        for (pceNum = 0; pceNum < pos->pcsNum[bP]; ++pceNum)
        {
            sq = pos->pList[bP][pceNum];

            ASSERT(SqOnBoard(sq));

            if (pos->pieces[sq - 10] == EMPTY)
            {
                AddBlackPawnMove(pos, sq, sq - 10, list);
                if (RanksBrd[sq] == RANK_7 && pos->pieces[sq - 20] == EMPTY)
                {
                    AddQuietMove(pos, MOVE(sq, (sq - 20), EMPTY, EMPTY, MFLAGPS), list);
                }
            }
            if (!SQOFFBOARD(sq - 9) && PieceCol[pos->pieces[sq - 9]] == WHITE)
            {
                AddBlackPawnCapMove(pos, sq, sq - 9, pos->pieces[sq - 9], list);
            }
            if (!SQOFFBOARD(sq - 11) && PieceCol[pos->pieces[sq - 11]] == WHITE)
            {
                AddBlackPawnCapMove(pos, sq, sq - 11, pos->pieces[sq - 11], list);
            }
            if (sq - 9 == pos->enpas)
            {
                AddEnPassantMove(pos, MOVE(sq, sq - 9, EMPTY, EMPTY, MFLAGEP), list);
            }
            if (sq - 11 == pos->enpas)
            {
                AddEnPassantMove(pos, MOVE(sq, sq - 11, EMPTY, EMPTY, MFLAGEP), list);
            }
        }
        if (pos->castlePerm & BKCA)
        {
            if (pos->pieces[F8] == EMPTY && pos->pieces[G8] == EMPTY)
            {
                if (!sqAttacked(E8, WHITE, pos) && !sqAttacked(F8, WHITE, pos))
                {
                    AddQuietMove(pos, MOVE(E8, G8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
        if (pos->castlePerm & BQCA)
        {
            if (pos->pieces[B8] == EMPTY && pos->pieces[C8] == EMPTY && pos->pieces[D8] == EMPTY)
            {
                if (!sqAttacked(E8, WHITE, pos) && !sqAttacked(D8, WHITE, pos))
                {
                    AddQuietMove(pos, MOVE(E8, C8, EMPTY, EMPTY, MFLAGCA), list);
                }
            }
        }
    }
    // maj pawns moves

    pceIndex = LoopSlideindex[side];
    pce = LoopSlidePce[pceIndex++];
    while (pce != 0)
    {
        ASSERT(PieceValid(pce));
        for (pceNum = 0; pceNum < pos->pcsNum[pce]; ++pceNum)
        {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));

            for (index = 0; index < NumDir[pce]; ++index)
            {

                dir = PceDir[pce][index];
                t_sq = sq + dir;

                while (!SQOFFBOARD(t_sq))
                {

                    if (pos->pieces[t_sq] != EMPTY)
                    {
                        if (PieceCol[pos->pieces[t_sq]] == (side ^ 1))
                        {
                            AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                        }
                        break;
                    }
                    AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
                    t_sq += dir;
                }
            }
        }
        pce = LoopSlidePce[pceIndex++];
    }

    pceIndex = LoopNonSlideindex[side];
    pce = LoopNonSlidePce[pceIndex++];

    while (pce != 0)
    {
        ASSERT(PieceValid(pce));

        for (pceNum = 0; pceNum < pos->pcsNum[pce]; ++pceNum)
        {
            sq = pos->pList[pce][pceNum];
            ASSERT(SqOnBoard(sq));

            for (index = 0; index < NumDir[pce]; ++index)
            {

                dir = PceDir[pce][index];
                t_sq = sq + dir;

                if (SQOFFBOARD(t_sq))
                {
                    continue;
                }

                if (pos->pieces[t_sq] != EMPTY)
                {
                    if (PieceCol[pos->pieces[t_sq]] == side ^ 1)
                    {
                        AddCaptureMove(pos, MOVE(sq, t_sq, pos->pieces[t_sq], EMPTY, 0), list);
                    }
                    continue;
                }
                AddQuietMove(pos, MOVE(sq, t_sq, EMPTY, EMPTY, 0), list);
            }
        }
        pce = LoopNonSlidePce[pceIndex++];
    }
}
