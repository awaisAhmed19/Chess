#include "defs.h"
#include "stdio.h"
#include "string.h"

int ThreeFoldRep(const S_BOARD *pos) {
  int i = 0, r = 0;
  for (i = 0; i < pos->hisply; ++i) {
    if (pos->history[i].posKey == pos->PosKey) {
      r++;
    }
  }
  return r;
}

int DrawMaterial(const S_BOARD *pos) {
  if (pos->pcsNum[wP] || pos->pcsNum[bP])
    return FALSE;
  if (pos->pcsNum[wQ] || pos->pcsNum[bQ] || pos->pcsNum[wR] || pos->pcsNum[bR])
    return FALSE;
  if (pos->pcsNum[wB] > 1 || pos->pcsNum[bB] > 1) {
    return FALSE;
  }
  if (pos->pcsNum[wN] > 1 || pos->pcsNum[bN] > 1) {
    return FALSE;
  }
  if (pos->pcsNum[wN] && pos->pcsNum[wB]) {
    return FALSE;
  }
  if (pos->pcsNum[bN] && pos->pcsNum[bB]) {
    return FALSE;
  }

  return TRUE;
}

int CheckResult(S_BOARD *pos) {
  if (pos->fiftyMove > 100) {
    printf("1/2-1/2 (Fifty move rule (claimed by chess 1.0) \n ");
    return TRUE;
  }
  if (ThreeFoldRep(pos) >= 2) {
    printf("1/2-1/2 (3- fold repitation (claimed by chess 1.0) \n ");
    return TRUE;
  }
  if (DrawMaterial(pos) == TRUE) {
    printf("1/2-1/2 (3 fold repitation (claiming by chess 1.0) \n ");
    return TRUE;
  }

  S_MOVELIST list[1];
  GenerateAllMoves(pos, list);

  int MoveNum = 0;
  int found = 0;
  for (MoveNum = 0; MoveNum < list->count; ++MoveNum) {
    if (!MakeMove(pos, list->moves[MoveNum].move)) {
      continue;
    }
    found++;
    TakeMove(pos);
    break;
  }
}
