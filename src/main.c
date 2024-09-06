#include "defs.h"

#define TEST                                                                   \
  "r1b1k2r/ppppnppp/2n2q2/2b5/3NP3/2P1B3/PP3PPP/RN1QKB1R w KQkq - 0 1 "

#define TEST2 "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - "
int main(void) {

  AllInit();
  S_BOARD pos[1];
  S_SEARCHINFO info[1];
  InitPvTable(pos->pvTable, 2);
  UciLoop(pos, info);
  free(pos->pvTable->pTable);
  return 0;
}
