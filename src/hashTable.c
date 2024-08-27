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

void InitPvTable(S_PVTABLE *table, const int MB) {
  const int PvSize = 0x100000 * MB;
  table->numEnteries = PvSize / sizeof(S_PVENTRY);
  table->numEnteries -= 2;
  if (table->pTable != NULL) {
    free(table->pTable);
  }
  table->pTable = (S_PVENTRY *)malloc(table->numEnteries * sizeof(S_PVENTRY));
  if (table->pTable == NULL) {
    printf("hash Allocation failed, trying %dMB...\n", MB / 2);
    InitPvTable(table, MB / 2);
  } else {
    clearPvTable(table);
    printf("Pvtable init complete with %d entries ", table->numEnteries);
  }
}

void StorePvMove(const S_BOARD *pos, int move) {
  int index = pos->PosKey % pos->pvTable->numEnteries;
  ASSERT(index >= 0 && index <= pos->pvTable->numEnteries - 1);
  // ASSERT(score >= -INFINITE && score <= INFINITE);
  ASSERT(pos->ply >= 0 && pos->ply < MAXDEPTH);
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
