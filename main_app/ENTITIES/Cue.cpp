#include <iostream>
#include <cmath>
#include "Cue.h"
#include "Ball.h"

Cue::Cue() : cueTexture("asset/cue.png"), cueSprite(cueTexture) {
    // ctor
    float r = 6.0f; 
    tip.setRadius(r);
    tip.setOrigin({r, r}); 
    tip.setFillColor(sf::Color::Transparent); 

    // texture
    if (cueTexture.loadFromFile("asset/cue.png")) {
        cueSprite.setTexture(cueTexture, true);
        sf::Vector2u size = cueTexture.getSize();
        cueSprite.setOrigin({static_cast<float>(size.x), size.y / 2.0f}); 
    }
    else {
        std::cout << "Note: asset/cue.png not found." << std::endl;
    }

    // visuals
    aimStick.setSize({300.f, 6.f}); 
    aimStick.setFillColor(sf::Color(100, 50, 0)); 
    aimStick.setOrigin({300.f, 3.f});

    aimAngle = 0.f;
    shootDir = {1.f, 0.f};
    lastPosition = {0.f, 0.f};
    currentVelocity = {0.f, 0.f};
    isShooting = false;
    isAiming = false;
}

void Cue::update(sf::RenderWindow& window, sf::Vector2f mousePos, float dt, bool wantToShoot, bool wantToAim) {
    
    sf::Vector2f newPos = tip.getPosition(); 

    // Aiming
    if (wantToAim && !isShooting) {
        if (!isAiming) {
            isAiming = true;
            aimTipOrigin = newPos; 
            float dx = mousePos.x - newPos.x;
            float dy = mousePos.y - newPos.y;
            aimAngle = std::atan2(dy, dx);
            window.setMouseCursorVisible(false);
            sf::Vector2i screenPos = window.mapCoordsToPixel(newPos);
            sf::Mouse::setPosition(screenPos, window);
        }
        
        newPos = aimTipOrigin;
        sf::Vector2i currentMouseScreen = sf::Mouse::getPosition(window);
        sf::Vector2i anchorScreen = window.mapCoordsToPixel(newPos);
        int deltaX = currentMouseScreen.x - anchorScreen.x;
        aimAngle += deltaX * 0.005f; 
        sf::Mouse::setPosition(anchorScreen, window);
        shootDir = sf::Vector2f(std::cos(aimAngle), std::sin(aimAngle));
        sf::Angle rot = sf::radians(aimAngle); 
        aimStick.setRotation(rot);
        cueSprite.setRotation(rot);
    }
    else {
        if (isAiming) {
            isAiming = false;
            window.setMouseCursorVisible(true);
        }
    }

    // Shooting
    if (wantToShoot) {
        if (!isShooting) {
            isShooting = true;
            shootTipOrigin = tip.getPosition(); 
            window.setMouseCursorVisible(false);
            currentPullback = 0.f; 
            sf::Vector2i screenPos = window.mapCoordsToPixel(shootTipOrigin);
            sf::Mouse::setPosition(screenPos, window);
        }

        // accumulate pullback
        sf::Vector2i currentMouseScreen = sf::Mouse::getPosition(window);
        sf::Vector2i anchorScreen = window.mapCoordsToPixel(shootTipOrigin);
        
        int deltaY = currentMouseScreen.y - anchorScreen.y;
        currentPullback += deltaY * 0.5f; 
        if (currentPullback < -150.0f) currentPullback = -150.0f;
        if (currentPullback > 60.0f)   currentPullback = 60.0f;
        newPos = shootTipOrigin - (shootDir * currentPullback);
        sf::Mouse::setPosition(anchorScreen, window);
    }
    else {
        // Just released Right Click?
        if (isShooting) {
            isShooting = false;
            window.setMouseCursorVisible(true);
            // Physics will take over 'newPos' automatically
        }
    }

    // Free move
    if (!isShooting && !isAiming) {
        newPos = mousePos;
        aimStick.setPosition(newPos);
        cueSprite.setPosition(newPos);
        sf::Angle currentRot = sf::radians(aimAngle);
        aimStick.setRotation(currentRot);
        cueSprite.setRotation(currentRot);
    }

    // Sync & physics
    tip.setPosition(newPos);
    aimStick.setPosition(newPos); 
    cueSprite.setPosition(newPos);

    if (dt > 0.0001f) {
        sf::Vector2f rawVelocity = (newPos - lastPosition) / dt;
        float maxSpeed = 3000.0f; 
        float currentSpeedSq = rawVelocity.x*rawVelocity.x + rawVelocity.y*rawVelocity.y;

        if (currentSpeedSq > maxSpeed * maxSpeed) {
            float speed = std::sqrt(currentSpeedSq);
            rawVelocity = (rawVelocity / speed) * maxSpeed;
        }
        currentVelocity = rawVelocity;
    }
    lastPosition = newPos;
}

bool Cue::checkCollisions(std::vector<BallObject*>& balls) {
    sf::Vector2f tipPos = tip.getPosition();
    float tipRadius = tip.getRadius();

    for (BallObject* ball : balls) {
        sf::Vector2f diff = ball->sprite.getPosition() - tipPos;
        float distSq = diff.x*diff.x + diff.y*diff.y;
        float minDist = ball->radius + tipRadius;

        if (distSq < minDist * minDist) {
            float dist = std::sqrt(distSq);
            if (dist < 0.0001f) { diff = {1.0f, 0.0f}; dist = 1.0f; }

            sf::Vector2f normal = diff / dist;
            float overlap = minDist - dist;
            ball->sprite.move(normal * overlap);

            sf::Vector2f relVel = currentVelocity - ball->velocity;
            float velAlongNormal = relVel.x * normal.x + relVel.y * normal.y;

            if (velAlongNormal > 0) {
                ball->velocity += (relVel.x * normal.x + relVel.y * normal.y) * normal * 1.5f;
                return true;
            }
        }
    }
    return false;
}

void Cue::render(sf::RenderWindow& window) {
    if (cueTexture.getSize().x > 0) window.draw(cueSprite);
    else window.draw(aimStick);
}