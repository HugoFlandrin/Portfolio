#pragma once
#include "AComponent.h"
#include "TransformComponent.h"

class AScene;

// Impact: a quick, small flash for a bullet hit that DIDN'T destroy its
// target - see BulletBehavior::beginCollision(). Destruction: the bigger
// burst for whenever a ship (player or enemy) is actually destroyed, be it
// the killing bullet hit or a ship-to-ship collision (see
// ShipBehavior::beginCollision()).
enum class ExplosionType
{
	Impact,
	Destruction
};

// A short, self-removing burst sprite: no frame animation, just its scale
// pulsing 0 -> peak -> 0 over a fixed lifetime (a sine ramp) - the effect
// reads as an expanding-then-fading explosion from a single static sprite.
class ExplosionEffect : public AComponent
{
	TransformComponent* transformComp = nullptr;
	float elapsed = 0.f;
	float duration = 0.f;
	float peakScale = 0.f;

public:
	// Spawns a fully-assembled, self-contained explosion entity into _scene
	// and adds it - fire-and-forget, nothing needs to hold onto the result.
	static void spawn(AScene* _scene, sf::Vec2f _position, ExplosionType _type = ExplosionType::Destruction);

	void init(float _duration, float _peakScale);
	void update(float _deltaTime) override;
};
