#ifndef PIECE_H // define if not defined yet - include guard
#define PIECE_H

typedef struct {
    char colour;
    char type;
    int position[2];
    const char *sprite;
} Piece;

#endif