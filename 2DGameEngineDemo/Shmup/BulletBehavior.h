#pragma once
#include "AComponent.h"
#include "TransformComponent.h"
#include "ICollisionEvent.h"

class AScene;

// Who fired a bullet decides what it damages: a Player bullet looks for a
// ShmupEnemyBehavior on whatever it touches, an Enemy bullet looks for a
// ShipBehavior instead (see BulletBehavior::beginCollision()).
enum class BulletOwner
{
	Player,
	Enemy
};

// A single shot, fired either by the player ship or by a ShmupEnemyBehavior
// of type Shooter. Moves at a constant velocity set once at spawn (dynamic
// body, sensor collider - see spawn()), self-despawns once it drifts off
// either edge of the play area, and damages (plus spawns an ExplosionEffect
// at) the first matching target it touches.
class BulletBehavior : public AComponent, public ICollisionEvent
{
	TransformComponent* transformComp = nullptr;
	float damage = 25.f;
	BulletOwner owner = BulletOwner::Player;
	// Guards against a single bullet damaging two overlapping targets in the
	// same physics step (deletion is deferred to the next scene update, so
	// without this a bullet could still be "live" for a second contact event
	// that arrives before it's actually removed).
	bool consumed = false;

public:
	// Spawns a fully-assembled bullet entity (transform/render/physics/
	// behavior) into _scene and adds it. _direction is expected normalized;
	// speed is in px/s.
	static void spawn(AScene* _scene, sf::Vec2f _position, sf::Vec2f _direction, float _speed, float _damage, BulletOwner _owner = BulletOwner::Player);

	void init(float _damage, BulletOwner _owner);
	void update(float _deltaTime) override;
	void beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) override;
	void endCollision(ACollider* _me, ACollider* _other) override;
};
