import pygame
WIDTH, HEIGHT = 500, 500

class Game:
    def __init__(self, screen):
        self.screen = screen
        self.running = True
    
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False

def main():
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    game = Game(screen)
    while game.running:
        game.handle_events()
    pygame.quit()

main()