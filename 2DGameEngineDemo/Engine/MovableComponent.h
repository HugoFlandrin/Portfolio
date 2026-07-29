#pragma once
#include <SFML/System/Vec2.hpp>
#include "AComponent.h"

class MovableComponent : public AComponent
{

	float speed;
	sf::Vec2f direction;

public:
	void move(sf::Vec2f _direction);
	void init(sf::Vec2f _direction, float _speed);
	void update(float _deltaTime) override;
	sf::Vec2f getDirection();
	void setDirection(sf::Vec2f _direction);
	void setSpeed(float _speed);
};
