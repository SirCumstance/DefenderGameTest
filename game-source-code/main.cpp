#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <iostream>
#include "Lander.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float PLAYER_SPEED = 5.0f;
const float LASER_SPEED = 70.0f;
const float LANDER_SPEED = 2.0f;
const float MISSILE_SPEED = 5.0f;
const int SPAWN_INTERVAL = 2000;   // Spawn a new Lander every 2 seconds
const int MISSILE_INTERVAL = 1000; // Fire a missile every 1 second
int score = 0;					   // Added score variable
int highScore = 0;				   // Added high score variable
bool gameStarted = false;		   // Initial game state
bool quitConfirmation = false;	   // To track if the quit confirmation dialog is shown
bool isSplashScreenVisible = true; // Initial splash screen visibility
bool isPauseScreenVisible = false; // Flag to control the pause screen
bool isGameOver = false;		   // Flag to indicate if the game is over

struct Missile
{
	sf::RectangleShape shape;
	sf::Vector2f velocity;
};

struct Laser
{
	sf::RectangleShape shape;
	sf::Vector2f velocity;
	bool isFired;
};

// void spawnLander(std::vector<Lander> &landers)
// {
// 	Lander lander;
// 	lander.shape.setSize(sf::Vector2f(30, 30));
// 	lander.shape.setFillColor(sf::Color::Red);
// 	lander.shape.setPosition(rand() % (WINDOW_WIDTH - 30), 0);
// 	lander.velocity.x = LANDER_SPEED;
// 	lander.velocity.y = LANDER_SPEED;
// 	lander.isDestroyed() = false;
// 	landers.push_back(lander);
// }

