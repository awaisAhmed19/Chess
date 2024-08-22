#include "defs.h"
#include "stdio.h"

int IsRepetition(S_BOARD *pos)
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
void SearchPosition(S_BOARD *pos)
{
}