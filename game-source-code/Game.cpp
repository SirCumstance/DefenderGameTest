#include "game.h"
#include <iostream>
Game::Game()
    : score(0), highScore(0), gameStarted(false),
      quitConfirmation(false), isSplashScreenVisible(true),
      isPauseScreenVisible(false), isWinScreenVisible(false), isGameOver(false),
      level(1), previousLevelScore(0), fuelBar(200.0f, 20.0f, 100.f) //, isGamePaused(false)
{
    // Load background texture
    backgroundTexture.loadFromFile("resources/background.jpg");
    // Set the background sprite
    backgroundSprite.setTexture(backgroundTexture);
    if (!font.loadFromFile("resources/sansation.ttf"))
    {
        std::cout << "Loading font error" << std::endl;
    }
    // Initialize textures
    landerTexture.loadFromFile("resources/landerShip.png");
    humanoidTexture.loadFromFile("resources/humanoid.png");
    fuelsTexture.loadFromFile("resources/fuel.png");
    playerTexture.loadFromFile("resources/playerShip.png");
    srand(static_cast<unsigned>(time(0))); // Seed random number generator
}

void Game::run(sf::RenderWindow &window)
{
    handleInput(window);
    update();
    render(window);
}

void Game::resetGame()
{
    // Reset player position
    player.setPosition(WINDOW_WIDTH / 2 - 25, WINDOW_HEIGHT - 60);

    // Clear the list of landers and missiles
    landers.clear();
    missiles.clear();
    lasers.clear();
    humanoids.clear(); // Clear the list of humanoids
    fuelBar.resetFuelBar();
    player.resetCurrentFuel();
    fuels.clear();
}

void Game::handleInput(sf::RenderWindow &window)
{
    sf::Event event;
    sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2f playerCenter = player.getPosition() + sf::Vector2f(player.getGlobalBounds().width / 2, player.getGlobalBounds().height / 2);
    sf::Vector2f rightScale(0.1f, 0.1f); // Set scale factors for X and Y uniformly
    sf::Vector2f leftScale(-0.1f, 0.1f); // Swap direction

    if (mousePosition.x > playerCenter.x)
    {
        player.setScale(rightScale); // Fire laser to the right
    }
    else
    {
        player.setScale(leftScale); // Flip horizontally by setting X scale to negative
    }

    if (!gameStarted && !isPauseScreenVisible && !isGameOver && !isWinScreenVisible)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        {
            gameStarted = true;
            isSplashScreenVisible = false;
            score = 0; // Reset the score when the game starts
        }
    }

    else if (gameStarted && !isPauseScreenVisible && !isGameOver && !isWinScreenVisible)
    {
        int oldScore = score; // Check if the player has won, ending the game, unless another level is chosen.

        if (score >= previousLevelScore + 100)
        {
            isWinScreenVisible = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
            isPauseScreenVisible = true;
        }
    }

    else if (isPauseScreenVisible)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y))
        {
            isPauseScreenVisible = false;
            gameStarted = false;
            if (score > highScore)
            {
                highScore = score; // Save the highscore
            }
            score = 0;
            resetGame();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N))
        {
            isPauseScreenVisible = false;
        }
    }
    // Handle input for game over screen
    else if (isWinScreenVisible)
    {
        gameStarted = false;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y)) // Continue the next level
        {
            isWinScreenVisible = false;
            gameStarted = true;
            level++;
            previousLevelScore = score;
            gameStarted = true;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N)) // Restart a new game

        {
            isWinScreenVisible = false;
            isSplashScreenVisible = true;
            resetGame();
            score = 0;
            previousLevelScore = 0;
            level = 1;
        }
    }
    else if (isGameOver)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y))
        {
            isGameOver = false;
            gameStarted = true;
            isSplashScreenVisible = false;
            if (score > highScore)
            {
                highScore = score;
            }
            score = 0;
            previousLevelScore = 0;
            level = 1;
            resetGame();
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N))
        {
            gameStarted = false;
            isSplashScreenVisible = true;
            resetGame();
            window.close();
        }
    }

    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }

        if (event.type == sf::Event::MouseButtonPressed)
        {

            if (event.mouseButton.button == sf::Mouse::Left)
            {

                Laser laser;
                laser.shape.setSize(sf::Vector2f(5, 20));
                laser.shape.setFillColor(sf::Color::Cyan);
                laser.shape.setPosition(player.getPosition().x + 22, player.getPosition().y);
                laser.velocity.x = (mousePosition.x > playerCenter.x) ? LASER_SPEED : -LASER_SPEED;

                laser.setFired(true);
                lasers.push_back(laser);
            }
        }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.getPosition().x > 0)
    {
        player.move(-PLAYER_SPEED, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && player.getPosition().x < WINDOW_WIDTH - 50)
    {
        player.move(PLAYER_SPEED, 0);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player.getPosition().y > 0)
    {
        player.move(0, -PLAYER_SPEED);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && player.getPosition().y < WINDOW_HEIGHT - 50)
    {
        player.move(0, PLAYER_SPEED);
    }
}

