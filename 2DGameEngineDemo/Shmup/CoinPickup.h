#pragma once
#include "AComponent.h"
#include "TransformComponent.h"

class AScene;

// A falling score pickup: constant downward speed (plain TransformComponent
// motion - no Box2D body, since the pickup check below doesn't need physics
// at all), credits whichever ship intercepts it - both its individual score
// and the shared total, see AScene::addPlayerScore() - and self-despawns if
// it falls past the bottom uncaught, same pattern as ShmupEnemyBehavior.
// The overlap check itself is a plain AABB test against every ShipBehavior
// in the scene (sf::findIntersection), mirroring Platformer/CoinBehavior.cpp
// rather than anything Shmup-specific - it already generalizes to any number
// of ships without change.
class CoinPickup : public AComponent
{
	TransformComponent* transformComp = nullptr;
	int value = 50;
	float fallSpeed = 0.f;
	// Tight pickup box instead of the raw sprite bounds, which include a lot
	// of transparent padding - same idea as Platformer's CoinBehavior.
	float pickupRadius = 20.f;

public:
	// Spawns a fully-assembled pickup entity into _scene and adds it.
	static void spawn(AScene* _scene, sf::Vec2f _position);

	void init(int _value, float _fallSpeed);
	void update(float _deltaTime) override;
};
