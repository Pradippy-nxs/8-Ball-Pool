#pragma once
#include <vector>
#include <SFML/Graphics.hpp>

struct BallObject;

class Cue
{
private:
    sf::Texture cueTexture;
    sf::Sprite cueSprite;

    // Physics & visuals
    sf::CircleShape tip;
    sf::RectangleShape aimStick;
    float aimAngle;

    sf::Vector2f lastPosition;
    sf::Vector2f currentVelocity;

    bool isShooting;
    bool isAiming;

    // Anchors
    sf::Vector2f shootTipOrigin;
    sf::Vector2f shootMouseOrigin;
    sf::Vector2f shootDir;

    // Pullback
    float currentPullback;

    sf::Vector2f aimTipOrigin;
    sf::Vector2f aimMouseOrigin;
    float initialAimAngle;

public:
    Cue();

    void update(sf::RenderWindow &window, sf::Vector2f mousePos, float dt, bool wantToShoot, bool wantToAim);
    void render(sf::RenderWindow &window);
    bool checkCollisions(std::vector<BallObject *> &balls);
};