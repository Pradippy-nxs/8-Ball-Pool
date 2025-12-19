#include <iostream>
#include <cmath>
#include "Game.h"
#include "Table.h"
#include "Ball.h"
#include "Cue.h"
#include "Player.h"

// Constructor
Game::Game() : window(sf::VideoMode({1600, 900}), "SFML Pool"),
               player1("Player 1"),
               player2("Player 2"),
               bgTexture("asset/wood.jpg"),
               bgSprite(bgTexture),
               font("asset/Montserrat-ExtraBold.ttf"),
               showCue(false),
               hitBuffer("asset/ball_hit.mp3"),
               pocketBuffer("asset/pocket.mp3"),
               cueBuffer("asset/cue_hit.mp3"),
               hitSound(hitBuffer),
               pocketSound(pocketBuffer),
               cueSound(cueBuffer)
{
    window.setFramerateLimit(60);
    hitSound.setVolume(80.f);
    if (bgMusic.openFromFile("asset/schlatt.mp3")) {
        bgMusic.setLooping(true);   // Make it repeat forever
        bgMusic.setVolume(30.f); // Keep it lower than sound effects (0-100)
        bgMusic.play();          // Start playing immediately
    }
    else {
        std::cout << "Error: Could not load asset/music.mp3" << std::endl;
    }
    // Setup background
    sf::Vector2u winSize = window.getSize();
    sf::Vector2u texSize = bgTexture.getSize();
    float scaleX = static_cast<float>(winSize.x) / texSize.x;
    float scaleY = static_cast<float>(winSize.y) / texSize.y;
    float scale = std::max(scaleX, scaleY);
    bgSprite.setScale({scale, scale});
    bgSprite.setOrigin({texSize.x / 2.0f, texSize.y / 2.0f});
    bgSprite.setPosition({winSize.x / 2.0f, winSize.y / 2.0f});

    // Game objects
    table.setup(get_window_size());
    initTableSetup(); // Positions balls

    // Debug shapes
    debugCushionShapes.clear();
    for (const auto &wallRect : table.getCushions())
    {
        sf::RectangleShape shape;
        shape.setPosition(wallRect.position);
        shape.setSize(wallRect.size);
        shape.setFillColor(sf::Color(255, 0, 0, 50));
        debugCushionShapes.push_back(shape);
    }
    debugPocketShapes.clear();
    for (const auto &pos : table.getPockets())
    {
        sf::CircleShape circle(table.getPocketRadius());
        circle.setOrigin({table.getPocketRadius(), table.getPocketRadius()});
        circle.setPosition(pos);
        circle.setFillColor(sf::Color(0, 255, 0, 150));
        debugPocketShapes.push_back(circle);
    }

    // Init state
    currentPlayer = &player1;
    foulCommitted = false;
    ballPottedThisTurn = false;
    isTurnInProgress = false;

    // Start menu
    gameState = GameState::MENU;
    if (hitBuffer.loadFromFile("asset/ball_hit.mp3"))
    {
        hitSound.setBuffer(hitBuffer);
        hitSound.setVolume(80.f); // Optional volume tweak
    }

    if (pocketBuffer.loadFromFile("asset/pocket.mp3"))
    {
        pocketSound.setBuffer(pocketBuffer);
    }

    if (cueBuffer.loadFromFile("asset/cue_hit.mp3"))
    {
        cueSound.setBuffer(cueBuffer);
    }
}

// Get window size
sf::Vector2u Game::get_window_size()
{
    return this->window.getSize();
}


