#include "test_utils.h"

bool split_test_line(
    char *line,
    char *fields[],
    int expected_fields,
    const char **error_out
)
{
    if (!line || !fields || expected_fields <= 0)
    {
        if (error_out) *error_out = "invalid split_test_line arguments";
        return false;
    }

    fields[0] = line;

    // track the field count
    int field_count = 1;

    // iterate through all chars in the line
    for (char *p = line; *p; p++)
    {
        if (*p == '|')
        {
            if (field_count >= expected_fields)
            {
                if (error_out) *error_out = "too many fields";
                return false;
            }

            // replace '|' with null terminators
            *p = '\0';
            fields[field_count] = p + 1;
            field_count++;
        }
    }

    // make sure field count stays consistent
    if (field_count != expected_fields)
    {
        if (error_out) *error_out = "wrong number of fields";
        return false;
    }

    return true;
}