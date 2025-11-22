#pragma once

#include <SFML/Graphics.hpp>

class Game{
    public:
    sf::RenderWindow window;

    public:
    // Declare app window method
    Game();
    void render_window();
    sf::Vector2u get_window_size();
};