// Main loop
void Game::run()
{
    sf::Clock clock;
    while (window.isOpen())
    {
        float dt = clock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

// Reset
void Game::resetGame()
{
    // Reset players
    player1.score = 0;
    player1.assignedGroup = BallGroup::Open;
    player2.score = 0;
    player2.assignedGroup = BallGroup::Open;
    currentPlayer = &player1;
    winnerName = "";

    // Reset flags
    foulCommitted = false;
    ballPottedThisTurn = false;
    isTurnInProgress = false;
    ballsInPockets.clear(); // Clear UI tracker

    // Reset balls
    // Reset ball properties and positions
    std::vector<BallObject *> all = balls.getAllBalls();
    for (auto *b : all)
    {
        b->velocity = {0.f, 0.f};
        b->isPotted = false;
    }

    // Reset positions
    initTableSetup();

    // Start game
    gameState = GameState::PLAYING;
    addMessage("Game Restarted!", sf::Color::Green);
}


// Events
// Process events
void Game::processEvents()
{
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();

        if (const auto *k = event->getIf<sf::Event::KeyPressed>())
        {
            if (k->code == sf::Keyboard::Key::Space)
            {
                showCue = !showCue; // Flip true/false

                // Safety: If we open the cue, drop any ball we are dragging
                if (showCue)
                    pSelectedBall = nullptr;
            }
        }
        // Mouse clicks
        if (const auto *m = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (m->button == sf::Mouse::Button::Left)
            {
                sf::Vector2f mousePos(static_cast<float>(m->position.x), static_cast<float>(m->position.y));

                // Menu click
                if (gameState == GameState::MENU)
                {
                    // "PLAY GAME" Button Rect (Must match renderMenu logic)
                    sf::Vector2f center = {window.getSize().x / 2.0f, window.getSize().y / 2.0f};
                    sf::FloatRect playBtn({center.x - 150, center.y}, {300, 60});
                    sf::FloatRect exitBtn({center.x - 150, center.y + 80}, {300, 60});

                    if (playBtn.contains(mousePos))
                    {
                        gameState = GameState::PLAYING;
                    }
                    else if (exitBtn.contains(mousePos))
                    {
                        window.close();
                    }
                }
                // Game over click
                else if (gameState == GameState::GAME_OVER)
                {
                    sf::Vector2f center = {window.getSize().x / 2.0f, window.getSize().y / 2.0f};
                    sf::FloatRect restartBtn({center.x - 150, center.y + 20}, {300, 60});
                    sf::FloatRect exitBtn({center.x - 150, center.y + 100}, {300, 60});

                    if (restartBtn.contains(mousePos))
                    {
                        resetGame();
                    }
                    else if (exitBtn.contains(mousePos))
                    {
                        window.close();
                    }
                }
                // Playing click
                else if (gameState == GameState::PLAYING)
                {
                    lastMousePos = mousePos;
                    for (BallObject *ball : balls.getAllBalls())
                    {
                        if (ball->isPotted)
                            continue;
                        if (ball->sprite.getGlobalBounds().contains(mousePos))
                        {
                            pSelectedBall = ball;
                            grabOffset = ball->sprite.getPosition() - mousePos;
                            pSelectedBall->velocity = {0.f, 0.f};
                            break;
                        }
                    }
                }
            }
        }

        // Mouse release
        if (const auto *m = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (m->button == sf::Mouse::Button::Left && pSelectedBall != nullptr)
            {
                sf::Vector2f mousePos(static_cast<float>(m->position.x), static_cast<float>(m->position.y));
                pSelectedBall->velocity = (mousePos - lastMousePos) * 30.0f;
                pSelectedBall = nullptr;
            }
        }
    }
}


// Update
void Game::update(float dt)
{
    if (gameState != GameState::PLAYING)
    {
        updateMessages(dt); // Keep animating messages
        return;
    }
    // Input
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos(static_cast<float>(pixelPos.x), static_cast<float>(pixelPos.y));

    bool activeCue = showCue;
    bool wantToAim = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
    bool wantToShoot = sf::Mouse::isButtonPressed(sf::Mouse::Button::Right);

    // Ball dragging
    if (!activeCue)
    {
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && pSelectedBall == nullptr)
        {
            for (BallObject *ball : balls.getAllBalls())
            {
                // FIX: Ignore Potted Balls!
                if (ball->isPotted)
                    continue;

                if (ball->sprite.getGlobalBounds().contains(mousePos))
                {
                    pSelectedBall = ball;
                    grabOffset = ball->sprite.getPosition() - mousePos;
                    pSelectedBall->velocity = {0.f, 0.f};
                    break;
                }
            }
        }

        if (pSelectedBall != nullptr)
        {
            pSelectedBall->sprite.setPosition(mousePos + grabOffset);
            lastMousePos = mousePos;

            if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                pSelectedBall = nullptr;
            }
        }
    }

    // Cue control
    if (showCue && pSelectedBall == nullptr)
    {
        // CHANGE: Pass 'window' as the first argument!
        cue.update(window, mousePos, dt, wantToShoot, wantToAim);

        std::vector<BallObject *> allBalls = balls.getAllBalls();
        if (cue.checkCollisions(allBalls))
        {
            cueSound.play();
        }
    }

    // Physics
    std::vector<BallObject *> allBalls = balls.getAllBalls();
    const std::vector<sf::FloatRect> &walls = table.getCushions();

    for (BallObject *ball : allBalls)
    {
        if (ball == pSelectedBall)
            continue;

        ball->sprite.move(ball->velocity * dt);

        // Pocket collision handling
        ball->velocity *= 0.985f;

        if (std::abs(ball->velocity.x) < 2.0f && std::abs(ball->velocity.y) < 2.0f)
        {
            ball->velocity = {0.f, 0.f};
        }

        // Wall collisions
        sf::FloatRect ballBounds = ball->sprite.getGlobalBounds();

        for (const auto &wall : walls)
        {
            if (ballBounds.findIntersection(wall))
            {
                bool isHorizontalWall = wall.size.x > wall.size.y;

                if (isHorizontalWall)
                {
                    ball->velocity.y *= -0.85f;

                    if (ball->sprite.getPosition().y < window.getSize().y / 2.0f)
                    {
                        ball->sprite.move({0.f, 2.0f});
                    }
                    else
                    {
                        ball->sprite.move({0.f, -2.0f});
                    }
                }
                else
                {
                    ball->velocity.x *= -0.85f;

                    if (ball->sprite.getPosition().x < window.getSize().x / 2.0f)
                    {
                        ball->sprite.move({2.0f, 0.f});
                    }
                    else
                    {
                        ball->sprite.move({-2.0f, 0.f});
                    }
                }
            }
        }
    }

    // Ball collisions
    handleCollisions();
    checkPockets();

    // Turn management
    bool moving = areBallsMoving();

    if (moving)
    {
        isTurnInProgress = true;
    }
    else if (isTurnInProgress)
    {
        switchTurn();
        isTurnInProgress = false;
    }
    updateMessages(dt);
}


