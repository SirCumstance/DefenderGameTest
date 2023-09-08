#ifndef LASER_H
#define LASER_H

#include <SFML/Graphics.hpp>

class Laser
{
public:
    Laser();
    void fire(sf::Vector2f startPosition);
    void move();
    void draw(sf::RenderWindow &window);
    bool isFired() const;
    void setFired(bool fired);
    sf::RectangleShape shape;
    sf::Vector2f velocity;
    bool fired;

private:
};

#endif // LASER_H
