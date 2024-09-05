#include "defs.h"

#define TEST                                                                   \
  "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1 "

#define TEST2 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - "
int main(void) {
  AllInit();
  // int d = 3;
  S_BOARD board[1];
  InitPvTable(board->pvTable, 2);
  S_MOVELIST list[1];
  S_SEARCHINFO info[1];
  ParseFen(TEST2, board);

  char input[6];
  int Move = NOMOVE;
  int pvNum = 0;
  int Max = 0;

  while (TRUE) {
    PrintBoard(board);
    printf("please enter a move >");
    fgets(input, 6, stdin);

    if (input[0] == 'q') {
      break;
    } else if (input[0] == 't') {
      TakeMove(board);
    } else if (input[0] == 'p') {
      Max = GetPvLine(4, board);
      printf("pvLine of %d moves", Max);
      for (pvNum = 0; pvNum < Max; ++pvNum) {
        Move = board->pvArray[pvNum];
        printf("%s ", PrMove(Move));
      }
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
  free(board->pvTable->pTable);
  return 0;
}
