#include "defs.h"
#include "stdio.h"
static void checkUp()
{
}

static int IsRepetition(S_BOARD *pos)
{
    for (int index = pos->hisply - pos->fiftyMove; index < pos->hisply - 1; ++index)
    {
        if (pos->PosKey == pos->history[index].posKey)
        {
            return TRUE;
        }
    }
    return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info)
{
    for (int i = 0; i < 13; ++i)
    {
        for (int j = 0; j < BRD_SQ_NUM; ++j)
        {
            pos->searchHistory[i][j] = 0;
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < MAXDEPTH; ++j)
        {
            pos->searchKiller[i][j] = 0;
        }
    }

    clearPvTable(pos->pvTable);
    pos->ply = 0;
    info->starttime = getTimeMs();
    info->stopped = 0;
    info->nodes = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info)
{
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos, S_SEARCHINFO *info, int DoNULL)
{
    return 0;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info)
{
}