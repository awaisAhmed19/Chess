#include <stdio.h>
#include "defs.h"
#include <stdlib.h>

void printBinary(unsigned int value)
{
    unsigned int mask = 1 << (sizeof(value) * 8 - 1); // Create a mask for the most significant bit

    for (int i = 0; i < sizeof(value) * 8; i++)
    {
        putchar(value & mask ? '1' : '0'); // Print '1' if the bit is set, otherwise '0'
        mask >>= 1;                        // Shift mask to the right
    }
    putchar('\n'); // Print a newline character at the end
}
int main(void)
{
    AllInit();
    int al = rand();
    int am = rand();
    int an = rand();
    int ao = rand();

    printf("al=%X\n", al);
    printf("am=%X\n", am);
    printf("an=%X\n", am);
    printf("ao=%X\n", ao);
    unsigned int key = al ^ am ^ an ^ ao;
    printf("%d\n", key);
    unsigned int temp = al;
    temp ^= am;
    temp ^= an;
    temp ^= ao;
    printf("%d\n", temp);
    printBinary(key);
    printBinary(temp);
    return 0;
}