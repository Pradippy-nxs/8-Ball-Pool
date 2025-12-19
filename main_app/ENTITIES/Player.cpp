#include "Player.h"

Player::Player(std::string n) : name(n), assignedGroup(BallGroup::Open), score(0) {}

bool Player::isLegalBall(int ballId) {
    // special cases
    if (ballId == 0 || ballId == 8) return false;
    if (assignedGroup == BallGroup::Open) return true;
    if (assignedGroup == BallGroup::Solids) return (ballId >= 1 && ballId <= 7);
    if (assignedGroup == BallGroup::Stripes) return (ballId >= 9 && ballId <= 15);
    return false;
}

std::string Player::getGroupString() {
    if (assignedGroup == BallGroup::Solids) return "SOLIDS";
    if (assignedGroup == BallGroup::Stripes) return "STRIPES";
    return "OPEN";
}