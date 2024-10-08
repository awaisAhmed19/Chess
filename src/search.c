#include "defs.h"
#include "stdio.h"
#define MATE 29000
static void checkUp(S_SEARCHINFO *info) {

  if (info->timeset == TRUE && getTimeMs() > info->stoptime) {
    info->stopped = TRUE;
  }
  ReadInput(info);
}

static void PickNextMove(int moveNum, S_MOVELIST *list) {
  S_MOVE temp;
  int index = 0;
  int bestScore = 0;
  int bestNum = moveNum;

  for (index = moveNum; index < list->count; ++index) {
    if (list->moves[index].score > bestScore) {
      bestScore = list->moves[index].score;
      bestNum = index;
    }
  }

  temp = list->moves[moveNum];
  list->moves[moveNum] = list->moves[bestNum];
  list->moves[bestNum] = temp;
}
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
  ASSERT(CheckBoard(pos));
  if ((info->nodes & 2047) == 0) {
    checkUp(info);
  }
  info->nodes++;
  if (IsRepetition(pos) || pos->fiftyMove >= 100) {
    return 0;
  }

  if (pos->ply > MAXDEPTH - 1) {
    return EvalPosition(pos);
  }

  int Score = EvalPosition(pos);

  if (Score >= beta) {
    return beta;
  }
  if (Score > alpha) {
    alpha = Score;
  }

  S_MOVELIST list[1];
  GenerateAllCaps(pos, list);

  int Movenum = 0;
  int Legal = 0;
  int BestMove = NOMOVE;
  int oldAlpha = alpha;
  Score = -INFINITE;
  int PvMove = probePvtable(pos);

  if (PvMove != NOMOVE) {
    for (Movenum = 0; Movenum < list->count; ++Movenum) {
      if (list->moves[Movenum].move == PvMove) {
        list->moves[Movenum].score = 2000000;
        break;
      }
    }
  }

  for (Movenum = 0; Movenum < list->count; ++Movenum) {
    PickNextMove(Movenum, list);
    if (!MakeMove(pos, list->moves[Movenum].move)) {
      continue;
    }

    Legal++;
    Score = -Quiescence(-beta, -alpha, pos, info);
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
  if (oldAlpha != alpha) {
    StorePvMove(pos, BestMove);
  }
  return alpha;
}

static int AlphaBeta(int alpha, int beta, int depth, S_BOARD *pos,
                     S_SEARCHINFO *info, int DoNULL) {
  ASSERT(CheckBoard(pos));
  ASSERT(beta > alpha);

  if (depth == 0) {
    return Quiescence(alpha, beta, pos, info);
    // info->nodes++;
    // return EvalPosition(pos);
  }

  if ((info->nodes & 2047) == 0) {
    checkUp(info);
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
  int PvMove = probePvtable(pos);

  if (PvMove != NOMOVE) {
    for (Movenum = 0; Movenum < list->count; ++Movenum) {
      if (list->moves[Movenum].move == PvMove) {
        list->moves[Movenum].score = 2000000;
        break;
      }
    }
  }

  for (Movenum = 0; Movenum < list->count; ++Movenum) {
    PickNextMove(Movenum, list);
    if (!MakeMove(pos, list->moves[Movenum].move)) {
      continue;
    }

    Legal++;
    Score = -AlphaBeta(-beta, -alpha, depth - 1, pos, info, TRUE);
    TakeMove(pos);
    if (info->stopped == TRUE) {
      return 0;
    }
    if (Score > alpha) {
      if (Score >= beta) {
        if (Legal == 1) {
          info->fhf++;
        }
        info->fh++;
        if (!(list->moves[Movenum].move & MFLAGCAP)) {
          pos->searchKiller[1][pos->ply] = pos->searchKiller[0][pos->ply];
          pos->searchKiller[0][pos->ply] = list->moves[Movenum].move;
        }
        return beta;
      }
      alpha = Score;
      BestMove = list->moves[Movenum].move;
      if (!(list->moves[Movenum].move & MFLAGCAP)) {
        pos->searchHistory[pos->pieces[FROMSQ(BestMove)]][TOSQ(BestMove)] +=
            depth;
        pos->searchKiller[0][pos->ply] = list->moves[Movenum].move;
      }
    }
  }

  if (Legal == 0) {
    if (sqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos)) {
      return -MATE + pos->ply;
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

  for (currentDepth = 1; currentDepth <= info->depth; ++currentDepth) {
    bestScore = AlphaBeta(-INFINITE, INFINITE, currentDepth, pos, info, TRUE);

    if ((info->stopped) == TRUE) {
      break;
    }
    pvMoves = GetPvLine(currentDepth, pos);
    bestMove = pos->pvArray[0];
    if (info->GAME_MODE == UCIMODE) {
      printf("info score cp %d depth %d nodes %ld time %d ", bestScore,
             currentDepth, info->nodes, getTimeMs() - info->starttime);
    } else if (info->GAME_MODE == XBOARDMODE && info->POST_THINKING == TRUE) {
      printf("%d %d %d %ld", currentDepth, bestScore,
             (getTimeMs() - info->starttime / 10), info->nodes);
    } else if (info->POST_THINKING == TRUE) {
      printf("score: %d depth: %d nodes: %ld time: %d ", bestScore,
             currentDepth, info->nodes, getTimeMs() - info->starttime);
    }
    if (info->GAME_MODE == UCIMODE || info->POST_THINKING == TRUE) {

      pvMoves = GetPvLine(currentDepth, pos);
      printf("pv");
      for (pvNum = 0; pvNum < pvMoves; ++pvNum) {
        printf(" %s ", PrMove(pos->pvArray[pvNum]));
      }
      printf("\n");
      //    printf("ordering :%.2f\n", (info->fhf / info->fh));
    }
  }
  if (info->GAME_MODE == UCIMODE) {
    printf("BestMove %s", PrMove(bestMove));
  } else if (info->GAME_MODE == XBOARDMODE) {
    MakeMove(pos, bestMove);
  } else {
    printf("\n\n***!! Chess Makes move %s !!***\n\n", PrMove(bestMove));
    PrintBoard(pos);
  }
}
