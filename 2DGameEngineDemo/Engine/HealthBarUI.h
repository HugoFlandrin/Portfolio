#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include "AComponent.h"

class Entity;

class HealthBarUI : public AComponent
{
	sf::RectangleShape background{ {} };
	sf::RectangleShape fill{ {} };

	float displayedRatio = 1.f;

	// nullptr means "fall back to the scene's camera target" (see init()
	// overload below and update()) - the original single-target behavior,
	// still what the Platformer and Shmup solo mode use.
	Entity* target = nullptr;

public:
	void init(sf::Vec2f _position = { 40.f, 40.f }, sf::Vec2f _size = { 300.f, 36.f });
	// Reads HP straight from _target instead of the scene's single camera
	// target - needed once more than one entity with an AliveComponent wants
	// its own health bar on screen (e.g. Shmup's 2-player co-op), since the
	// scene only ever has one camera target slot for all of them to share.
	void init(Entity* _target, sf::Vec2f _position, sf::Vec2f _size = { 300.f, 36.f });
	void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;
};
