#include "defs.h"
#include "locale.h"
#include <stdio.h>
#include <stdlib.h>

#define TEST "n1n5/PPPk4/8/8/8/8/4Kppp/5N1N w - - 0 1 "

int main(void) {
  AllInit();
  // int d = 3;
  S_BOARD board[1];
  // S_MOVELIST list[1];
  S_SEARCHINFO info[1];
  ParseFen(START_FEN, board);

  char input[6];
  int Move = NOMOVE;
  // int pvNum = 0;
  // int Max = 0;

  while (TRUE) {
    PrintBoard(board);
    printf("please enter a move >");
    fgets(input, 6, stdin);

    if (input[0] == 'q') {
      break;
    } else if (input[0] == 't') {
      TakeMove(board);
    } else if (input[0] == 's') {
      info->depth = 4;
      SearchPosition(board, info);
    } else {
      Move = ParseMove(input, board);
      if (Move != NOMOVE) {
        StorePvMove(board, Move);
        MakeMove(board, Move);
      } else {
        printf("Move not parsed %s", input);
      }
    }
    fflush(stdin);
  }
  return 0;
}
