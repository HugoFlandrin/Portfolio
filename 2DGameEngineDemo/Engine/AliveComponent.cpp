#include "AliveComponent.h"
#include <SFML/System/Time.hpp>
#include "SceneManager.h"
#include <algorithm>

float AliveComponent::getHp() {
	return hp;
}

float AliveComponent::getMaxHp() {
	return maxHp;
}

float AliveComponent::getHpRatio() {
	return maxHp > 0.f ? hp / maxHp : 0.f;
}

void AliveComponent::takeDamage(float _amount) {
	if (invulnerable) {
		// A shield blocks exactly one hit, then breaks (see ShipBehavior::
		// applyShieldPowerUp()) - consumed here rather than on a timer, so
		// whoever grants this just needs to poll isInvulnerable() to notice
		// it was used up.
		invulnerable = false;
		return;
	}

	hp -= _amount;
	sinceLastHit.restart();

	if (hp <= 0.f && !dying) {
		dying = true;
		deathTimer = 0.f;
	}
}

void AliveComponent::update(float _deltaTime) {
	if (dying) {
		deathTimer += _deltaTime;
		if (deathTimer >= deathDelay && !deathScene.empty()) {
			SceneManager::instance()->requestChangeScene(deathScene);
		}
		return;
	}

	if (canRegen && hp < maxHp && sinceLastHit.getElapsedTime().asSeconds() > regenDelay) {
		hp = std::min(maxHp, hp + regenRate * _deltaTime);
	}
}

void AliveComponent::heal(float _amount) {
	hp = std::min(maxHp, hp + _amount);
}

void AliveComponent::init(float _maxHp, std::string _deathScene, float _deathDelay, bool _canRegen) {
	maxHp = _maxHp;
	hp = _maxHp;
	deathScene = _deathScene;
	deathDelay = _deathDelay;
	canRegen = _canRegen;
}
