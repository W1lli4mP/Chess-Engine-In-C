#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "move_gen.h"

#define MOVE_GEN_CASES_FILE "tests/data/move_gen_cases.txt"
#define MAX_LINE_LEN 256

static bool run_move_gen_case(
    const char *test_id,
    const char *fen,
    const char *from,
    const char *expected_count,
    const char *expected_moves
);

int main()
{
    puts("------------------------");

    FILE *fp = fopen(MOVE_GEN_CASES_FILE, "r");
    if (!fp)
    {
        perror("Failed to open Move Generation test file");
        return 1;
    }

    int passed = 0;
    int total = 0;

    char line[MAX_LINE_LEN];

    while (fgets(line, sizeof line, fp))
    {
        size_t len = strcspn(line, "\r\n");
        line[len] = '\0';

        // skip blank lines and comments
        if (line[0] == '\0' || line[0] == '#') continue;

        char *test_id = line;

        // 1st separator
        char *sep1 = strchr(line, '|');

        if (!sep1)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep1 = '\0';

        char *fen = sep1 + 1;

        // 2nd separator
        char *sep2 = strchr(fen, '|');

        if (!sep2)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep2 = '\0';

        char *from = sep2 + 1;

        // 3rd separator
        char *sep3 = strchr(from, '|');

        if (!sep3)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep3 = '\0';

        char *expected_count = sep3 + 1;

        // 4th separator
        char *sep4 = strchr(expected_count, '|');

        if (!sep4)
        {
            printf("Malformed test line: %s\n", line);
            continue;
        }

        *sep4 = '\0';

        char *expected_moves = sep4 + 1;
        
        // process test case
        total++;

        if (run_move_gen_case(test_id, fen, from, expected_count, expected_moves)) passed++;
    }

    fclose(fp);

    printf("Passed %d/%d tests\n", passed, total);
    
    return passed != total;
}

//* HELPER
static bool run_move_gen_case(
    const char *test_id,
    const char *fen,
    const char *from,
    const char *expected_count,
    const char *expected_moves
)
{
    //! complete
}