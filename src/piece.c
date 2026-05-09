#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "piece.h"

// helpers for validating attribute inputs
static int valid_type(PieceType type);
static int valid_colour(Colour colour);

// initialise existing memory
Piece *init_piece(Piece *piece, PieceType type, Colour colour)
{
    if (!piece) return false;

    const char *sprite = find_sprite(type, colour);

    if (!valid_type(type)) return false;
    if (!valid_colour(colour)) return false;
    if (strcmp(sprite, SPRITE_NONE) == 0) return false;

    piece->type = type;
    piece->colour = colour;
    piece->sprite = sprite;

    return true;
}

// allocate + initialise
Piece *create_piece(PieceType type, Colour colour)
{

    Piece *piece = malloc(sizeof *piece);
    if (!piece) return NULL;

    if (!init_piece(piece, type, colour))
    {
        free(piece);
        return NULL;
    }

    return piece;
}

// free piece
void destroy_piece(Piece *piece)
{
    free(piece);
}

// helpers for validating piece attributes
static int valid_type(PieceType type)
{
    return type >= TYPE_NONE && type <= TYPE_KING;
}

static int valid_colour(Colour colour)
{
    return colour >= COLOUR_NONE && colour <= COLOUR_BLACK;
}

const char *find_sprite(PieceType type, Colour colour)
{
    if (colour == COLOUR_WHITE || colour == COLOUR_BLACK)
    {
        switch (type)
        {
            case TYPE_PAWN: return (colour == COLOUR_WHITE) ? SPRITE_WHITE_PAWN : SPRITE_BLACK_PAWN;
            case TYPE_ROOK: return (colour == COLOUR_WHITE) ? SPRITE_WHITE_ROOK : SPRITE_BLACK_ROOK;
            case TYPE_KNIGHT: return (colour == COLOUR_WHITE) ? SPRITE_WHITE_KNIGHT : SPRITE_BLACK_KNIGHT;
            case TYPE_BISHOP: return (colour == COLOUR_WHITE) ? SPRITE_WHITE_BISHOP : SPRITE_BLACK_BISHOP;
            case TYPE_QUEEN: return (colour == COLOUR_WHITE) ? SPRITE_WHITE_QUEEN : SPRITE_BLACK_QUEEN;
            case TYPE_KING: return (colour == COLOUR_WHITE) ? SPRITE_WHITE_KING : SPRITE_BLACK_KING;
            default: return SPRITE_NONE;
        }
    }
    else return SPRITE_NONE;
}