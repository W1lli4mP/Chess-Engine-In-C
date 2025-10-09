import pygame
import copy

from config import SQUARE_SIZE

class Board:
    def __init__(self, screen, square_size=SQUARE_SIZE):
        self.screen = screen
        self.square_size = square_size

    def render(self, position):
        x, y = position
        for i in range(8):
            for j in range(8):
                # if the square's coordinates are even then let the colour be dark, else let it be light
                colour = "#CDDBE1" if (i + j) % 2 == 0 else "#386F88"
                pygame.draw.rect(self.screen, colour, (x + i * self.square_size, y + j * self.square_size, self.square_size, self.square_size))