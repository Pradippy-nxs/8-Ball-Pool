#include "Game.h"
#include <SFML/Graphics.hpp>
#include <iostream>

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
 
    // Ball rendering
    std::vector<sf::Texture*> ball_textures;
    std::vector<sf::Sprite*> ball_sprites;

    for (int i = 0; i < 16; i++)
    {   
        // Getting asset file names
        std::string filename = "asset/ball_" + std::to_string(i+1) + ".png";
        // std::cout<<filename<<std::endl;

        // Making and storing texture
        sf::Texture* texture_ptr = new sf::Texture(filename);
        ball_textures.push_back(texture_ptr);
        // std::cout<<"TEXTURES LOADED"<<std::endl;

        // Making and storing sprites
        sf::Sprite* sprite_ptr = new sf::Sprite(*texture_ptr);
        sprite_ptr->setScale({scale_x,scale_y});
        ball_sprites.push_back(sprite_ptr);
        // std::cout<<"SPRITES LOADED"<<std::endl;

    }
    

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
        
        // Drawing balls
        float initial_pos_x = 60;
        float initial_pos_y = 360; 
        for (sf::Sprite* ball_sprite : ball_sprites){
            ball_sprite->setPosition({initial_pos_x, initial_pos_y});
            window.draw(*ball_sprite);
            initial_pos_x += 50;
        }
        // Update the window
        window.display();
    }
};