// Physics & Collisions
// Handle collisions
void Game::handleCollisions()
{
    std::vector<BallObject *> allBalls = balls.getAllBalls();

    for (size_t i = 0; i < allBalls.size(); ++i)
    {
        for (size_t j = i + 1; j < allBalls.size(); ++j)
        {
            resolveBallCollision(*allBalls[i], *allBalls[j]);
        }
    }
}

// Resolve collision
void Game::resolveBallCollision(BallObject &b1, BallObject &b2)
{
    // Skip if any ball is potted
    if (b1.isPotted || b2.isPotted)
        return;
    sf::Vector2f pos1 = b1.sprite.getPosition();
    sf::Vector2f pos2 = b2.sprite.getPosition();
    sf::Vector2f delta = pos2 - pos1;

    float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);
    float minDist = b1.radius + b2.radius;

    if (dist < minDist && dist != 0.f)
    {
        float overlap = minDist - dist;
        sf::Vector2f normal = delta / dist;

        b1.sprite.move(-normal * (overlap * 0.5f));
        b2.sprite.move(normal * (overlap * 0.5f));

        sf::Vector2f relativeVelocity = b2.velocity - b1.velocity;

        float velAlongNormal = relativeVelocity.x * normal.x + relativeVelocity.y * normal.y;

        if (velAlongNormal > 0)
            return;

        float e = 0.9f;

        float j = -(1 + e) * velAlongNormal;
        j /= (1 / b1.mass + 1 / b2.mass);

        sf::Vector2f impulse = j * normal;
        b1.velocity -= (1 / b1.mass) * impulse;
        b2.velocity += (1 / b2.mass) * impulse;
        float pitch = 0.8f + (static_cast<float>(rand()) / RAND_MAX) * 0.4f;
        hitSound.setPitch(pitch);
        hitSound.play();
    }
}


