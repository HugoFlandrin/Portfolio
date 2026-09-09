#pragma once
#include "AComponent.h"
#include "TransformComponent.h"

class AScene;

// FasterFire is a permanent upgrade to ShipBehavior::fireInterval (lasts the
// rest of the run, floored - see ShipBehavior::applyFasterFirePowerUp());
// MultiShot expires on its own timer; Shield has no timer and instead breaks
// the moment it blocks a hit; Heal is instant, a fixed one-off hp restore
// (see ShipBehavior::applyMultiShotPowerUp()/applyShieldPowerUp()/
// applyHealPowerUp()).
enum class PowerUpType
{
	FasterFire,
	MultiShot,
	Shield,
	Heal
};

// A power-up drop: falls the same way CoinPickup does (see that class's own
// comment for why no Box2D body is needed here either), applies its effect
// to whichever ship intercepts it (see ShipBehavior::applyFasterFirePowerUp()/
// applyMultiShotPowerUp()/applyShieldPowerUp()/applyHealPowerUp()), and
// self-despawns if it falls past the bottom uncaught. Spawned by
// ShmupEnemyBehavior on a kill (see update()) rather than PickupSpawner - a
// power-up is meant as a reward tied to actually landing a kill, not a
// passive drop on its own timer like coins.
class PowerUpPickup : public AComponent
{
	TransformComponent* transformComp = nullptr;
	PowerUpType type = PowerUpType::FasterFire;
	float fallSpeed = 0.f;
	// Tight pickup box instead of the raw sprite bounds - same idea as
	// CoinPickup/Platformer's CoinBehavior.
	float pickupRadius = 20.f;

public:
	// Spawns a fully-assembled pickup entity into _scene and adds it.
	static void spawn(AScene* _scene, sf::Vec2f _position, PowerUpType _type);

	void init(PowerUpType _type, float _fallSpeed);
	void update(float _deltaTime) override;
};
