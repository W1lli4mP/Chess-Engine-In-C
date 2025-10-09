import pygame
from config import SQUARE_SIZE, board_to_screen

class Piece:
    def __init__(self, piece, position, sprite): # piece is a 2-character string
        self.colour = piece[0]
        self.type = piece[1]
        self.position = (position[0], position[1])
        self.moved = False
        self.sprite = pygame.transform.scale(sprite, (SQUARE_SIZE, SQUARE_SIZE)) # scales to square

    def render(self, screen):
        position = board_to_screen(self.position)
        screen.blit(self.sprite, position)