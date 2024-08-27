#include "defs.h"
#include "stdio.h"
#define HASH_PCE(pce, sq) (pos->PosKey ^= (PieceKeys[(pce)][(sq)]))
#define HASH_CA (pos->PosKey ^= (CastleKeys[(pos->castlePerm)]))
#define HASH_SIDE (pos->PosKey ^= (SideKey))
#define HASH_EP (pos->PosKey ^= (PieceKeys[EMPTY][(pos->enpas)]))

// ca_perm &= CastlePerm[from]
// if from or to was a sq that has the value coressponding to 7,3,11,13,12,14
// the castle permission is updated and the castleing is denied. as long as
// ca_perm is 1111(15) the castlepermissiion remains for that side and changes
// when castling occurs or the king or both the rooks move

const int
    CastlePerm[120] = {15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                       15, 15, 15, 15, 15, 15, 15, 15, 15, 13, 15, 15,
                       15, 12, 15, 15, 14, 15, // a8=13 (1101) e8=12 (1100)
                                               // h8=14 (1110)
                       15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                       15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                       15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                       15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                       15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                       15, 7,  15, 15, 15, 3,  15, 15, 11, 15, // a1 =7 0111 and
                                                               // e1 =3 0011
                                                               // h1=11 1011
                       15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
                       15, 15, 15, 15, 15, 15, 15, 15};

static void ClearPiece(const int sq, S_BOARD *pos) {
  ASSERT(SqOnBoard(sq));

  int pce = pos->pieces[sq];

  ASSERT(PieceValid(pce));

  int col = PieceCol[pce];
  int index = 0;
  int t_pceNum = -1;

  HASH_PCE(pce, sq);

  pos->pieces[sq] = EMPTY;
  pos->material[col] -= PieceVal[pce];

  if (PieceBig[pce]) {
    pos->bigPce[col]--;
    if (PieceMaj[pce]) {
      pos->majPce[col]--;
    } else {
      pos->minPce[col]--;
    }
  } else {
    CLRBIT(pos->pawns[col], SQ64(sq));
    CLRBIT(pos->pawns[BOTH], SQ64(sq));
  }

  for (index = 0; index < pos->pcsNum[pce]; ++index) {
    if (pos->pList[pce][index] == sq) {
      t_pceNum = index;
      break;
    }
  }

  ASSERT(t_pceNum != -1);
  pos->pcsNum[pce]--;
  pos->pList[pce][t_pceNum] = pos->pList[pce][pos->pcsNum[pce]];
}

static void AddPiece(const int sq, S_BOARD *pos, const int pce) {

  ASSERT(PieceValid(pce));
  ASSERT(SqOnBoard(sq));

  pos->pieces[sq] = pce;
  HASH_PCE(pce, sq);

  int col = PieceCol[pce];

  if (PieceBig[pce]) {
    pos->bigPce[col]++;
    if (PieceMaj[pce]) {
      pos->majPce[col]++;
    } else {
      pos->minPce[col]++;
    }
  } else {
    SETBIT(pos->pawns[col], SQ64(sq));
    SETBIT(pos->pawns[BOTH], SQ64(sq));
  }

  pos->material[col] += PieceVal[pce];
  pos->pList[pce][pos->pcsNum[pce]++] = sq;
}

static void MovePiece(const int from, const int to, S_BOARD *pos) {

  ASSERT(SqOnBoard(from));
  ASSERT(SqOnBoard(to));
  int index = 0;
  int pce = pos->pieces[from];
  int col = PieceCol[pce];
  int t_PieceNum = FALSE;
#ifndef DEBUG
  t_PieceNum = FALSE;

#endif
  HASH_PCE(pce, from);
  pos->pieces[from] = EMPTY;
  HASH_PCE(pce, to);
  pos->pieces[to] = pce;

  if (!PieceBig[pce]) {
    CLRBIT(pos->pawns[col], SQ64(from));
    CLRBIT(pos->pawns[BOTH], SQ64(from));
    SETBIT(pos->pawns[col], SQ64(to));
    SETBIT(pos->pawns[BOTH], SQ64(to));
  }

  for (index = 0; index < pos->pcsNum[pce]; ++index) {
    if (pos->pList[pce][index] == from) {
      pos->pList[pce][index] = to;

#ifndef DEBUG
      t_PieceNum = TRUE;

#endif
      break;
    }
  }
  // ASSERT(t_PieceNum);
}

