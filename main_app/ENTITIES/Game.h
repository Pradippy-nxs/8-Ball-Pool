#pragma once
#include <vector>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Table.h"
#include "Ball.h"
#include "Cue.h"
#include "Player.h"

// Game states
enum class GameState
{
    MENU,
    PLAYING,
    GAME_OVER
};

struct GameMessage
{
    std::string text;
    sf::Color color;
    float duration;
    float yOffset;
};

class Game
{
private:
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer pocketBuffer;
    sf::SoundBuffer cueBuffer;
    sf::Sound hitSound;
    sf::Sound pocketSound;
    sf::Sound cueSound;
    sf::RenderWindow window;
    sf::Music bgMusic;
    bool showCue;

    // NEW: Game State
    GameState gameState;

    // Shared font
    sf::Font font;

    // Game objects
    Table table;
    Balls balls;
    Cue cue;

    // Background
    sf::Texture bgTexture;
    sf::Sprite bgSprite;

    // Logic
    BallObject *pSelectedBall = nullptr;
    sf::Vector2f grabOffset;
    sf::Vector2f lastMousePos;

    Player player1;
    Player player2;
    Player *currentPlayer;
    std::string winnerName; // Track who won

    bool foulCommitted;
    bool ballPottedThisTurn;
    bool isTurnInProgress;

    // Messages
    std::vector<GameMessage> messages;

    // Lists
    std::vector<sf::RectangleShape> debugCushionShapes;
    std::vector<sf::CircleShape> debugPocketShapes;
    std::vector<BallObject *> ballsInPockets;

    // Helpers
    bool areBallsMoving();
    void switchTurn();
    void assignGroups(int pottedId);
    void addMessage(std::string text, sf::Color color);
    void updateMessages(float dt);

    // Reset
    void resetGame();

    // Render helpers
    void checkPockets();
    void renderUI();
    void renderMessages();

    // Screens
    void renderMenu();
    void renderGameOver();

    void processEvents();
    void update(float dt);
    void render();

    // Physics
    void handleCollisions();
    void resolveBallCollision(BallObject &b1, BallObject &b2);

public:
    Game();
    sf::Vector2u get_window_size();
    void run();
    void initTableSetup();
};