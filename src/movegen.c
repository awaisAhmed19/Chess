#include "stdio.h"
#include "defs.h"

#define MOVE(f, t, ca, prom, fl) ((f) | ((t) << 7) | ((ca) << 14) | ((prom) << 20) | (fl))
#define SQOFFBOARD(sq) (FilesBrd[(sq)] == OFFBOARD)

int LoopSlidePce[8] = {
    wB, wR, wQ, 0, bB, bR, bQ, 0};

int LoopSlideindex[2] = {0, 4};

int LoopNonSlidePce[8] = {
    wN, wK, 0, bN, bK, 0};

int LoopNonSlideindex[2] = {0, 3};

void AddQuietMove(const S_BOARD *pos, int move, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddCaptureMove(const S_BOARD *pos, int move, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddEnPassantMove(const S_BOARD *pos, int move, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(FROMSQ(move)));
    ASSERT(SqOnBoard(TOSQ(move)));
    ASSERT(CheckBoard(pos));
    list->moves[list->count].move = move;
    list->moves[list->count].score = 0;
    list->count++;
}

void AddWhitePawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list)
{
    ASSERT(SqOnBoard(from));
    ASSERT(SqOnBoard(to));
    ASSERT(CheckBoard(pos));
    // ASSERT(PieceValidEmpty(cap));
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

void AddBlackPawnCapMove(const S_BOARD *pos, const int from, const int to, const int cap, S_MOVELIST *list)
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

void AddWhitePawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list)
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

void AddBlackPawnMove(const S_BOARD *pos, const int from, const int to, S_MOVELIST *list)
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

    printf("side: %d", side);
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
    }
    // maj pawns moves

    pceIndex = LoopSlideindex[side];
    pce = LoopSlidePce[pceIndex++];
    while (pce != 0)
    {
        // ASSERT(PieceValid(pce));
        printf("sliding pieceindex :%d  pce :%d \n", pceIndex, pce);
        pce = LoopSlidePce[pceIndex++];
    }
}
