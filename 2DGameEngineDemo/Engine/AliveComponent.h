#pragma once
#include <string>
#include <SFML/System/Clock.hpp>
#include "AComponent.h"

class AliveComponent : public AComponent
{
	float hp;
	float maxHp;
	std::string deathScene;

	sf::Clock sinceLastHit;
	float regenDelay = 2.f;
	float regenRate = 10.f;
	bool canRegen = true;

	// Delay between hp hitting 0 and actually triggering the death scene, so
	// UI bound to getHpRatio() (the health bar) has time to visually reach
	// empty first instead of the scene cutting away while it's still mid-lerp.
	// HealthBarUI eases toward its target with an exponential lerp (speed 6),
	// so even a one-hit kill from full health needs ~0.9s to visually decay
	// to an imperceptible sliver (0.6s would still leave ~3% of the bar lit).
	bool dying = false;
	float deathTimer = 0.f;
	float deathDelay = 0.9f;

public:
	float getHp();
	float getMaxHp();
	float getHpRatio();
	void takeDamage(float _amount);
	// True from the instant hp reaches 0, regardless of whether a
	// deathScene is set - lets gameplay code (e.g. ShipBehavior) freeze
	// whoever's dying in place instead of them drifting away from where
	// they were actually destroyed during the death delay below.
	bool isDying() const { return dying; }
	// True once deathDelay has elapsed since hp reached 0 - for entities
	// with no deathScene (e.g. enemies), this is what actually gates their
	// removal (see ShmupEnemyBehavior::update()) instead of removing them
	// the instant hp hits 0, which used to cut them from the scene before
	// their own destruction explosion had time to play.
	bool isDeathSequenceComplete() const { return dying && deathTimer >= deathDelay; }

	virtual void update(float deltaTime) override;
	// _deathDelay: how long to linger (frozen, mid-explosion) after hp
	// reaches 0 before deathScene is actually requested (if set) / before
	// isDeathSequenceComplete() becomes true - defaults to long enough for
	// HealthBarUI to visibly drain first (see the field's own comment
	// history); pass a shorter value for entities with no health bar to
	// wait for; e.g. enemies just want to roughly match their explosion's
	// runtime (see ShmupEnemyBehavior::spawn()).
	// _canRegen: whether hp passively climbs back toward max after a delay
	// with no hits (see update()) - on by default (the Platformer's design
	// relies on it), but some designs need damage to actually stick, e.g.
	// the Shmup ship (see ShmupScene::init()) - a regenerating health bar
	// there would make it effectively impossible to lose to enemy fire.
	void init(float _maxHp, std::string _deathScene = "", float _deathDelay = 0.9f, bool _canRegen = true);
};