// Game Setup
// Init table setup
void Game::initTableSetup()
{
    // Table data
    sf::Vector2f offset = table.getOffset();
    sf::Vector2u area = table.getGameAreaSize();

    // Table center
    float centerX = offset.x + (area.x / 2.0f);
    float centerY = offset.y + (area.y / 2.0f);

    // Position cue ball
    BallObject *cue = balls.getCueBall();
    // Place at 25% of the table width
    cue->sprite.setPosition({offset.x + (area.x * 0.25f), centerY});
    cue->velocity = {0.f, 0.f};

    // Position rack
    std::vector<BallObject *> rackBalls;
    std::vector<BallObject *> solids = balls.getSolids();
    std::vector<BallObject *> stripes = balls.getStripes();

    rackBalls.insert(rackBalls.end(), solids.begin(), solids.end());
    rackBalls.insert(rackBalls.end(), stripes.begin(), stripes.end());

    float ballRadius = rackBalls[0]->radius;
    float ballDiameter = ballRadius * 2.0f;

    // Rack start position
    float startX = offset.x + (area.x * 0.75f);
    float startY = centerY;

    int currentBallIndex = 0;

    for (int row = 0; row < 5; ++row)
    {
        float rowStartY = startY - (row * ballRadius);
        float rowX = startX + (row * ballDiameter * 0.866f);

        for (int col = 0; col <= row; ++col)
        {
            if (currentBallIndex >= rackBalls.size())
                break;

            BallObject *ball = rackBalls[currentBallIndex];

            float x = rowX;
            float y = rowStartY + (col * ballDiameter);

            ball->sprite.setPosition({x, y});
            ball->velocity = {0.f, 0.f};

            currentBallIndex++;
        }
    }
}


// Pockets
// Check pockets
void Game::checkPockets()
{
    const std::vector<sf::Vector2f> &pockets = table.getPockets();
    std::vector<BallObject *> allBalls = balls.getAllBalls();
    float pocketRadius = 35.0f;

    for (BallObject *ball : allBalls)
    {
        if (ball->isPotted)
            continue;

        for (const auto &pocketPos : pockets)
        {
            sf::Vector2f diff = ball->sprite.getPosition() - pocketPos;
            float distSq = diff.x * diff.x + diff.y * diff.y;

            if (distSq < pocketRadius * pocketRadius)
            {
                pocketSound.play();
                // Cue ball
                if (ball->id == 0)
                {
                    if (foulCommitted)
                        return;
                    addMessage("SCRATCH! Foul.", sf::Color::Red);
                    foulCommitted = true;
                    sf::Vector2f offset = table.getOffset();
                    sf::Vector2u area = table.getGameAreaSize();
                    ball->sprite.setPosition({offset.x + (area.x / 4.0f), offset.y + (area.y / 2.0f)});
                    ball->velocity = {0.f, 0.f};
                    ball->isPotted = false;
                    return;
                }

                // Normal ball
                ball->isPotted = true;
                ball->velocity = {0.f, 0.f};
                ballsInPockets.push_back(ball);

                // Game over check
                if (ball->id == 8)
                {
                    bool clearedAll = (currentPlayer->score == 7);
                    bool cleanWin = clearedAll && !foulCommitted;

                    if (cleanWin)
                    {
                        winnerName = currentPlayer->name;
                    }
                    else
                    {
                        if (currentPlayer == &player1)
                            winnerName = player2.name;
                        else
                            winnerName = player1.name;
                    }
                    gameState = GameState::GAME_OVER; // <--- CHANGE STATE
                    return;
                }

                // Scoring
                if (player1.assignedGroup == BallGroup::Open)
                    assignGroups(ball->id);

                bool isLegalPot = currentPlayer->isLegalBall(ball->id);
                if (isLegalPot)
                    ballPottedThisTurn = true;
                else
                    addMessage("Opponent Ball Potted!", sf::Color(255, 100, 0));

                BallGroup ballGroup = BallGroup::Open;
                if (ball->id >= 1 && ball->id <= 7)
                    ballGroup = BallGroup::Solids;
                else if (ball->id >= 9 && ball->id <= 15)
                    ballGroup = BallGroup::Stripes;

                Player *scoringPlayer = nullptr;
                if (player1.assignedGroup == ballGroup)
                    scoringPlayer = &player1;
                else if (player2.assignedGroup == ballGroup)
                    scoringPlayer = &player2;

                if (scoringPlayer != nullptr)
                    scoringPlayer->score++;

                break;
            }
        }
    }
}


