#include "draw_rules.h"

bool is_fifty_move_draw(const GameState *game);

bool is_insufficient_material(const GameState *game);

bool is_threefold_repetition(const GameState *game);

bool is_draw(const GameState *game);