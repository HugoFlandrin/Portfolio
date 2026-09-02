#pragma once
#include <SFML/System/Clock.hpp>
#include "AComponent.h"
#include "TransformComponent.h"
#include "ICollisionEvent.h"

class EnemyBehavior : public AComponent, public ICollisionEvent
{
	sf::Clock shootClock;
	TransformComponent* transformComp;
	float spawnX = 0.f;
	float patrolRange = 275.f;
	bool killable = false;
	int lastDirection = 0;

	float damageAmount = 20.f;
	float bounceSpeed = 3.5f;

	bool touchingPlayer = false;
	sf::Clock damageTickClock;
	float damageTickInterval = 0.6f;

public:
	void update(float _deltaTime) override;
	void init(float _patrolRange = 275.f, bool _killable = false);
	void move();
	bool isKillable();
	void beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) override;
	void endCollision(ACollider* _me, ACollider* _other) override;
};
