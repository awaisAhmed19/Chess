#include "defs.h"
#include "stdio.h"

int GetPvLine(const int depth, S_BOARD *pos) {
  ASSERT(depth < MAXDEPTH);

  int move = probePvtable(pos);
  int count = 0;

  while (move != NOMOVE && count < depth) {
    ASSERT(count < MAXDEPTH);

    if (MoveExist(pos, move)) {
      MakeMove(pos, move);
      pos->pvArray[count++] = move;
    } else {
      break;
    }
    move = probePvtable(pos);
  }
  while (pos->ply > 0) {
    TakeMove(pos);
  }
  return count;
}

void clearPvTable(S_PVTABLE *table) {
  S_PVENTRY *pvEntry;
  for (pvEntry = table->pTable; pvEntry < table->pTable + table->numEnteries;
       pvEntry++) {
    pvEntry->posKey = 0ULL;
    pvEntry->move = NOMOVE;
  }
}

const int PvSize = 0x100000 * 2;

void InitPvTable(S_PVTABLE *table) {
  table->numEnteries = PvSize / sizeof(S_PVENTRY);
  table->numEnteries -= 2;
  free(table->pTable);
  table->pTable = (S_PVENTRY *)malloc(table->numEnteries * sizeof(S_PVENTRY));
  clearPvTable(table);
  printf("Pvtable init complete with %d entries ", table->numEnteries);
}

void StorePvTable(const S_BOARD *pos, int move) {
  int index = pos->PosKey % pos->pvTable->numEnteries;
  ASSERT(index >= 0 && index <= pos->pvTable->numEnteries - 1);
  pos->pvTable->pTable[index].move = move;
  pos->pvTable->pTable[index].posKey = pos->PosKey;
}

int probePvtable(const S_BOARD *pos) {
  int index = pos->PosKey % pos->pvTable->numEnteries;
  ASSERT(index >= 0 && index <= pos->pvTable->numEnteries - 1);
  if (pos->pvTable->pTable[index].posKey == pos->PosKey) {
    return pos->pvTable->pTable[index].move;
  }
  return NOMOVE;
}
