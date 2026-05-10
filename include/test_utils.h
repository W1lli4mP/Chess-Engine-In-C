#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdbool.h>

bool split_test_line(
    char *line,
    char *fields[],
    int expected_fields,
    const char **error_out
);

#endif