int MakeMove(S_BOARD *pos, int move) {
  ASSERT(CheckBoard(pos));

  int from = FROMSQ(move);
  int to = TOSQ(move);
  int side = pos->side;

  ASSERT(SqOnBoard(from));
  ASSERT(SqOnBoard(to));
  ASSERT(SideValid(side));
  ASSERT(PieceValid(pos->pieces[from]));

  pos->history[pos->hisply].posKey = pos->PosKey;

  if (move & MFLAGEP) {
    if (side == WHITE) {
      ClearPiece(to - 10, pos);
    } else {
      ClearPiece(to + 10, pos); // black pawn move
    }
  } else if (move & MFLAGCA) {
    switch (to) {
    case C1:
      MovePiece(A1, D1, pos);
      break;
    case C8:
      MovePiece(A8, D8, pos);
      break;
    case G1:
      MovePiece(H1, F1, pos);
      break;
    case G8:
      MovePiece(H8, F8, pos);
      break;
    default:
      ASSERT(FALSE);
      break;
    }
  }

  if (pos->enpas != NO_SQ)
    HASH_EP;
  HASH_CA;

  pos->history[pos->hisply].move = move;
  pos->history[pos->hisply].fiftyMove = pos->fiftyMove;
  pos->history[pos->hisply].enpas = pos->enpas;
  pos->history[pos->hisply].castlePerm = pos->castlePerm;

  pos->castlePerm &= CastlePerm[from];
  pos->castlePerm &= CastlePerm[to];
  pos->enpas = NO_SQ;

  HASH_CA;

  int captured = CAPTURED(move);
  pos->fiftyMove++;

  if (captured != EMPTY) {
    ASSERT(PieceValid(captured));
    ClearPiece(to, pos);
    pos->fiftyMove = 0;
  }

  pos->hisply++;
  pos->ply++;

  if (PiecePawn[pos->pieces[from]]) {
    pos->fiftyMove = 0;
    if (move & MFLAGPS) {
      if (side == WHITE) {
        pos->enpas = from + 10;
        ASSERT(RanksBrd[pos->enpas] == RANK_3);
      } else {
        pos->enpas = from - 10;
        ASSERT(RanksBrd[pos->enpas] == RANK_6);
      }
      HASH_EP;
    }
  }
  MovePiece(from, to, pos);

  int prPce = PROMOTED(move);
  if (prPce != EMPTY) {
    ASSERT(PieceValid(prPce) && !PiecePawn[prPce]);
    ClearPiece(to, pos);
    AddPiece(to, pos, prPce);
  }

  if (PieceKing[pos->pieces[to]]) {
    pos->KingSq[pos->side] = to;
  }

  pos->side ^= 1;
  HASH_SIDE;

  ASSERT(CheckBoard(pos));

  if (sqAttacked(pos->KingSq[side], pos->side, pos)) {
    TakeMove(pos);
    return FALSE;
  }

  return TRUE;
}

void TakeMove(S_BOARD *pos) {
  ASSERT(CheckBoard(pos));
  // decrementing ply and hisply
  pos->hisply--;
  pos->ply--;

  // poping the previous move from history array
  // getting the from and to sq for the move popped
  int move = pos->history[pos->hisply].move;
  int from = FROMSQ(move);
  int to = TOSQ(move);

  ASSERT(SqOnBoard(from)); // check if the move is on the board
  ASSERT(SqOnBoard(to));

  if (pos->enpas != NO_SQ) // check if the enpas sq is not nosq and hash a new
                           // ep and castleing if true
    HASH_EP;
  HASH_CA;

  pos->castlePerm = pos->history[pos->hisply].castlePerm;
  pos->enpas = pos->history[pos->hisply].enpas;
  pos->fiftyMove = pos->history[pos->hisply].fiftyMove;

  if (pos->enpas != NO_SQ)
    HASH_EP;
  HASH_CA;

  pos->side ^= 1;
  HASH_SIDE;

  if (MFLAGEP & move) {
    if (pos->side == WHITE) {
      AddPiece(to - 10, pos, bP);
    } else {
      AddPiece(to + 10, pos, wP);
    }
  } else if (MFLAGCA & move) {
    switch (to) {
    case C1:
      MovePiece(D1, A1, pos);
      break;
    case C8:
      MovePiece(D8, A8, pos);
      break;
    case G1:
      MovePiece(F1, H1, pos);
      break;
    case G8:
      MovePiece(F8, H8, pos);
      break;
    default:
      ASSERT(FALSE);
      break;
    }
  }
  MovePiece(to, from, pos);

  if (PieceKing[pos->pieces[from]]) {
    pos->KingSq[pos->side] = from;
  }
  int captured = CAPTURED(move);
  if (captured != EMPTY) {
    ASSERT(PieceValid(captured));
    AddPiece(to, pos, captured);
  }
  if (PROMOTED(move) != EMPTY) {
    ASSERT(PieceValid(PROMOTED(move)) && !PiecePawn[PROMOTED(move)]);
    ClearPiece(from, pos); // clear the promoted maj piece and
    // then add the pawn piece on the previous rank with appropriate colour
    AddPiece(from, pos, (PieceCol[PROMOTED(move)] == WHITE ? wP : bP));
  }
  ASSERT(CheckBoard(pos));
}
