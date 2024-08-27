#include "defs.h"
#include "stdio.h"

static void checkUp() {}

static int IsRepetition(S_BOARD *pos) {
  for (int index = pos->hisply - pos->fiftyMove; index < pos->hisply - 1;
       ++index) {
    if (pos->PosKey == pos->history[index].posKey) {
      return TRUE;
    }
  }
  return FALSE;
}

static void ClearForSearch(S_BOARD *pos, S_SEARCHINFO *info) {
  for (int i = 0; i < 13; ++i) {
    for (int j = 0; j < BRD_SQ_NUM; ++j) {
      pos->searchHistory[i][j] = 0;
    }
  }

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < MAXDEPTH; ++j) {
      pos->searchKiller[i][j] = 0;
    }
  }

  clearPvTable(pos->pvTable);
  pos->ply = 0;
  info->starttime = getTimeMs();
  info->stopped = 0;
  info->nodes = 0;
  info->fh = 0;
  info->fhf = 0;
}

static int Quiescence(int alpha, int beta, S_BOARD *pos, S_SEARCHINFO *info) {
  return 0;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos,
                     S_SEARCHINFO *info, int DoNULL) {
  ASSERT(CheckBoard(pos));
  ASSERT(beta > alpha);

  if (depth == 0) {
    info->nodes++;
    return EvalPosition(pos);
  }
  info->nodes++;

  if (IsRepetition(pos) || pos->fiftyMove >= 100) {
    return 0;
  }
  if (pos->ply > MAXDEPTH - 1) {
    return EvalPosition(pos);
  }

  S_MOVELIST list[1];
  GenerateAllMoves(pos, list);

  int Movenum = 0;
  int Legal = 0;
  int BestMove = NOMOVE;
  int oldAlpha = alpha;
  int Score = -INFINITE;

  for (Movenum = 0; Movenum < list->count; ++Movenum) {
    if (!MakeMove(pos, list->moves[Movenum].move)) {
      continue;
    }

    Legal++;
    Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
    TakeMove(pos);

    if (Score > alpha) {
      if (Score >= beta) {
        if (Legal == 1) {
          info->fhf++;
        }
        info->fh++;
        return beta;
      }
      alpha = Score;
      BestMove = list->moves[Movenum].move;
    }
  }

  if (Legal == 0) {
    if (sqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos)) {
      return -INFINITE + pos->ply;
    } else {
      return 0;
    }
  }

  if (alpha != oldAlpha) {
    StorePvMove(pos, BestMove);
  }
  return alpha;
}

void SearchPosition(S_BOARD *pos, S_SEARCHINFO *info) {
  int bestMove = NOMOVE;
  int bestScore = -INFINITE;
  int currentDepth = 0;
  int pvMoves = 0;
  int pvNum = 0;

  ClearForSearch(pos, info);

  for (currentDepth = 0; currentDepth <= info->depth; ++currentDepth) {
    bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);
    pvMoves = GetPvLine(currentDepth, pos);
    bestMove = pos->pvArray[0];
    printf("Depth: %d score: %d move:%s nodes: %ld", currentDepth, bestScore,
           PrMove(bestMove), info->nodes);
    pvMoves = GetPvLine(currentDepth, pos);
    printf("pv");
    for (pvNum = 0; pvNum < pvMoves; ++pvNum) {
      printf("%s ", PrMove(pos->pvArray[pvNum]));
    }
    printf("\n");
    printf("ordering :%.2f\n", (info->fhf / info->fh));
  }
}
