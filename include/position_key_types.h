#ifndef POSITION_KEY_TYPES_H
#define POSITION_KEY_TYPES_H

#define POSITION_KEY_LEN 128
#define MAX_POSITION_HISTORY 1024

// compact form of a chess position to be used as a "lookup key"
// useful for threefold and fetching transpositions
typedef struct
{
    char text[POSITION_KEY_LEN];
} PositionKey;

#endif