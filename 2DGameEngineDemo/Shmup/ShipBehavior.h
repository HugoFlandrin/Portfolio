#pragma once
#include <SFML/System/Clock.hpp>
#include "AComponent.h"
#include "TransformComponent.h"
#include "ICollisionEvent.h"

// Player ship: dragged by the touch/mouse position - moves by exactly how
// far the finger/cursor moved since last frame while held down, not toward
// its absolute position (see update()) - or the arrow keys (direct
// velocity), and fires automatically on a fixed timer. There is no manual
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

	// Drives the drag-delta movement (see update()): whether the
	// touch/mouse was already down last frame, and where it was then - a
	// fresh press (wasDown transitioning false->true) deliberately produces
	// zero delta on its first frame, so touching down far from the ship
	// without moving the finger doesn't yank it toward that point.
	bool wasDown = false;
	sf::Vec2f lastRawPosition;

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
