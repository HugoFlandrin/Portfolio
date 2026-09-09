#pragma once
#include <random>
#include <SFML/System/Clock.hpp>
#include "AComponent.h"
#include "TransformComponent.h"
#include "ICollisionEvent.h"

class AScene;

// The enemy archetypes EnemySpawner picks from at random - each has its own
// hp/fall speed/score/sprite (see ShmupEnemyBehavior.cpp's statsFor()):
// Basic is the baseline, Fast trades hp for speed, Tank is slow but soaks up
// hits, and Shooter lingers and fires back at the ship (see update()).
enum class EnemyType
{
	Basic,
	Fast,
	Tank,
	Shooter
};

// A single descending enemy: constant downward velocity (dynamic body,
// gravity zeroed, set once at spawn - never touched afterward, including on
// hit: see BulletBehavior's sensor bullets), dies when its AliveComponent HP
// reaches 0 from bullet hits and awards score (amount depends on type).
// Physically touching the ship is instant death for both (see
// ShipBehavior::beginCollision()).
class ShmupEnemyBehavior : public AComponent, public ICollisionEvent
{
	TransformComponent* transformComp = nullptr;
	int scoreValue = 100;
	EnemyType type = EnemyType::Basic;

	// Shooter-only: fires a bullet straight down on a fixed timer. Unused
	// (never reaches zero) for every other type.
	sf::Clock fireClock;
	static constexpr float fireInterval = 1.8f;

	// Who dealt the killing blow (2-player co-op individual scoring) -
	// -1 (default) means "not attributed to a specific player", so
	// update()'s death-complete branch falls back to crediting only the
	// shared score, exactly as before this existed. Set by whichever
	// gameplay code actually kills this enemy - see BulletBehavior::
	// beginCollision() and ShipBehavior::beginCollision().
	int killedByPlayer = -1;

	// Rolled once per kill to decide whether a power-up drops - see
	// update()'s death-complete branch and PowerUpPickup.
	std::mt19937 rng{ std::random_device{}() };

public:
	// Spawns a fully-assembled enemy entity into _scene and adds it.
	// _hpMultiplier/_speedMultiplier: infinite mode's long-run difficulty
	// scaling (see EnemySpawner::update()) - applied on top of statsFor()'s
	// base per-type stats so enemies keep growing tougher/faster well past
	// the point where the spawn-rate/type-mix ramp alone caps out, instead of
	// power-ups eventually letting the run be played forever risk-free.
	// Default 1.f each: the timed (Score Attack) mode's call site is
	// unaffected.
	static void spawn(AScene* _scene, sf::Vec2f _position, EnemyType _type = EnemyType::Basic, float _hpMultiplier = 1.f, float _speedMultiplier = 1.f);

	void init(int _scoreValue, EnemyType _type);
	void update(float _deltaTime) override;
	void beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) override;
	void endCollision(ACollider* _me, ACollider* _other) override;
	void setKilledByPlayer(int _playerId) { killedByPlayer = _playerId; }
};
