#include "piece.h"
#include <stdio.h>
#include <stdlib.h>

// helpers for validating attribute inputs
static int valid_type(PieceType type);
static int valid_colour(Colour colour);

Piece *initialise_piece()
{
    Piece *new_piece = malloc(sizeof *new_piece);
    if (!new_piece) return NULL;

    new_piece->type = TYPE_NONE;
    new_piece->colour = COLOUR_NONE;
    new_piece->sprite = SPRITE_NONE;
    return new_piece;
}

Piece *create_piece(PieceType type, Colour colour)
{
    const char *sprite = find_sprite(type, colour);

    if (!valid_type(type) || !valid_colour(colour) || strcmp(sprite, SPRITE_NONE) == 0) return NULL;

    Piece *new_piece = initialise_piece();
    if (!new_piece)
    {
        destroy_piece(new_piece);
        return NULL;
    }
    new_piece->type = type;
    new_piece->colour = colour;
    new_piece->sprite = sprite;
    return new_piece;
}

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

void print_piece(Piece *p)
{
    printf("%s", p->sprite);
}