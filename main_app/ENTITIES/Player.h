#pragma once
#include <string>

// Ball groups
enum class BallGroup {
    Open,    // no group yet
    Solids,  // balls 1-7
    Stripes  // balls 9-15
};

class Player {
public:
    std::string name;
    BallGroup assignedGroup;
    int score;

    Player(std::string name);

    // Check if ball belongs to player
    bool isLegalBall(int ballId);

    // Get group name
    std::string getGroupString();
};