// UI & Rendering
// Render UI
void Game::renderUI()
{
    sf::Vector2f viewSize = sf::Vector2f(window.getSize());

    // Colors
    sf::Color darkBg(15, 15, 15, 220);
    sf::Color activeP1(0, 191, 255);
    sf::Color activeP2(255, 69, 0);
    sf::Color inactiveText(150, 150, 150);
    sf::Color white(255, 255, 255);

    // Bottom rail
    float railHeight = 70.f;
    sf::RectangleShape railRect({viewSize.x, railHeight});
    railRect.setPosition({0.f, viewSize.y - railHeight});
    railRect.setFillColor(darkBg);
    railRect.setOutlineColor(sf::Color(50, 50, 50));
    railRect.setOutlineThickness(-2.f);
    window.draw(railRect);

    if (!ballsInPockets.empty())
    {
        float iconSize = 40.0f;
        float padding = 10.0f;
        float startY = viewSize.y - railHeight + (railHeight - iconSize) / 2.0f;
        float totalWidth = ballsInPockets.size() * (iconSize + padding);
        float startX = (viewSize.x / 2.0f) - (totalWidth / 2.0f);

        for (int i = 0; i < ballsInPockets.size(); i++)
        {
            BallObject *ball = ballsInPockets[i];
            sf::Sprite uiSprite(ball->sprite.getTexture());

            sf::Vector2u texSize = ball->sprite.getTexture().getSize();
            float scaleX = iconSize / static_cast<float>(texSize.x);
            float scaleY = iconSize / static_cast<float>(texSize.y);
            uiSprite.setScale({scaleX, scaleY});
            uiSprite.setPosition({startX + (i * (iconSize + padding)), startY});
            window.draw(uiSprite);
        }
    }

    // HUD
    float hudHeight = 90.f;
    float panelWidth = 350.f;
    float panelHeight = 70.f;
    float panelY = 10.f;

    // Player 1 Panel
    sf::RectangleShape p1Panel({panelWidth, panelHeight});
    p1Panel.setPosition({20.f, panelY});
    p1Panel.setFillColor(darkBg);
    if (currentPlayer == &player1)
    {
        p1Panel.setOutlineColor(activeP1);
        p1Panel.setOutlineThickness(3.f);
    }
    else
    {
        p1Panel.setOutlineColor(sf::Color(50, 50, 50));
        p1Panel.setOutlineThickness(1.f);
    }
    window.draw(p1Panel);

    sf::Text p1Name(font, player1.name, 24);
    p1Name.setPosition({35.f, panelY + 10.f});
    p1Name.setFillColor(currentPlayer == &player1 ? activeP1 : inactiveText);
    window.draw(p1Name);

    sf::Text p1Group(font, player1.getGroupString(), 18);
    p1Group.setPosition({35.f, panelY + 40.f});
    p1Group.setFillColor(sf::Color(200, 200, 200));
    window.draw(p1Group);

    sf::Text p1Score(font, std::to_string(player1.score), 50);
    sf::FloatRect s1Bounds = p1Score.getGlobalBounds();
    p1Score.setPosition({20.f + panelWidth - s1Bounds.size.x - 20.f, panelY + 5.f});
    p1Score.setFillColor(white);
    window.draw(p1Score);

    // Player 2 Panel
    sf::RectangleShape p2Panel({panelWidth, panelHeight});
    p2Panel.setPosition({viewSize.x - panelWidth - 20.f, panelY});
    p2Panel.setFillColor(darkBg);
    if (currentPlayer == &player2)
    {
        p2Panel.setOutlineColor(activeP2);
        p2Panel.setOutlineThickness(3.f);
    }
    else
    {
        p2Panel.setOutlineColor(sf::Color(50, 50, 50));
        p2Panel.setOutlineThickness(1.f);
    }
    window.draw(p2Panel);

    sf::Text p2Name(font, player2.name, 24);
    p2Name.setPosition({viewSize.x - panelWidth - 20.f + 15.f, panelY + 10.f});
    p2Name.setFillColor(currentPlayer == &player2 ? activeP2 : inactiveText);
    window.draw(p2Name);

    sf::Text p2Group(font, player2.getGroupString(), 18);
    p2Group.setPosition({viewSize.x - panelWidth - 20.f + 15.f, panelY + 40.f});
    p2Group.setFillColor(sf::Color(200, 200, 200));
    window.draw(p2Group);

    sf::Text p2Score(font, std::to_string(player2.score), 50);
    sf::FloatRect s2Bounds = p2Score.getGlobalBounds();
    p2Score.setPosition({viewSize.x - 20.f - s2Bounds.size.x - 20.f, panelY + 5.f});
    p2Score.setFillColor(white);
    window.draw(p2Score);

    // Center status (VS)
    if (gameState != GameState::GAME_OVER)
    {
        sf::Text vsText(font, "VS", 30);
        sf::FloatRect vsBounds = vsText.getGlobalBounds();
        vsText.setPosition({(viewSize.x / 2.0f) - (vsBounds.size.x / 2.0f), panelY + 15.f});
        vsText.setFillColor(sf::Color(255, 255, 255));
        window.draw(vsText);
    }
}


