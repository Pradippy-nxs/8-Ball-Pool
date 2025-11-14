#include "Game.h"
#include <SFML/Graphics.hpp>

void Game::render_window(){
    // Create the main window
    sf::RenderWindow window(sf::VideoMode({1280,720}), "SFML window");
 
    // Load a sprite to display
    const sf::Texture texture("asset/table.png");
    sf::Sprite sprite(texture);

    // Get the window size and texture size
    sf::Vector2u window_size = window.getSize();
    sf::Vector2u texture_size = texture.getSize();

    // Get the scale for texture : window ratio
    // Only need to cast one int to float to return a float
    float scale_x = static_cast<float>(window_size.x) / texture_size.x;
    float scale_y = static_cast<float>(window_size.y) / texture_size.y;
 
    // Resizing the texture to fit the window
    sprite.setScale({scale_x,scale_y});

    // Create a graphical text to display
    // const sf::Font font("arial.ttf");
    // sf::Text text(font, "Hello SFML", 50);
 
    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        while (const std::optional event = window.pollEvent())
        {
            // Close window: exit
            if (event->is<sf::Event::Closed>())
                window.close();
        }
 
        // Clear screen
        window.clear();
 
        // Draw the sprite
        window.draw(sprite);
 
        // Draw the string
        // window.draw(text);
 
        // Update the window
        window.display();
    }
};