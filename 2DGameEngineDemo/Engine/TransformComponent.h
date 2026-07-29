#pragma once
#include <SFML/Graphics/Transformable.hpp>
#include "AComponent.h"

class TransformComponent : public AComponent, public sf::Transformable
{

public:
	virtual void update(float _deltaTime) override;
	void init(	sf::Vec2f _position = sf::Vec2f(0.f, 0.f),
				sf::Vec2f _scale = sf::Vec2f(1.f, 1.f),
				sf::Angle _rotation = sf::Angle(sf::radians(0.f)));

	// Compatibility accessors: VRSFML's sf::Transformable exposes
	// position/scale/origin/rotation as plain public fields instead of
	// setter/getter methods - these wrappers keep the rest of the codebase
	// (which calls getPosition()/setPosition()/move()/etc.) unchanged.
	sf::Vec2f getPosition() const { return position; }
	void setPosition(sf::Vec2f _position) { position = _position; }
	void move(sf::Vec2f _offset) { position += _offset; }
	sf::Vec2f getScale() const { return scale; }
	void setScale(sf::Vec2f _scale) { scale = _scale; }
	sf::Vec2f getOrigin() const { return origin; }
	void setOrigin(sf::Vec2f _origin) { origin = _origin; }
	sf::Angle getRotation() const { return rotation; }
	void setRotation(sf::Angle _rotation) { rotation = _rotation; }
};
