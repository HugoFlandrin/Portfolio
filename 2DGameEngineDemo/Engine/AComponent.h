#pragma once
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
class Entity;

class AComponent
{
	Entity* parent;

	friend Entity;

public:
	virtual ~AComponent() = default;

	Entity* getParent() const;

	virtual void update(float _deltaTime) = 0;
	virtual void draw(sf::RenderTarget& _target, sf::RenderStates _states) const;
};
