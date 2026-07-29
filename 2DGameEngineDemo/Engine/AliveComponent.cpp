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
	hp -= _amount;
	sinceLastHit.restart();

	if (hp <= 0.f && !deathScene.empty() && !dying) {
		dying = true;
		deathTimer = 0.f;
	}
}

void AliveComponent::update(float _deltaTime) {
	if (dying) {
		deathTimer += _deltaTime;
		if (deathTimer >= deathDelay) {
			SceneManager::instance()->requestChangeScene(deathScene);
		}
		return;
	}

	if (hp < maxHp && sinceLastHit.getElapsedTime().asSeconds() > regenDelay) {
		hp = std::min(maxHp, hp + regenRate * _deltaTime);
	}
}

void AliveComponent::init(float _maxHp, std::string _deathScene) {
	maxHp = _maxHp;
	hp = _maxHp;
	deathScene = _deathScene;
}