void Game::update()
{
    if (gameStarted && !isPauseScreenVisible && !isGameOver)
    {
        if (spawnTimer.getElapsedTime().asMilliseconds() > SPAWN_INTERVAL) // Spawn Landers
        {
            Lander newLander;

            // Set the lander's position to a random location along the X-axis
            float xPos = static_cast<float>(rand() % static_cast<int>(WINDOW_WIDTH - newLander.landerSprite.getGlobalBounds().width));
            newLander.landerSprite.setPosition(sf::Vector2f(xPos, -newLander.landerSprite.getGlobalBounds().height));
            newLander.landerSprite.setTexture(landerTexture);
            landers.push_back(newLander);
            spawnTimer.restart();
        }
        if (spawnHumanoidTimer.getElapsedTime().asMilliseconds() > HUMANOID_SPAWN_INTERVAL)
        {
            // Spawn Humanoids
            Humanoid newHumanoid;

            // Set the humanoid's position to a random location along the X-axis
            float xPos = static_cast<float>(rand() % static_cast<int>(WINDOW_WIDTH - newHumanoid.humanoidSprite.getGlobalBounds().width));
            newHumanoid.humanoidSprite.setPosition(sf::Vector2f(xPos, WINDOW_HEIGHT - newHumanoid.humanoidSprite.getGlobalBounds().height));
            newHumanoid.humanoidSprite.setTexture(humanoidTexture);
            humanoids.push_back(newHumanoid);
            spawnHumanoidTimer.restart();
        }
        if (fuelSpawnTimer.getElapsedTime().asMilliseconds() > FUEL_SPAWN_INTERVAL)
        {
            // Spawn Humanoids
            Fuels fuelCan;

            // Set the humanoid's position to a random location along the X-axis
            float xPos = static_cast<float>(rand() % static_cast<int>(WINDOW_WIDTH - fuelCan.fuelsSprite.getGlobalBounds().width));
            fuelCan.fuelsSprite.setPosition(sf::Vector2f(xPos, WINDOW_HEIGHT - fuelCan.fuelsSprite.getGlobalBounds().height));
            fuelCan.fuelsSprite.setTexture(fuelsTexture);
            fuels.push_back(fuelCan);
            fuelSpawnTimer.restart();
        }
        // moveLandersTowardsHumanoids();
        fuelBar.setFuel(player.getCurrentFuel(), player.getTotalFuel());
        // fuelBar.update();
        float deltaTime = frameClock.restart().asSeconds();
        player.consumeFuel(deltaTime);
        if (player.getCurrentFuel() <= 0)
        {
            player.move(0.0f, 4.0f);
            if (player.getPosition().y > WINDOW_HEIGHT - 60)
                isGameOver = true;
        }

        // Fire Missiles from Landers towards the player's ship after the desired interval
        for (size_t i = 0; i < landers.size(); i++)
        {
            if (missileTimer.getElapsedTime().asMilliseconds() > MISSILE_INTERVAL)
            {
                for (size_t i = 0; i < landers.size(); i++)
                {
                    if (!landers[i].isDestroyed())
                    {
                        missiles.push_back(Missile(landers[i].getPosition(), player.getPosition()));
                    }
                }
                missileTimer.restart();
            }
        }

        for (size_t i = 0; i < landers.size(); i++)
        {
            if (!landers[i].isDestroyed())
            {
                landers[i].landerSprite.move(0, LANDER_SPEED); // Move downward
            }

            // Remove Landers that go out of bounds
            if (landers[i].landerSprite.getPosition().y > WINDOW_HEIGHT)
            {
                landers.erase(landers.begin() + i);
                i--;
                missiles.erase(missiles.begin() + i);
                i--;
            }
        }

        // Move and draw missiles
        for (size_t i = 0; i < missiles.size(); i++)
        {
            missiles[i].move();
        }

        // Move Lasers
        for (size_t i = 0; i < lasers.size(); i++)
        {
            lasers[i].shape.move(lasers[i].velocity);
        }

        // Check for collision between lasers and landers
        for (size_t i = 0; i < lasers.size(); i++)
        {
            for (size_t j = 0; j < landers.size(); j++)
            {
                if (!landers[j].isDestroyed() && lasers[i].shape.getGlobalBounds().intersects(landers[j].getSprite().getGlobalBounds()))
                {
                    score += 10;
                    landers[j].destroy();
                    lasers.erase(lasers.begin() + i);
                    i--;   // Adjust the index after removal
                    break; // Exit the inner loop when a collision occurs
                }
            }
        }

        // Check for collision between player and landers
        for (size_t i = 0; i < landers.size(); i++)
        {
            if (!landers[i].isDestroyed() && player.getGlobalBounds().intersects(landers[i].landerSprite.getGlobalBounds()))
            {
                isGameOver = true;
                break;
            }
        }

        // Check for collision between player and missiles
        for (size_t i = 0; i < missiles.size(); i++)
        {
            if (missiles[i].shape.getGlobalBounds().intersects(player.getGlobalBounds()))
            {
                isGameOver = true;
                break;
            }
        }
    }
}

