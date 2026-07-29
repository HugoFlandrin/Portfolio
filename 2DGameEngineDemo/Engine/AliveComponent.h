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

	// Delay between hp hitting 0 and actually triggering the death scene, so
	// UI bound to getHpRatio() (the health bar) has time to visually reach
	// empty first instead of the scene cutting away while it's still mid-lerp.
	// HealthBarUI eases toward its target with an exponential lerp (speed 6),
	// so even a one-hit kill from full health needs ~0.9s to visually decay
	// to an imperceptible sliver (0.6s would still leave ~3% of the bar lit).
	bool dying = false;
	float deathTimer = 0.f;
	static constexpr float deathDelay = 0.9f;

public:
	float getHp();
	float getMaxHp();
	float getHpRatio();
	void takeDamage(float _amount);

	virtual void update(float deltaTime) override;
	void init(float _maxHp, std::string _deathScene = "");
};
