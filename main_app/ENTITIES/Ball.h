#pragma once
#include <vector>
#include <string>
#include "SFML/Graphics.hpp"

// Ball object
struct BallObject
{
    sf::Sprite sprite;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    float radius;
    float mass;
    int id; // 0 = cue, 1-7 solids, 8 black, 9-15 stripes

    bool isPotted = false;
    bool isInPocket = false;

    // Construct from texture
    BallObject(sf::Texture &texture) : sprite(texture)
    {
        // init
        radius = texture.getSize().x / 2.0f;
        sprite.setOrigin({radius, radius});
        velocity = {0.f, 0.f};
        acceleration = {0.f, 0.f};
        mass = 1.0f;
    }

    sf::Vector2f getPosition() const
    {
        return sprite.getPosition();
    }
};

class Balls
{
private:
    std::vector<sf::Texture *> solid_textures;
    std::vector<sf::Texture *> striped_textures;
    sf::Texture cue_texture;

    // store BallObjects
    std::vector<BallObject *> solid_balls;
    std::vector<BallObject *> striped_balls;

    // Cue ball
    BallObject *cue_ball;

public:
    Balls();
    ~Balls();

    // Getters
    std::vector<BallObject *> getSolids();
    std::vector<BallObject *> getStripes();

    BallObject *getCueBall();
    std::vector<BallObject *> getAllBalls()
    {
        std::vector<BallObject *> all;
        all.push_back(cue_ball);
        all.insert(all.end(), solid_balls.begin(), solid_balls.end());
        all.insert(all.end(), striped_balls.begin(), striped_balls.end());
        return all;
    }
};