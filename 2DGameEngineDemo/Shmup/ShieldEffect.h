#pragma once
#include <SFML/Graphics/CircleShape.hpp>
#include "AComponent.h"

class AScene;
class Entity;
class TransformComponent;
class ShipBehavior;

// Purely visual: a translucent ring that follows its ship every frame for as
// long as ShipBehavior::isShieldActive() reports true (see
// ShipBehavior::applyShieldPowerUp()), then self-removes. The actual damage
// immunity lives on the ship's own AliveComponent (see
// AliveComponent::setInvulnerable()) - this component only ever mirrors that
// state, it doesn't grant or revoke it.
class ShieldEffect : public AComponent
{
	Entity* shipEntity = nullptr;
	TransformComponent* shipTransform = nullptr;
	ShipBehavior* ship = nullptr;
	// sf::CircleShape (VRSFML) has no default constructor - only
	// explicit CircleShape(const Data&) - so it needs an in-class
	// initializer here; init() below fills in the actual radius/colors.
	sf::CircleShape circle{ sf::CircleShape::Data{} };
	float pulseElapsed = 0.f;

public:
	// Spawns a fully-assembled, self-contained effect entity into _scene and
	// adds it - fire-and-forget, nothing needs to hold onto the result.
	// _shipEntity is kept as a raw pointer for this effect's whole lifetime:
	// safe because ships are never removed from the scene once created (see
	// ShipBehavior::update()'s isDying() branch and its own comment on why).
	static void spawn(AScene* _scene, Entity* _shipEntity);

	void init(Entity* _shipEntity);
	void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;
};
