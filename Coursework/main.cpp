#include "Game.hpp"

int main() {
    srand(time(NULL));
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "ARKANOID");

    sf::Font font;
    font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

    Board b(window, font);
    Game game(b);
    game.Play();

    return 0;
}