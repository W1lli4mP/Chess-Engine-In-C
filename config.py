# === Board & Layout ===
SQUARE_SIZE = 100
WIDTH, HEIGHT = 800, 800


def board_to_screen(position):
    screen_X = position[0] * SQUARE_SIZE
    screen_Y = position[1] * SQUARE_SIZE
    return (screen_X, screen_Y)

def screen_board(position):
    board_X = position[0] // SQUARE_SIZE
    board_Y = position[1] // SQUARE_SIZE
    return (board_X, board_Y)