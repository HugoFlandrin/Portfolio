#pragma once
#include <SFML/System/Clock.hpp>
#include "AComponent.h"
#include "ICollisionEvent.h"

// Damages the player on contact from any side (unlike EnemyBehavior, there is
// no stomp exception). Used for spike tiles, which can be placed pointing up
// (ground hazard) or flipped to point down (mounted under a platform).
class SpikeBehavior : public AComponent, public ICollisionEvent
{
	float damageAmount = 25.f;
	bool touchingPlayer = false;
	sf::Clock damageTickClock;
	float damageTickInterval = 0.6f;

public:
	void init(float _damageAmount = 25.f);
	void update(float _deltaTime) override;
	void beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) override;
	void endCollision(ACollider* _me, ACollider* _other) override;
};
