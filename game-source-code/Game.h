// game.h
#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "GameConstants.h"
#include "Lander.h"
#include "Missile.h"
#include "Laser.h"
#include "Player.h"
#include "Humanoid.h"
#include <cmath>
#include <limits>
// #include
class Game
{
public:
    Game();
    void moveLandersTowardsHumanoids();

    void run(sf::RenderWindow &window);

private:
    // Add these variables to track the nearest humanoid and whether a lander is attached
    Humanoid *targetHumanoid;
    bool isLanderAttached;

    // Add this function to find the nearest humanoid
    Humanoid *findNearestHumanoid();
    int previousLevelScore;
    int level;
    int score;
    int highScore;
    bool gameStarted;
    bool quitConfirmation;
    bool isSplashScreenVisible;
    bool isPauseScreenVisible;
    bool isWinScreenVisible;
    bool isGameOver;
    sf::Texture landerTexture;
    sf::Texture humanoidTexture;
    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    Player player;
    sf::Texture playerTexture;
    std::vector<Lander> landers;
    std::vector<Humanoid> humanoids;
    std::vector<Missile> missiles;
    std::vector<Laser> lasers;
    sf::Clock spawnTimer;
    sf::Clock spawnHumanoidTimer;
    sf::Clock missileTimer;
    sf::Font font;
    void resetGame();
    void handleInput(sf::RenderWindow &window);
    void update();
    void render(sf::RenderWindow &window);
    bool landerContact;          // To track if a lander has contacted a humanoid
    int humanoidsKilled;         // To keep track of how many humanoids have been killed
    Humanoid *connectedHumanoid; // To store the humanoid connected to a lander or player
    Lander *connectedLander;     // To store the lander connected to a humanoid
    bool playerCarryingHumanoid; // To track if the player is carrying a humanoid
};