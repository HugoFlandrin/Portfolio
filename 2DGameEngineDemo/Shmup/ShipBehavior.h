#pragma once
#include <SFML/System/Clock.hpp>
#include "AComponent.h"
#include "TransformComponent.h"
#include "ICollisionEvent.h"

// Player ship: follows the touch/mouse position (dynamic body chasing a
// target point at a capped speed) or the arrow keys (direct velocity, see
// update()), and fires automatically on a fixed timer. There is no manual
// fire input by design: the player only ever manages movement (held touch on
// mobile, held left mouse button or arrow keys on desktop).
class ShipBehavior : public AComponent, public ICollisionEvent
{
	TransformComponent* transformComp = nullptr;

	// The actual play area (see ShmupScene::init() - NOT necessarily the
	// canvas's declared 720x1280: SDL reports whatever size CSS actually
	// renders the canvas at). Everything below is derived from this at
	// init() time instead of being a hardcoded pixel constant, so the touch
	// target clamp always agrees with the coordinate space TouchInput
	// reports positions in.
	float playAreaWidth = 0.f;
	float playAreaHeight = 0.f;
	static constexpr float shipHalfSizeFraction = 0.05f;

	float moveSpeed = 0.f;
	// The ship is offset above the finger so it stays visible instead of
	// hiding directly under the touch point.
	sf::Vec2f touchOffset;

	sf::Clock fireClock;
	float fireInterval = 0.25f;
	float bulletSpeed = 0.f;
	// 3 hits to kill a regular enemy - see ShmupEnemyBehavior's enemyHp.
	float bulletDamage = 20.f;

	bool movedOnce = false;

public:
	void init(float _playAreaWidth, float _playAreaHeight);
	void update(float _deltaTime) override;
	void beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) override;
	void endCollision(ACollider* _me, ACollider* _other) override;
	// Used by ShmupControlHint to hide itself once the player has proven
	// they've found a way to move, regardless of which input method
	// (keyboard, mouse, or touch) they actually used.
	bool hasMovedOnce() const { return movedOnce; }

private:
	void fireBullet();
};
