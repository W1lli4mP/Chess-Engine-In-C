#ifndef PIECE_H
#define PIECE_H

#include <string.h>
#include <stdbool.h>

// sprite types
#define SPRITE_NONE " "

#define SPRITE_WHITE_PAWN "♙"
#define SPRITE_BLACK_PAWN "♟"

#define SPRITE_WHITE_ROOK  "♖"
#define SPRITE_BLACK_ROOK  "♜"

#define SPRITE_WHITE_KNIGHT "♘"
#define SPRITE_BLACK_KNIGHT "♞"

#define SPRITE_WHITE_BISHOP "♗"
#define SPRITE_BLACK_BISHOP "♝"

#define SPRITE_WHITE_QUEEN  "♕"
#define SPRITE_BLACK_QUEEN  "♛"

#define SPRITE_WHITE_KING   "♔"
#define SPRITE_BLACK_KING   "♚"

// piece type types
typedef enum
{
    TYPE_NONE,
    TYPE_PAWN,
    TYPE_ROOK,
    TYPE_KNIGHT,
    TYPE_BISHOP,
    TYPE_QUEEN,
    TYPE_KING
} PieceType;

// colour types
typedef enum
{
    COLOUR_NONE,
    COLOUR_WHITE,
    COLOUR_BLACK
} Colour;

typedef struct
{
    PieceType type;
    Colour colour;
    const char *sprite;
} Piece;

bool init_piece(Piece *piece, PieceType type, Colour colour);
Piece *create_piece(PieceType type, Colour colour);
void destroy_piece(Piece *piece);

// helper
const char *find_sprite(PieceType type, Colour colour);

#endif