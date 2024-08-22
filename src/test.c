#include "defs.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define ERROR_LOG_FILE "error_log.txt"

void log_error(const char *message)
{
    FILE *log_file = fopen(ERROR_LOG_FILE, "a");
    if (log_file != NULL)
    {
        fprintf(log_file, "%s\n", message);
        fclose(log_file);
    }
}

void showAttacks(const int side, S_BOARD *pos)
{
    for (int rank = RANK_8; rank >= RANK_1; --rank)
    {
        for (int file = FILE_A; file <= FILE_H; ++file)
        {
            int sq = FR2SQ(file, rank);
            if (sqAttacked(sq, side, pos) == TRUE)
            {
                printf(" x ");
            }
            else
            {
                printf(" - ");
            }
        }
        printf("\n");
    }
    printf("\n\n");
}

void getFen(char *filename, FENDATA *data)
{
    char buffer[256];
    int index = 0;
    FILE *file;

    file = fopen(filename, "r");

    if (file == NULL)
    {
        fprintf(stderr, "Error opening file '%s': %s\n", filename, strerror(errno));
        exit(1);
    }

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        char fen_part[FEN_LENGTH];
        long long int d4 = 0, d5 = 0, d6 = 0;
        sscanf(buffer, "%99[^;]", fen_part);

        // Print the parsed values
        // printf("Parsed FEN: '%s', D4: %ld, D5: %ld\n", fen_part, d4, d5);
        strncpy(data->fen[index], fen_part, sizeof(data->fen[index]));
        data->fen[index][sizeof(data->fen[index]) - 1] = '\0';

        char *token = strtok(buffer, ";");

        while ((token = strtok(NULL, ";")) != NULL)
        {
            if (strstr(token, "D4") != NULL)
            {
                sscanf(token, "D4 %ld", &d4);
                data->D4[index] = d4;
            }
            else if (strstr(token, "D5") != NULL)
            {
                sscanf(token, "D5 %ld", &d5);
                data->D5[index] = d5;
            }
            else if (strstr(token, "D6") != NULL)
            {
                sscanf(token, "D6 %ld", &d6);
                data->D6[index] = d6;
            }
        }
        index++;
    }
    data->count = index;
    fclose(file);
}

void accuracyTest(FENDATA *data, S_BOARD *pos)
{

    int count = 0;
    int correct = 0;
    int incorrect = 0;
    if (pos == NULL)
    {
        fprintf(stderr, "Error: Null pointer passed to accuracyTest.\n");
        return;
    }

    while (count < data->count)
    {
        AllInit();
        ParseFen(data->fen[count], pos);
        printf("%d d4: %ld\n ", count + 1, data->D4[count]);

        if (PerftTest(5, pos) == data->D5[count])
        {
            correct++;
            printf("Correct: %d count: %d\n", correct, data->count);
        }
        else
        {
            incorrect++;
            printf("--------------incorrect:%d-------------", incorrect);
        }
        count++;
    }
    printf("Data count: %d\n", data->count);

    float total = (float)correct / data->count;
    printf("total accuracy %f %", total * 100);
}