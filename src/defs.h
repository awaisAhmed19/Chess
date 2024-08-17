#ifndef DEFS_H
#define DEFS_H
#include "stdlib.h"
#include "locale.h"
typedef unsigned long long U64;

#define Name "chess 1.0"
#define BRD_SQ_NUM 120

#define MAXPOSITIONMOVES 256
#define MAXGAMEMOVE 2048
#define DEBUG

#ifndef DEBUG
#define ASSERT(n)
#else
#define ASSERT(n)                          \
    if (!(n))                              \
    {                                      \
        printf("%s -Failed \n", #n);       \
        printf("On %s \n", __DATE__);      \
        printf("At %s \n", __TIME__);      \
        printf("In File %s \n", __FILE__); \
        printf("At Line %d \n", __LINE__); \
        exit(1);                           \
    }
#endif

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

enum
{
    EMPTY,
    wP,
    wN,
    wB,
    wR,
    wQ,
    wK,
    bP,
    bN,
    bB,
    bR,
    bQ,
    bK
};
enum
{
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_NONE
};
enum
{
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_NONE
};

enum
{
    WHITE,
    BLACK,
    BOTH
};
enum
{
    UCIMODE,
    XBOARDMODE,
    CONSOLEMODE
};
enum
{
    A1 = 21,
    B1,
    C1,
    D1,
    E1,
    F1,
    G1,
    H1,
    A2 = 31,
    B2,
    C2,
    D2,
    E2,
    F2,
    G2,
    H2,
    A3 = 41,
    B3,
    C3,
    D3,
    E3,
    F3,
    G3,
    H3,
    A4 = 51,
    B4,
    C4,
    D4,
    E4,
    F4,
    G4,
    H4,
    A5 = 61,
    B5,
    C5,
    D5,
    E5,
    F5,
    G5,
    H5,
    A6 = 71,
    B6,
    C6,
    D6,
    E6,
    F6,
    G6,
    H6,
    A7 = 81,
    B7,
    C7,
    D7,
    E7,
    F7,
    G7,
    H7,
    A8 = 91,
    B8,
    C8,
    D8,
    E8,
    F8,
    G8,
    H8,
    NO_SQ,
    OFFBOARD
};

enum
{
    FALSE,
    TRUE
};

enum
{
    WKCA = 1,
    WQCA = 2,
    BKCA = 4,
    BQCA = 8
};

typedef struct
{
    int move;
    int castlePerm;
    int fiftyMove;
    U64 posKey; // 64 bits of value

} S_UNDO;

typedef struct
{
    int move;
    int score;
} S_MOVE;

typedef struct
{
    S_MOVE moves[MAXPOSITIONMOVES];
    int count;
} S_MOVELIST;

typedef struct
{
    int pieces[BRD_SQ_NUM]; // the whole 120 matrix of 12x10
    U64 pawns[3];           // types of pawn WHITE ,BLACK ,OR BOTH

    int KingSq[2]; // BLACK and WHITE KINGSQ values

    int side;      // either WHITE or BLACK
    int enpas;     // 64 Sq value of which sq is an enpas sq
    int fiftyMove; // checkes if there are 50 moves with no change in score or position value leading to a draw

    int ply;
    int hisply;

    int castlePerm; // 0 to 15 value for the castle permission
    U64 PosKey;     // ULL value that  hold a randomly generated value for the position of the pawns

    int pcsNum[13]; // hold the number of piece of each type
    int bigPce[2];  // hold the count for the big peices like the rooks and queen
    int majPce[2];  // holds the count for the maj pieces like the bishops
    int minPce[2];
    int material[2]; // counts the number of pieces in each side

    S_UNDO history[MAXGAMEMOVE];

    int pList[13][10]; // hold the pieces lists for the move generation

} S_BOARD;

/*
0000 0000 0000 0000 0000 0111 1111 -> From 0x7f
0000 0000 0000 0011 1111 1000 0000 -> to >>7 0x7f
0000 0000 0011 1100 0000 0000 0000 -> capture >>14 0xf
0000 0000 0100 0000 0000 0000 0000 -> Enpas 0x40000
0000 0000 1000 0000 0000 0000 0000 -> pawn start 0x80000
0000 1111 0000 0000 0000 0000 0000 -> Promoted piece >>20 0xf
0001 0000 0000 0000 0000 0000 0000 -> Castle 0x1000000
*/

// Macros
#define FROMSQ(m) ((m) & 0x7f)
#define TOSQ(m) (((m) >> 7) & 0x7f)
#define CAPTURED(m) (((m) >> 14) & 0xf)
#define PROMOTED(m) (((m) >> 20) & 0xf)
#define MFLAGEP 0x40000
#define MFLAGPS 0x80000
#define MFLAGCA 0x1000000
#define MFLAGCAP 0x7c000
#define MFLAGPROM 0xf00000

#define FR2SQ(f, r) ((21 + (f)) + ((r) * 10)) // converts files and rank into 120based sq board index
#define SQ64(sq120) (Sq120ToSq64[(sq120)])    // converts 120sq board index into 64 sq
#define SQ120(sq64) (Sq64ToSq120[(sq64)])     // converts 64sq board index into 120 sq
#define POP(b) PopBit(b)                      // calls a functions removes the least significant bit from the bit board
#define COUNT(b) CountBit(b)                  // calls a function to count the number of bits in the bit board
#define CLRBIT(bb, sq) ((bb) &= ClearMask[(sq)])
#define SETBIT(bb, sq) ((bb) |= SetMask[(sq)])
#define isBQ(p) (PieceBishopQueen[(p)])
#define isRQ(p) (PieceRookQueen[(p)])
#define isKn(p) (PieceKnight[(p)])
#define isKi(p) (PieceKing[(p)])

// Globals

extern int Sq120ToSq64[BRD_SQ_NUM];
extern int Sq64ToSq120[64];
extern U64 SetMask[64];
extern U64 ClearMask[64];
extern U64 PieceKeys[13][120]; // 2d array holding has keys for different pieces at each of the 120 sq
extern U64 SideKey;            // using ULL to represent the side to move
extern U64 CastleKeys[16];     // array fo ULL holding the hash key for castling rights

extern char PceChar[];  // mapping array to piece types to characters
extern char SideChar[]; // mapping the side to character
extern char RankChar[]; // mapping rank numbers to characters (1 to 8)
extern char FileChar[]; // mapping file letters to characters(a to h)

extern int PieceBig[13]; // Array indicating whether each piece type is considered a "big" piece (e.g., queen, rook).
extern int PieceMaj[13]; // Array indicating whether each piece type is considered a "major" piece (e.g., queen, rook).
extern int PieceMin[13]; // Array indicating whether each piece type is considered a "minor" piece (e.g., bishop, knight).
extern int PieceVal[13]; // Array holding the value of the piece type (e.g., pawn=100).
extern int PieceCol[13]; // Array indicating the color of each piece type (e.g., WHITE ,BLACK).
extern int FilesBrd[BRD_SQ_NUM];
extern int RanksBrd[BRD_SQ_NUM];

extern int PieceKnight[13];
extern int PieceKing[13];
extern int PieceRookQueen[13];
extern int PieceBishopQueen[13];

// function
extern void AllInit();
extern void PrintBitBoard(U64 bb);
extern int PopBit(U64 *bb);
extern int CountBit(U64 b);

// Hashkey.c
extern U64 GeneratePosKeys(const S_BOARD *pos);

// board.c
extern void ResetBoard(S_BOARD *pos);
extern int ParseFen(char *fen, S_BOARD *pos);
extern void PrintBoard(const S_BOARD *pos);
extern void UpdateListMaterial(S_BOARD *pos);
extern int CheckBoard(const S_BOARD *pos);

// attack.c
extern int sqAttacked(const int sq, const int side, const S_BOARD *pos);

// test.c
extern void showAttacks(const int side, S_BOARD *pos);

// io.c
extern char *PrSq(const int sq);
extern char *PrMove(const int move);

// validate.c

extern int SqOnBoard(const int sq);
extern int SideValid(const int side);
extern int FileRankValid(const int fr);
extern int PieceValidEmpty(const int pce);
extern int PieceValid(const int pce);

#endif