void Game::render(sf::RenderWindow &window) // Rendering the game shapes and sprites
{
    window.clear();
    window.draw(backgroundSprite);
    player.render(window);

    window.draw(player);
    // Draw humanoids
    for (auto &humanoid : humanoids)
    {
        if (!humanoid.isDestroyed())
        {
            humanoid.render(window);
        }
    }
    for (auto &fuelCan : fuels)
    {

        fuelCan.render(window);
    }
    // Draw Landers using the render function from Lander
    for (auto &lander : landers)
    {
        if (!lander.isDestroyed())
        {
            lander.render(window);
        }
    }

    // Draw Missiles
    for (const auto &missile : missiles)
    {
        window.draw(missile.shape);
    }

    // Draw Lasers
    for (const auto &laser : lasers)
    {
        window.draw(laser.shape);
    }
    // Draw the fuel bar
    fuelBar.render(window);

    // Display and update the score
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    scoreText.setString("Score: " + std::to_string(score)); // Display the score

    // Create a text object for the high score
    sf::Text highScoreText;
    highScoreText.setFont(font);
    highScoreText.setCharacterSize(24);
    highScoreText.setFillColor(sf::Color::White);
    highScoreText.setPosition(10, 40); // Adjust the vertical position for the high score
    highScoreText.setString("Highscore: " + std::to_string(highScore));

    window.draw(scoreText);
    window.draw(highScoreText);

    // Display the splash screen
    if (!gameStarted && !isPauseScreenVisible && !isGameOver && !isWinScreenVisible)
    {
        sf::Text splashText;
        splashText.setFont(font);
        splashText.setCharacterSize(32);
        splashText.setFillColor(sf::Color::White);
        splashText.setPosition(200, 200);
        splashText.setString("Press Space to Start \n "
                             " \n WASD keys to move \n "
                             " \n Left mouseclick to fire  \n "
                             " \n Hover mouse left or right of \n "
                             " \n player to change direction \n "
                             " \n esc to pause \n");

        window.draw(splashText);
        window.draw(highScoreText);
    }

    // Display the pause screen
    if (isPauseScreenVisible)
    {
        sf::Text pauseText;
        pauseText.setFont(font);
        pauseText.setCharacterSize(32);
        pauseText.setFillColor(sf::Color::White);
        pauseText.setPosition(200, 200);
        pauseText.setString("Are you sure you want to quit?\nY for yes, N for no");

        window.draw(pauseText);
    }
    if (isWinScreenVisible)
    {
        sf::Text winText;
        winText.setFont(font);
        winText.setCharacterSize(32);
        winText.setFillColor(sf::Color::White);
        winText.setPosition(200, 200);
        winText.setString("You killed 10 landers! YOU WIN!\nProceed to level " + std::to_string(level + 1) + "?\nY for yes, N for no");

        window.draw(winText);
    }

    // Display the game over screen
    if (isGameOver)
    {
        sf::Text gameOverText;
        gameOverText.setFont(font);
        gameOverText.setCharacterSize(32);
        gameOverText.setFillColor(sf::Color::Red);
        gameOverText.setPosition(200, 200);
        gameOverText.setString("You Died\nPlay again?\nY for yes, N to close the window");

        window.draw(gameOverText);
        resetGame();
    }

    // Display everything on the window
    window.display();
}

