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

  if (found != 0)
    return FALSE;

  int InCheck = sqAttacked(pos->KingSq[pos->side], pos->side ^ 1, pos);

  if (InCheck == TRUE) {
    if (pos->side == WHITE) {
      printf("0-1 (black mates (claimed by Chess 1.0\n");
      return TRUE;
    } else {
      printf("0-1 (white mates (claimed by Chess 1.0\n");
      return TRUE;
    }
  } else {
    printf("\n 1/2-1/2 (stalemate (Claimed by Chess 1.0))\n ");
    return TRUE;
  }
  return FALSE;
}

void xboardLoop(S_BOARD *pos, S_SEARCHINFO *info) {
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);
  int depth = -1, movestogo[2] = {30, 30}, movetime = -1;
  int time = -1, inc = 0;
  int engineSide = BOTH;
  int timeLeft;
  int mps;
  int sec;
  int move = NOMOVE;
  int i, score;
  char inBuf[80], command[80];

  while (TRUE) {
    fflush(stdout);

    if (pos->side == engineSide && CheckResult(pos) == FALSE) {
      // thinking
      info->starttime = getTimeMs();
      info->depth = depth;

      if (time != -1) {
        info->timeset = TRUE;
        time /= movestogo[pos->side];
        time -= 50;
        info->stoptime = info->starttime + time + inc;
      }
      if (depth == -1 || depth > MAXDEPTH) {
        info->depth = MAXDEPTH;
      }

      printf("time: %d start: %d stop: %d depth: %d timeset: %d movestogo: %d "
             "mps: %d\n",
             time, info->starttime, info->stoptime, info->depth, info->timeset,
             movestogo[pos->side], mps);
      SearchPosition(pos, info);
      if (mps != 0) {
        movestogo[pos->side ^ 1]--;
        if (movestogo[pos->side ^ 1] < 1) {
          movestogo[pos->side ^ 1] = mps;
        }
      }
    }

    fflush(stdout);
    memset(&inBuf[0], 0, sizeof(inBuf));
    fflush(stdout);
    if (!fgets(inBuf, 80, stdin)) {
      continue;
    }
    sscanf(inBuf, "%s", command);
    if (!strcmp(command, "quit")) {
      break;
    }
    if (!strcmp(command, "force")) {
      engineSide = BOTH;
      continue;
    }
    if (!strcmp(command, "time")) {
      sscanf(inBuf, "time %d", &time);
      time *= 10;
      printf("DEBUG time: %d\n", time);
      continue;
    }
    if (!strcmp(command, "level")) {
      sec = 0;
      movetime = -1;
      if (sscanf(inBuf, "level %d %d %d ", &mps, &timeLeft, &inc) != 3) {
        sscanf(inBuf, "level %d %d:%d %d ", &mps, &timeLeft, &sec, &inc);
        printf("DEBUG level with \n");
      } else {
        printf("DEBUG level without:\n");
      }
      timeLeft *= 60000;
      timeLeft += sec * 1000;
      movestogo[0] = movestogo[1] = 30;
      if (mps != 0) {
        movestogo[0] = movestogo[1] = mps;
      }
      time = -1;
      printf("DEBUG level timeleft : %d movestogo: %d inc: %d mps: %d",
             timeLeft, movestogo[0], inc, mps);
      continue;
    }
    if (!strcmp(command, "protover")) {
      printf("Feature ping=1 setboard=1 color=0 usermove=1\n");
      printf("feature done=1 \n");
      continue;
    }
    if (!strcmp(command, "sd")) {
      sscanf(inBuf, "sd %d", &depth);
      continue;
    }
    if (!strcmp(command, "st")) {
      sscanf(inBuf, "st %d", &movetime);
      continue;
    }
    if (!strcmp(command, "ping")) {
      printf("ping %s\n", inBuf + 4);
      continue;
    }
    if (!strcmp(command, "new")) {
      engineSide = BLACK;
      ParseFen(START_FEN, pos);
      depth = -1;
      continue;
    }
    if (!strcmp(command, "setboard")) {
      engineSide = BOTH;
      ParseFen(inBuf, pos);
      continue;
    }
    if (!strcmp(command, "go")) {
      engineSide = pos->side;
      continue;
    }
    if (!strcmp(command, "usermove")) {
      movestogo[pos->side]--;
      move = ParseMove(inBuf + 9, pos);
      if (move == NOMOVE)
        continue;
      MakeMove(pos, move);
      pos->ply = 0;
    }
  }
}
