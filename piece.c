#include <stdio.h>
#include "piece.h"

int generate_pseudo_legal_moves(Board *b, Piece *p, Position *moves) { // p is the selected piece
    int capacity = 20;
    if (p->type == 'p')
        return generate_pawn_moves(b, p, moves, capacity);
    if (p->type == 'n')
        return generate_knight_moves(b, p, moves, capacity);
    if (p->type == 'k')
        return generate_king_moves(b, p, moves, capacity);
    if (p->type == 'r')
        return generate_rook_moves(b, p, moves, capacity);
    if (p->type == 'b')
        return generate_bishop_moves(b, p, moves, capacity);
    if (p->type == 'q')
        return generate_queen_moves(b, p, moves, capacity);

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
        
        // checking double forward
        if ((p->position[0] == 1 && d == 1) || (p->position[0] == 6 && d == -1)) {
            Position double_forward = {forward.row + d, forward.col};
            int for_arr2[2] = {double_forward.row, double_forward.col};
            Piece *check_piece2 = get_piece_at(b, for_arr2);
            if (in_bounds(double_forward.row, double_forward.col) && check_piece2->type == '\0') { // checking if the square after initial square is empty
                if (count < capacity) moves[count] = init_position(double_forward.row, double_forward.col);
                count++;
            }
        }
    }
    return count;
    // TODO: add en passant and promotion logic in the future
}

int generate_knight_moves(Board *b, Piece *p, Position *moves, int capacity) {
    int count = 0;
    // create all knight move variations
    int var[8][2] = {
        {2, -1}, {2, 1}, {1, 2}, {-1, 2},
        {-2, 1}, {-2, 1}, {-1, -2}, {1, -2}
    };

    for (int i = 0; i < 8; i++) {
        int row = p->position[0] + var[i][0];
        int col = p->position[1] + var[i][1];
        Position pos = {row, col};
        Piece *target = get_piece_at(b, (int[2]){row, col});
        if (in_bounds(row, col) && (is_enemy(b, p, pos) || target->type == '\0')) {
            if (count >= capacity) break;
            moves[count] = pos;
            count++;
        }
    }
    return count;

}

int generate_king_moves(Board *b, Piece *p, Position *moves, int capacity) {
    int count = 0;
    // create all knight move variations
    int var[8][2] = {
        {1, 0}, {1, 1}, {0, 1}, {-1, 1},
        {-1, 0}, {-1, -1}, {0, -1}, {1, -1}
    };

    for (int i = 0; i < 8; i++) {
        int row = p->position[0] + var[i][0];
        int col = p->position[1] + var[i][1];
        Position pos = {row, col};
        Piece *target = get_piece_at(b, (int[2]){row, col});
        if (in_bounds(row, col) && (is_enemy(b, p, pos) || target->type == '\0')) {
            if (count >= capacity) break;
            moves[count] = pos;
            count++;
        }
    }
    return count;
}

int generate_sliding_moves(Board *b, Piece *p, Position *moves, int capacity, int directions[][2], int num_d) {
    int count = 0;
    for (int i = 0; i < num_d; i++) {
        Position t = {p->position[0] + directions[i][0], p->position[1] + directions[i][1]}; // target square
        Piece *target = get_piece_at(b, (int[2]){t.row, t.col}); 
        while (in_bounds(t.row, t.col) && target->type == '\0') {
            if (count >= capacity) break;
            moves[count] = t;
            count++;

            t.row += directions[i][0];
            t.col += directions[i][1];
            target = get_piece_at(b, (int[2]){t.row, t.col}); // switch to next potential target
        }
    }
    return count;
}

int generate_bishop_moves(Board *b, Piece *p, Position *moves, int capacity) {
    int dir[4][2] = {
        {1, 1}, {-1, 1},
        {-1, -1}, {1, -1}
    };
    return generate_sliding_moves(b, p, moves, capacity, dir, 4);
}

int generate_rook_moves(Board *b, Piece *p, Position *moves, int capacity) {
    int dir[4][2] = {
        {1, 0}, {0, 1},
        {-1, 0}, {0, -1}
    };
    return generate_sliding_moves(b, p, moves, capacity, dir, 4);
}
int generate_queen_moves(Board *b, Piece *p, Position *moves, int capacity) {
    int dir[8][2] = {
            {1, 1}, {-1, 1}, {-1, -1}, {1, -1},
            {1, 0}, {0, 1}, {-1, 0}, {0, -1}
    };
    return generate_sliding_moves(b, p, moves, capacity, dir, 8);
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