// void Game::moveLandersTowardsHumanoids()
// {
//     for (auto &lander : landers)
//     {
//         if (!lander.isDestroyed())
//         {
//             // Check if the lander is attached to a humanoid
//             if (lander.isAttached())
//             {
//                 // Move both the lander and the attached humanoid upwards
//                 lander.moveLanderUp();
//                 lander.getAttachedHumanoid()->moveHumanoidUp();

//                 // Check if both lander and humanoid are out of the window bounds
//                 if (lander.getPosition().y < -lander.landerSprite.getGlobalBounds().height)
//                 {
//                     lander.destroy();
//                 }
//             }
//             else
//             {
//                 // Find the nearest Humanoid
//                 Humanoid *nearestHumanoid = nullptr;
//                 float minDistance = std::numeric_limits<float>::max(); // Initialize with a large value

//                 for (auto &humanoid : humanoids)
//                 {
//                     if (!humanoid.isDestroyed())
//                     {
//                         float distance = lander.getDistance(humanoid.getPosition());
//                         if (distance < minDistance)
//                         {
//                             minDistance = distance;
//                             nearestHumanoid = &humanoid;
//                         }
//                     }
//                 }

//                 if (nearestHumanoid)
//                 {
//                     sf::Vector2f targetPosition = nearestHumanoid->getPosition();

//                     // Check if the lander's x-position is not equal to the humanoid's x-position
//                     if (lander.getPosition().x != targetPosition.x)
//                     {
//                         // Move the lander towards the x-position of the nearest Humanoid
//                         lander.moveTowards(sf::Vector2f(targetPosition.x, lander.getPosition().y));
//                     }
//                     else
//                     {
//                         // If lander is aligned with the humanoid, only move on the x-axis
//                         if (targetPosition.x < lander.getPosition().x)
//                         {
//                             // Move left on the x-axis
//                             lander.moveTowards(sf::Vector2f(targetPosition.x - 1.0f, lander.getPosition().y));
//                         }
//                         else
//                         {
//                             // Move right on the x-axis
//                             lander.moveTowards(sf::Vector2f(targetPosition.x + 1.0f, lander.getPosition().y));
//                         }
//                     }

//                     // Check if the lander touches the humanoid
//                     if (lander.getSprite().getGlobalBounds().intersects(nearestHumanoid->getSprite().getGlobalBounds()))
//                     {
//                         lander.attachToHumanoid(nearestHumanoid);
//                     }
//                 }
//             }
//         }
//     }
// }
