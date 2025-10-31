#include <stdio.h>
#include "piece.h"

int generate_pseudo_legal_moves(Board *b, Piece *p, Position *moves) { // p is the selected piece
    int capacity = 20;
    if (p->type == 'p')
        return generate_pawn_moves(b, p, moves, capacity);
    // if (p->type == 'r' || p->type == 'b' || p->type == 'q')
    //     return generate_sliding_moves();
    // if (p->type == 'n')
    //     return generate_knight_moves();
    // if (p->type == 'k')
    //     return generate_king_moves();

}

int generate_pawn_moves(Board *b, Piece *p, Position *moves, int capacity) {
    int count = 0;
    int d = (p->colour == 'w') ? 1 : -1; // inverse direction for black

    // checking for captures
    Position forward = {p->position[0] + d, p->position[1]};
    Position c_left = {forward.row, p->position[1] - 1};
    Position c_right = {forward.row, p->position[1] + 1};

    // checking diagonal captures
    if (in_bounds(c_left.row, c_left.col) && is_enemy(b, p, c_left)) {
        if (count < capacity) moves[count] = init_position(c_left.row, c_left.col);
        count++;
    }
    if (in_bounds(c_right.row, c_right.col) && is_enemy(b, p, c_right)) {
        if (count < capacity) moves[count] = init_position(c_right.row, c_right.col);
        count++;
    }

    // checking forward
    int for_arr[2] = {forward.row, forward.col};
    Piece *check_piece = get_piece_at(b, for_arr);
    if (in_bounds(forward.row, forward.col) && check_piece->type == '\0') { // checking if the square is empty
        if (count < capacity) moves[count] = init_position(forward.row, forward.col);
        count++;
        
        // TODO: add initial square conditions in the future
        Position double_forward = {forward.row + d, forward.col};
        int for_arr2[2] = {double_forward.row, double_forward.col};
        Piece *check_piece2 = get_piece_at(b, for_arr2);
        if (in_bounds(double_forward.row, double_forward.col) && check_piece2->type == '\0') { // checking if the square after initial square is empty
            if (count < capacity) moves[count] = init_position(double_forward.row, double_forward.col);
            count++;
        }
    }
    return count;
    // TODO: add en passant and promotion logic in the future
}

int is_enemy(Board *board, Piece *piece, Position destination) {
    int raw_destination[2] = {destination.row, destination.col};
    Piece *enemy_piece = get_piece_at(board, raw_destination);
    if (enemy_piece->type == '\0') return 0;
    return (piece->colour != enemy_piece->colour);
}

int in_bounds(int row, int col) {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}