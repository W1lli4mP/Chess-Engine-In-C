#include <stdio.h>
#include "piece.h"

// returns number of moves
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
    if (in_bounds(forward.row, forward.col)) {
        Piece *check_piece = get_piece_at(b, forward);
        if (in_bounds(forward.row, forward.col) && check_piece->type == '\0') { // checking if the square is empty
            if (count < capacity) moves[count] = init_position(forward.row, forward.col);
            count++;
            
            // checking double forward
            if ((p->position[0] == 1 && d == 1) || (p->position[0] == 6 && d == -1)) {
                Position double_forward = {forward.row + d, forward.col};
                if (in_bounds(double_forward.row, double_forward.col)) {
                    Piece *check_piece2 = get_piece_at(b, double_forward);
                    if (check_piece2->type == '\0') { // checking if the square after initial square is empty
                        if (count < capacity) moves[count] = init_position(double_forward.row, double_forward.col);
                        count++;
                    }
                }
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
        {-2, 1}, {-2, -1}, {-1, -2}, {1, -2}
    };

    for (int i = 0; i < 8; i++) {
        int row = p->position[0] + var[i][0];
        int col = p->position[1] + var[i][1];
        Position pos = {row, col};
        if (!in_bounds(row, col)) continue;
        Piece *target = get_piece_at(b, pos);
        if (is_enemy(b, p, pos) || target->type == '\0') {
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
        if (!in_bounds(row, col)) continue;
        Piece *target = get_piece_at(b, pos);
        if (is_enemy(b, p, pos) || target->type == '\0') {
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
        int dr = directions[i][0];
        int dc = directions[i][1];
        int r = p->position[0] + dr;
        int c = p->position[1] + dc;

        while (in_bounds(r, c)) {
            Position t = {r, c}; // target square
            Piece *target = get_piece_at(b, t);
            if (target->type == '\0') {
                if (count >= capacity) break;
                moves[count++] = t;
            } else {
                if (is_enemy(b, p, t)) {
                    if (count < capacity) moves[count] = t;
                    count++;
                }
                break; // stop sliding when blocked by any piece
            }
            
            r += dr;
            c += dc;
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
    Piece *enemy_piece = get_piece_at(board, destination);
    if (enemy_piece->type == '\0') return 0;
    return (piece->colour != enemy_piece->colour);
}

int in_bounds(int row, int col) {
    return row >= 0 && row < 8 && col >= 0 && col < 8;
}