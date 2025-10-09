import pygame
import subprocess
from config import WIDTH, HEIGHT
from board import Board
from piece import Piece

def get_board_state():
    out = subprocess.check_output(["./main"], text=True) # checks the output of the main C file as a string and not in bytes
    raw_text = out.split() # splits each square into a one-dimensional array of size 64
    rows, cols = 8, 8
    text = [raw_text[i * cols:(i + 1) * cols] for i in range(rows)]
    return text

class Game:
    def __init__(self, screen):
        self.screen = screen
        self.board = Board(screen)
        self.running = True

    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False

    def render(self):
        self.board.render((0, 0))

        # rendering pieces
        board_state = get_board_state() # array of all 64 squares
        for row in range(len(board_state)):
            for col in range(len(board_state[0])):
                current_piece = board_state[col][row]
                if current_piece != "--": # if not empty
                    sprite = pygame.image.load(f"Pieces/{current_piece}.png").convert_alpha()
                    piece = Piece(current_piece, (row, col), sprite)
                    piece.render(self.screen)

        pygame.display.flip()

def main():
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    game = Game(screen)
    while game.running:
        game.handle_events()
        game.render()
    pygame.quit()

main()