// Game Logic
void Game::assignGroups(int pottedId)
{
    if (pottedId >= 1 && pottedId <= 7)
    {
        currentPlayer->assignedGroup = BallGroup::Solids;
        if (currentPlayer == &player1)
            player2.assignedGroup = BallGroup::Stripes;
        else
            player1.assignedGroup = BallGroup::Stripes;
        std::string msg = currentPlayer->name + " is " + currentPlayer->getGroupString();
        addMessage(msg, sf::Color::Yellow);
    }
    else if (pottedId >= 9 && pottedId <= 15)
    {
        currentPlayer->assignedGroup = BallGroup::Stripes;
        if (currentPlayer == &player1)
            player2.assignedGroup = BallGroup::Solids;
        else
            player1.assignedGroup = BallGroup::Solids;

        std::cout << currentPlayer->name << " is STRIPES" << std::endl;
    }
}

// Turn switching
void Game::switchTurn()
{
    bool keepTurn = ballPottedThisTurn && !foulCommitted;
    if (foulCommitted)
    {
        addMessage("FOUL! Turn Lost.", sf::Color::Red);
    }
    else if (!ballPottedThisTurn)
    {
        addMessage("Miss.", sf::Color(200, 200, 200));
    }
    if (keepTurn)
    {
        addMessage("Good Pot!", sf::Color::Green);
        addMessage(currentPlayer->name + " Continues", sf::Color::Cyan);
    }
    else
    {
        if (currentPlayer == &player1)
            currentPlayer = &player2;
        else
            currentPlayer = &player1;

        addMessage("Turn Switched: " + currentPlayer->name, sf::Color::White);
    }

    // Reset turn flags
    ballPottedThisTurn = false;
    foulCommitted = false;
}

