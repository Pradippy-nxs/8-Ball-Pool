#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

class Table
{
private:
    sf::Texture texture;
    sf::Sprite sprite;
    std::vector<sf::FloatRect> cushions;

    // Pockets
    std::vector<sf::Vector2f> pockets;

    // Constants
    const float CUSHION_WIDTH = 37.0f;
    const float POCKET_SIZE = 80.0f;

    // Pocket radius
    const float POCKET_RADIUS = 25.0f;

    sf::Vector2f tableOffset;
    sf::Vector2u gameAreaSize;

public:
    Table();
    void setup(sf::Vector2u windowSize);

    sf::Sprite &getSprite();
    const std::vector<sf::FloatRect> &getCushions();

    // Getters
    const std::vector<sf::Vector2f> &getPockets();
    float getPocketRadius() const { return POCKET_RADIUS; }
    sf::Vector2f getOffset() const { return tableOffset; }
    sf::Vector2u getGameAreaSize() const { return gameAreaSize; }
};