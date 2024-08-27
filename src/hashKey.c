#include "defs.h"
#include "stdio.h"

U64 GeneratePosKeys(const S_BOARD *pos) {
  int sq = 0;
  U64 finalKey = 0;
  int piece = EMPTY;

  for (sq = 0; sq < BRD_SQ_NUM; ++sq) {
    piece = pos->pieces[sq];
    if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD) {
      // printf("Debug: piece = %d\n", piece);
      ASSERT(piece >= wP && piece <= bK);
      finalKey ^= PieceKeys[piece][sq];
    }
  }

  if (pos->side == WHITE) {
    finalKey ^= SideKey;
  }
  if (pos->enpas != NO_SQ) {
    ASSERT(pos->enpas >= 0 && pos->enpas < BRD_SQ_NUM);
    finalKey ^= PieceKeys[EMPTY][pos->enpas];
  }
  ASSERT(pos->castlePerm >= 0 && pos->castlePerm <= 15);
  finalKey ^= CastleKeys[pos->castlePerm];

  return finalKey;
}