bool Game::areBallsMoving()
{
    for (BallObject *ball : balls.getAllBalls())
    {
        if (std::abs(ball->velocity.x) > 0.05f || std::abs(ball->velocity.y) > 0.05f)
        {
            return true;
        }
    }
    return false;
}


// Rendering
// Render
void Game::render()
{
    window.clear(sf::Color(30, 30, 30));

    // 1. Always Draw Background
    window.draw(bgSprite);

    // Draw by state
    if (gameState == GameState::MENU)
    {
        renderMenu();
    }
    else if (gameState == GameState::PLAYING)
    {
        // Draw the Table & Balls
        window.draw(table.getSprite());

        for (auto *ball : balls.getSolids())
            if (!ball->isPotted)
                window.draw(ball->sprite);

        for (auto *ball : balls.getStripes())
            if (!ball->isPotted)
                window.draw(ball->sprite);

        window.draw(balls.getCueBall()->sprite);

        // Debug shapes (optional)

        // Draw cue
        if (showCue && pSelectedBall == nullptr)
        {
            cue.render(window);
        }

        // Draw HUD
        renderUI();
    }
    else if (gameState == GameState::GAME_OVER)
    {
        // Draw the game "frozen" in the background
        window.draw(table.getSprite());
        for (auto *ball : balls.getAllBalls())
            if (!ball->isPotted)
                window.draw(ball->sprite);

        // Draw HUD (so you can see final scores)
        renderUI();

        // Draw the Dark Overlay & Winner Text
        renderGameOver();
    }

    // Floating messages
    renderMessages();
    window.display();
}


// Messages
void Game::addMessage(std::string text, sf::Color color)
{
    // Add floating message
    messages.push_back({text, color, 2.5f, 0.f});
    std::cout << "[GAME] " << text << std::endl;
}

void Game::updateMessages(float dt)
{
    // Update and remove expired messages
    for (int i = messages.size() - 1; i >= 0; i--)
    {
        GameMessage &msg = messages[i];
        msg.duration -= dt;
        msg.yOffset -= 20.f * dt;
        if (msg.duration <= 0.f)
            messages.erase(messages.begin() + i);
    }
}

void Game::renderMessages()
{
    static const sf::Font font("asset/Montserrat-ExtraBold.ttf");
    sf::Vector2f center = {window.getSize().x / 2.0f, window.getSize().y / 2.0f};
    for (int i = 0; i < messages.size(); i++)
    {
        GameMessage &msg = messages[i];
        sf::Text text(font, msg.text, 32);
        sf::FloatRect bounds = text.getGlobalBounds();
        text.setOrigin({bounds.size.x / 2.0f, bounds.size.y / 2.0f});
        float yPos = center.y - 100.f + msg.yOffset + (i * 40.f);
        text.setPosition({center.x, yPos});
        float alpha = (msg.duration / 2.5f) * 255.f;
        if (alpha > 255.f)
            alpha = 255.f;
        sf::Color c = msg.color;
        c.a = static_cast<uint8_t>(alpha);
        text.setFillColor(c);
        text.setOutlineColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));
        text.setOutlineThickness(2.f);
        window.draw(text);
    }
}


