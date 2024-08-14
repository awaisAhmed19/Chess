#include "stdio.h"
#include "defs.h"

const int BitTable[64] = {
    63, 30, 3, 32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34,
    61, 29, 2, 51, 21, 43, 45, 10, 18, 47, 1, 54, 9, 57, 0, 35,
    62, 31, 40, 4, 49, 5, 52, 26, 60, 6, 23, 44, 46, 27, 56, 16,
    7, 39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

/*
PopBit function takes a `unsigned long long` pointer to a bitboard, which is initialized to 64 bits of zeros. Its main operation is to pop the first instance of a `1` bit found in the bitboard and clear the bitboard off. It does this by performing the following operations:

1: `U64 b = *bb ^ (*bb - 1);`
   - Here, we take a `U64` type variable `b` and perform a XOR (`^`) operation between the bitboard (`*bb`) and `*bb - 1`.
   - What `*bb - 1` does is that it takes the bitboard value, for example:
     ```
     00000000 00000000 00000000 00000000 00000000 00000000 10000000 00000000
     ```
     and subtracts `1`, resulting in:
     ```
     (*bb - 1) = 00000000 00000000 00000000 00000000 00000000 00000000 01111111 11111111
     ```
   - Performing the XOR operation:
     ```
     *bb ^ (*bb - 1) =
     00000000 00000000 00000000 00000000 00000000 00000000 10000000 00000000 ^
     00000000 00000000 00000000 00000000 00000000 00000000 01111111 11111111
     ------------------------------------------------------------------------
     00000000 00000000 00000000 00000000 00000000 00000000 11111111 11111111
     ```
   - As we already know that XOR results in `0^0=0`, `1^0=1`, `0^1=1`, `1^1=0`, the result for `*bb ^ (*bb - 1)` is:
     ```
     b = 00000000 00000000 00000000 00000000 00000000 00000000 11111111 11111111
     ```

2: `unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));`
   - `fold = (unsigned)` <- typecasting the result to an unsigned integer.
   - `b & 0xffffffff` isolates the lower 32 bits of `b`:
     ```
     b & 0xffffffff =
     00000000 00000000 11111111 11111111 &
     00000000 00000000 11111111 11111111
     ------------------------------------
     00000000 00000000 11111111 11111111
     ```
   - `b >> 32` shifts the bits in `b` by 32 bits to the right, giving:
     ```
     b >> 32 = 00000000 00000000 00000000 00000000
     ```
   - Performing XOR on these two values:
     ```
     (b & 0xffffffff) ^ (b >> 32) =
     00000000 00000000 11111111 11111111 ^
     00000000 00000000 00000000 00000000
     ------------------------------------
     00000000 00000000 11111111 11111111
     ```
   - This value, called `fold`, is used to derive the index of the bit being popped from the bitboard, leveraging the properties of bit manipulation and bit folding.

3: `*bb &= (*bb - 1);` or `*bb = *bb & (*bb - 1);`
   - This is to clear the bitboard. The bitwise AND operation gives `1` only when `1 & 1 = 1`, but in this case:
     ```
     *bb  = 00000000 00000000 00000000 00000000 00000000 00000000 10000000 00000000 &
     *bb-1= 00000000 00000000 00000000 00000000 00000000 00000000 01111111 11111111
     -------------------------------------------------------------------------------
            00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
     ```
   - The bitboard is cleared:
     ```
     *bb = 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
     ```

4: `index = (fold * 0x783a9b23) >> 26;`
   - `0x783a9b23` is a special hexadecimal number used in chess programming and is standardly used to find the index of any bit on a bitboard.
   - `fold` is the 32-bit unsigned integer result from the previous operation.
   - `0x783a9b23` is a hexadecimal constant, which is a specific 32-bit value chosen as part of the bitboard manipulation algorithm.
   - This value helps in creating a pseudo-random distribution of indices when multiplied by `fold`.

*/

int PopBit(U64 *bb)
{
    U64 b = *bb ^ (*bb - 1);
    unsigned int fold = (unsigned)((b & 0xffffffff) ^ (b >> 32));
    *bb &= (*bb - 1);
    int index = (fold * 0x783a9b23) >> 26;
    return BitTable[index];
}

int CountBit(U64 b)
{
    int r;
    for (r = 0; b; r++, b &= b - 1)
        ;
    return r;
}

void PrintBitBoard(U64 bb)
{
    U64 shiftMe = 1ULL;

    int rank = 0;
    int file = 0;
    int sq = 0;
    int sq64 = 0;

    printf("\n");
    for (rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (file = 0; file <= FILE_H; ++file)
        {
            sq = FR2SQ(file, rank);
            sq64 = SQ64(sq);
            if ((shiftMe << sq64) & bb)
            {
                printf("X");
            }
            else
            {
                printf("-");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}