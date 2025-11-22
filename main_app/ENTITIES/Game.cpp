#include "Game.h"
#include <iostream>
#include <cmath>
// Create the main window using constructor initialization
Game::Game() : window(sf::VideoMode({1280, 720}), "SFML window")
{
}

sf::Vector2u Game::get_window_size()
{
    return this->window.getSize();
}
void Game::render_window()
{

    // Load a sprite to display
    const sf::Texture table_texture("asset/table.png");
    sf::Sprite table_sprite(table_texture);

    // Get the window size and texture size
    sf::Vector2u table_size = table_texture.getSize();

    // Get the scale for texture : window ratio
    // Only need to cast one int to float to return a float
    float scale_x = static_cast<float>(get_window_size().x) / table_size.x;
    float scale_y = static_cast<float>(get_window_size().y) / table_size.y;

    // Resizing the texture to fit the window
    table_sprite.setScale({scale_x, scale_y});

    // Entity rendering
    // Balls
    std::vector<sf::Texture> solid_ball_textures;
    std::vector<sf::Sprite> solid_ball_sprites;
    std::vector<sf::Texture> stripe_ball_textures;
    std::vector<sf::Sprite> stripe_ball_sprites;

    // Cue ball
    sf::Texture cue_ball_texture("asset/cue_ball.png");
    sf::Sprite cue_ball_sprite(cue_ball_texture);

    // Cue
    sf::Texture cue_texture("asset/cue.png");
    sf::Sprite cue_sprite(cue_texture);

    // Triangle
    sf::Texture triangle_texture("asset/triangle.png");
    sf::Sprite triangle_sprite(triangle_texture);

    // Input & rotation variables
    bool grabbing_cue_ball = false;
    bool right_click_held = false;
    sf::Vector2f grabOffset;

    sf::Vector2f last_mouse_pos;
    float cue_angle = 0.f;
    float rotateSensitivity = 0.4f;

    // Set cue rotation origin to right end
    cue_sprite.setOrigin({static_cast<float>(cue_texture.getSize().x) + 20, cue_texture.getSize().y / 2.f});

    // Getting the new origin
    sf::Vector2f cue_tip = cue_sprite.getTransform().transformPoint(
        cue_sprite.getOrigin());

    // Main loop
    while (window.isOpen())
    {
        // Events go here
        while (const std::optional<sf::Event> event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            // Grab cue ball
            if (const auto *m = event->getIf<sf::Event::MouseButtonPressed>();
                m && m->button == sf::Mouse::Button::Left)
            {
                sf::Vector2f mouse_pos((float)m->position.x, (float)m->position.y);

                if (cue_ball_sprite.getGlobalBounds().contains(mouse_pos))
                {
                    grabbing_cue_ball = true;
                    grabOffset = cue_ball_sprite.getPosition() - mouse_pos;
                }
            }

            // Release cue ball
            if (const auto *m = event->getIf<sf::Event::MouseButtonReleased>();
                m && m->button == sf::Mouse::Button::Left)
            {
                grabbing_cue_ball = false;
            }

            // Start rotating cue
            if (const auto *m = event->getIf<sf::Event::MouseButtonPressed>();
                m && m->button == sf::Mouse::Button::Right)
            {
                sf::Vector2f mouse_pos((float)m->position.x, (float)m->position.y);
                right_click_held = true;
                last_mouse_pos = mouse_pos;
            }

            // Stop rotating cue
            if (const auto *m = event->getIf<sf::Event::MouseButtonReleased>();
                m && m->button == sf::Mouse::Button::Right)
            {
                right_click_held = false;
            }
        }

        // Frame updates
        // Mouse position (in window coords)
        sf::Vector2i mp = sf::Mouse::getPosition(window);
        sf::Vector2f mouse_pos((float)mp.x, (float)mp.y);

        // Update cue ball
        if (grabbing_cue_ball)
            cue_ball_sprite.setPosition(mouse_pos + grabOffset);

        // Get the necessary positions and sizes
        sf::Vector2f cue_ball_pos = cue_ball_sprite.getPosition();
        float cue_ball_radius = cue_ball_texture.getSize().x * 0.5f;

        // Vector calculation stuff (hands off on this part)
        sf::Vector2f diff_to_cursor = mouse_pos - cue_ball_pos;
        float distance_to_cursor = std::sqrt(diff_to_cursor.x * diff_to_cursor.x + diff_to_cursor.y * diff_to_cursor.y);

        // Calculate the angle from the center of the ball to the cursor
        float angle_radians = std::atan2(diff_to_cursor.y, diff_to_cursor.x);
        float angle_degrees = angle_radians * 180.f / M_PI;

        // Set rotation
        cue_sprite.setRotation(sf::degrees(angle_degrees + 180.f));

        // Positioning the cue
        sf::Vector2f direction_vector_from_ball = diff_to_cursor;
        if (distance_to_cursor != 0.f)
            direction_vector_from_ball /= distance_to_cursor;

        // The sprite's position is the location of its origin point.
        sf::Vector2f origin_pos = cue_ball_pos + (direction_vector_from_ball * distance_to_cursor);
        cue_sprite.setPosition(origin_pos);

        // Recalculate the actual tip position after the sprite has been rotated and positioned
        sf::Vector2f cue_tip =
            cue_sprite.getTransform().transformPoint(cue_sprite.getOrigin());

        // Recalculate the distance and vector from the cue tip to the cue ball center
        sf::Vector2f diff = cue_ball_pos - cue_tip;
        float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        float distance_threshold = cue_ball_radius;

        if (distance < distance_threshold)
        {

            if (distance != 0.f)
                diff /= distance; 

            float overlap = distance_threshold - distance;

        
            cue_sprite.move(-diff * overlap);
        }

        // Update last mouse position for rotation tracking
        last_mouse_pos = mouse_pos;

        // Render everything
        window.clear();
        window.draw(table_sprite);
        window.draw(cue_ball_sprite);
        window.draw(cue_sprite);
        window.display();
    }
};