// Screens
// Menu screen
void Game::renderMenu()
{
    sf::Vector2f center = {window.getSize().x / 2.0f, window.getSize().y / 2.0f};

    // Title
    sf::Text title(font, "D-Pool", 100);
    sf::FloatRect tBounds = title.getGlobalBounds();
    title.setOrigin({tBounds.size.x / 2.0f, tBounds.size.y / 2.0f});
    title.setPosition({center.x, center.y - 150.f});
    title.setOutlineColor(sf::Color::Black);
    title.setOutlineThickness(5.f);
    window.draw(title);

     sf::Text subtitle(font, "Kelompok 3 - Made with SFML and Love <3", 20);
    sf::FloatRect stBounds = subtitle.getGlobalBounds();
    subtitle.setOrigin({stBounds.size.x / 2.0f, stBounds.size.y / 2.0f});
    subtitle.setPosition({center.x, center.y - 50.0f});
    window.draw(subtitle);

    // Play Button
    sf::RectangleShape playBtn({300.f, 60.f});
    playBtn.setOrigin({150.f, 30.f});
    playBtn.setPosition({center.x, center.y + 30.f});
    playBtn.setFillColor(sf::Color(0, 150, 255));
    playBtn.setOutlineColor(sf::Color::White);
    playBtn.setOutlineThickness(2.f);
    window.draw(playBtn);

    sf::Text playText(font, "PLAY GAME", 30);
    sf::FloatRect ptBounds = playText.getGlobalBounds();
    playText.setOrigin({ptBounds.size.x / 2.f, ptBounds.size.y / 2.f});
    playText.setPosition(playBtn.getPosition());
    window.draw(playText);

    // Exit Button
    sf::RectangleShape exitBtn({300.f, 60.f});
    exitBtn.setOrigin({150.f, 30.f});
    exitBtn.setPosition({center.x, center.y + 110.f}); // 80px gap
    exitBtn.setFillColor(sf::Color(200, 50, 50));
    exitBtn.setOutlineColor(sf::Color::White);
    exitBtn.setOutlineThickness(2.f);
    window.draw(exitBtn);

    sf::Text exitText(font, "EXIT", 30);
    sf::FloatRect etBounds = exitText.getGlobalBounds();
    exitText.setOrigin({etBounds.size.x / 2.f, etBounds.size.y / 2.f});
    exitText.setPosition(exitBtn.getPosition());
    window.draw(exitText);
}

// Game over screen
void Game::renderGameOver()
{
    sf::Vector2f viewSize = sf::Vector2f(window.getSize());
    sf::Vector2f center = {viewSize.x / 2.0f, viewSize.y / 2.0f};

    // Dark Overlay
    sf::RectangleShape overlay(viewSize);
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    // Winner Text
    std::string winMsg = winnerName + " WINS!";
    sf::Text winText(font, winMsg, 80);
    sf::FloatRect wBounds = winText.getGlobalBounds();
    winText.setOrigin({wBounds.size.x / 2.0f, wBounds.size.y / 2.0f});
    winText.setPosition({center.x, center.y - 100.f});
    winText.setFillColor(sf::Color::Green);
    winText.setOutlineColor(sf::Color::Black);
    winText.setOutlineThickness(3.f);
    window.draw(winText);

    // Restart Button
    sf::RectangleShape restartBtn({300.f, 60.f});
    restartBtn.setOrigin({150.f, 30.f});
    restartBtn.setPosition({center.x, center.y + 50.f});
    restartBtn.setFillColor(sf::Color(0, 200, 100));
    restartBtn.setOutlineThickness(2.f);
    window.draw(restartBtn);

    sf::Text rText(font, "RESTART", 30);
    sf::FloatRect rBounds = rText.getGlobalBounds();
    rText.setOrigin({rBounds.size.x / 2.f, rBounds.size.y / 2.f});
    rText.setPosition(restartBtn.getPosition());
    window.draw(rText);

    // Exit Button
    sf::RectangleShape exitBtn({300.f, 60.f});
    exitBtn.setOrigin({150.f, 30.f});
    exitBtn.setPosition({center.x, center.y + 130.f});
    exitBtn.setFillColor(sf::Color(200, 50, 50));
    exitBtn.setOutlineThickness(2.f);
    window.draw(exitBtn);

    sf::Text eText(font, "EXIT", 30);
    sf::FloatRect eBounds = eText.getGlobalBounds();
    eText.setOrigin({eBounds.size.x / 2.f, eBounds.size.y / 2.f});
    eText.setPosition(exitBtn.getPosition());
    window.draw(eText);
}