void fireMissile(std::vector<Missile> &missiles, const sf::Vector2f &position, const sf::Vector2f &target)
{
	Missile missile;
	missile.shape.setSize(sf::Vector2f(5, 20));
	missile.shape.setFillColor(sf::Color::Yellow);
	missile.shape.setPosition(position.x + 12, position.y + 30);

	sf::Vector2f direction = target - missile.shape.getPosition();
	float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

	missile.velocity = direction / length * MISSILE_SPEED;

	missiles.push_back(missile);
}
void resetGame(sf::RectangleShape &player, std::vector<Lander> &landers, std::vector<Missile> &missiles)
{
	// Reset player position
	player.setPosition(WINDOW_WIDTH / 2 - 25, WINDOW_HEIGHT - 60);

	// Clear the list of landers
	landers.clear();
	missiles.clear();
	// Reset any other game-related variables if needed
}
int main()
{
	sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Defender-like Game");
	window.setFramerateLimit(60);

	sf::RectangleShape player(sf::Vector2f(50, 50));
	player.setFillColor(sf::Color::Green);
	player.setPosition(WINDOW_WIDTH / 2 - 25, WINDOW_HEIGHT - 60);

	std::vector<Lander> landers;
	std::vector<Missile> missiles;
	std::vector<Laser> lasers;
	sf::Clock spawnTimer;
	sf::Clock missileTimer;

	srand(static_cast<unsigned>(time(0))); // Seed random number generator

	sf::Font font;
	if (!font.loadFromFile("C:/SoftwareIIGame-2/game-source-code/resources/sansation.ttf"))
	{
		// Handle font loading error
	}

	while (window.isOpen())
	{
		sf::Event event;

		// Handle input for starting the game
		if (!gameStarted && !isPauseScreenVisible && !isGameOver)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
			{
				gameStarted = true;
				isSplashScreenVisible = false;
				score = 0; // Reset the score when the game starts
			}
		}
		// Handle input for pausing the game
		else if (gameStarted && !isPauseScreenVisible && !isGameOver)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
			{
				isPauseScreenVisible = true;
			}
		}
		// Handle input for quit confirmation screen
		else if (isPauseScreenVisible)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y))
			{
				isPauseScreenVisible = false;
				gameStarted = false;
				if (score > highScore)
				{
					highScore = score;
				}
				score = 0; // Reset the score
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N))
			{
				isPauseScreenVisible = false;
			}
		}
		// Handle input for game over screen
		else if (isGameOver)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Y))
			{
				isGameOver = false;
				gameStarted = true;
				isSplashScreenVisible = false;
				resetGame(player, landers, missiles);
			}
			else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::N))
			{
				gameStarted = false;
				isSplashScreenVisible = true;
				resetGame(player, landers, missiles);
				window.close();
			}
		}

		// Game logic
		if (gameStarted && !isPauseScreenVisible && !isGameOver)
		{
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						Laser laser;
						laser.shape.setSize(sf::Vector2f(5, 20));
						laser.shape.setFillColor(sf::Color::Cyan);
						laser.shape.setPosition(player.getPosition().x + 22, player.getPosition().y);
						laser.velocity.x = LASER_SPEED;
						laser.isFired = true;
						lasers.push_back(laser);
					}
				}
			}

			// Player movement
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.getPosition().x > 0)
				player.move(-PLAYER_SPEED, 0);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && player.getPosition().x < WINDOW_WIDTH - 50)
				player.move(PLAYER_SPEED, 0);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && player.getPosition().y > 0)
				player.move(0, -PLAYER_SPEED);
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && player.getPosition().y < WINDOW_HEIGHT - 50)
				player.move(0, PLAYER_SPEED);

			// Spawn Landers
			if (spawnTimer.getElapsedTime().asMilliseconds() > SPAWN_INTERVAL)
			{
				landers.push_back(Lander());		   // Spawn a new lander
				landers.back().spawn(window, landers); // Initialize it
				spawnTimer.restart();
			}

			// Fire Missiles from Landers towards the player's ship
			for (size_t i = 0; i < landers.size(); i++)
			{
				if (missileTimer.getElapsedTime().asMilliseconds() > MISSILE_INTERVAL)
				{
					fireMissile(missiles, landers[i].shape.getPosition(), player.getPosition());
					missileTimer.restart();
				}
			}

			// Move Landers
			for (size_t i = 0; i < landers.size(); i++)
			{
				if (!landers[i].isDestroyed())
				{
					landers[i].shape.move(landers[i].velocity);
				}

				// Remove Landers that go out of bounds
				if (landers[i].shape.getPosition().y > WINDOW_HEIGHT)
				{
					landers.erase(landers.begin() + i);
					i--;
				}
			}

			// Move Missiles
			for (size_t i = 0; i < missiles.size(); i++)
			{
				missiles[i].shape.move(missiles[i].velocity);

				// Check for collision with the player's ship
				if (missiles[i].shape.getGlobalBounds().intersects(player.getGlobalBounds()))
				{
					// Player is hit, show "You Died" screen
					isGameOver = true;
				}
				for (size_t i = 0; i < landers.size(); i++)
				{
					if (!landers[i].isDestroyed() && player.getGlobalBounds().intersects(landers[i].shape.getGlobalBounds()))
					{
						// Player is hit by a lander, show "Game Over" screen
						isGameOver = true;
						break; // No need to check further collisions
					}
				}
				// Remove Missiles that go out of bounds
				if (missiles[i].shape.getPosition().y > WINDOW_HEIGHT)
				{
					missiles.erase(missiles.begin() + i);
					i--;
				}
			}

			// Move Lasers
			for (size_t i = 0; i < lasers.size(); i++)
			{
				lasers[i].shape.move(lasers[i].velocity);

				// Remove Lasers that go out of bounds
				if (lasers[i].shape.getPosition().y < 0)
				{
					lasers.erase(lasers.begin() + i);
					i--;
				}
			}

			// Check for collision between lasers and landers
			for (size_t i = 0; i < lasers.size(); i++)
			{
				for (size_t j = 0; j < landers.size(); j++)
				{
					if (!landers[j].isDestroyed() && lasers[i].shape.getGlobalBounds().intersects(landers[j].shape.getGlobalBounds()))
					{
						landers[j].isDestroyed() = true;
						lasers.erase(lasers.begin() + i);
						i--;
						score += 10; // Increase the score when a lander is destroyed
						break;		 // No need to check further lasers for this lander
					}
				}
			}
		}

		// Clear the window and draw the game elements
		window.clear();
		window.draw(player);

		// Draw Landers
		for (const auto &lander : landers)
		{
			if (!lander.isDestroyed())
			{
				window.draw(lander.shape);
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

		// Display and update the score
		sf::Text scoreText;
		scoreText.setFont(font);
		scoreText.setCharacterSize(24);
		scoreText.setFillColor(sf::Color::White);
		scoreText.setPosition(10, 10);
		scoreText.setString("Score: " + std::to_string(score)); // Display the score

		window.draw(scoreText);

		// Display the splash screen
		if (!gameStarted && !isPauseScreenVisible && !isGameOver)
		{
			sf::Text splashText;
			splashText.setFont(font);
			splashText.setCharacterSize(32);
			splashText.setFillColor(sf::Color::White);
			splashText.setPosition(200, 200);
			splashText.setString("Press Space to Start");

			sf::Text highScoreText;
			highScoreText.setFont(font);
			highScoreText.setCharacterSize(24);
			highScoreText.setFillColor(sf::Color::White);
			highScoreText.setPosition(200, 300);
			highScoreText.setString("Highscore: " + std::to_string(highScore));

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

		// Display the game over screen
		if (isGameOver)
		{
			sf::Text gameOverText;
			gameOverText.setFont(font);
			gameOverText.setCharacterSize(32);
			gameOverText.setFillColor(sf::Color::Red);
			gameOverText.setPosition(200, 200);
			gameOverText.setString("You Died\nPlay again?\nY for yes, N for no");
			resetGame(player, landers, missiles);

			window.draw(gameOverText);
		}

		// Display everything on the window
		window.display();
	}

	return 0;
}
