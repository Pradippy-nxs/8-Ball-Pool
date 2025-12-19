#include <iostream>
#include "Ball.h"

Balls::Balls()
{
    // Solids
    for (int i = 1; i <= 8; i++)
    {
        std::string filename = "asset/ball_" + std::to_string(i) + ".png";

        // create texture
        sf::Texture *tex = new sf::Texture(filename);
        solid_textures.push_back(tex);

        // create ball
        BallObject *ball = new BallObject(*tex);
        // set id
        ball->id = i;
        solid_balls.push_back(ball);
    }

    // Stripes
    for (int i = 9; i <= 15; i++)
    {
        std::string filename = "asset/ball_" + std::to_string(i) + ".png";

        sf::Texture *tex = new sf::Texture(filename);
        striped_textures.push_back(tex);

        BallObject *ball = new BallObject(*tex);
        // set id
        ball->id = i;
        striped_balls.push_back(ball);
    }

    // Cue ball
    if (!cue_texture.loadFromFile("asset/cue_ball.png"))
    {
        // handle error
    }

    cue_ball = new BallObject(cue_texture);
    cue_ball->id = 0;
    cue_ball->sprite.setPosition({400, 400});
}

// Getters
std::vector<BallObject *> Balls::getSolids() { return this->solid_balls; }
std::vector<BallObject *> Balls::getStripes() { return this->striped_balls; }
BallObject *Balls::getCueBall() { return this->cue_ball; }

// Destructor
Balls::~Balls()
{
    // cleanup
    for (BallObject *b : solid_balls)
        delete b;
    for (BallObject *b : striped_balls)
        delete b;
    delete cue_ball;

    for (sf::Texture *t : solid_textures)
        delete t;
    for (sf::Texture *t : striped_textures)
        delete t;
}