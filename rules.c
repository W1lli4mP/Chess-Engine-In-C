#include "rules.h"

int get_king_position(Board *b, char colour, Position *out) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            Piece *p = &b->grid[i][j];
            if (p->type == 'k' && p->colour == colour) {
                out->row = i;
                out->col = j;
                return 1;
            }
        }
    }
    return 0;
}