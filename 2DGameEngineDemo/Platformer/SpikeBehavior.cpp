#include "SpikeBehavior.h"
#include <SFML/System/Time.hpp>
#include "PlayerBehavior.h"
#include "AliveComponent.h"
#include "SceneManager.h"

void SpikeBehavior::init(float _damageAmount) {
	damageAmount = _damageAmount;
}

void SpikeBehavior::update(float _deltaTime) {
	if (touchingPlayer && damageTickClock.getElapsedTime().asSeconds() >= damageTickInterval) {
		if (Entity* player = SceneManager::instance()->getCurrentScene()->getCameraTarget()) {
			if (AliveComponent* alive = player->getComponent<AliveComponent>()) {
				alive->takeDamage(damageAmount);
			}
		}
		damageTickClock.restart();
	}
}

void SpikeBehavior::beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) {
	Entity* otherEntity = _other->getParent();
	if (otherEntity->getComponent<PlayerBehavior>() == nullptr) {
		return;
	}

	if (AliveComponent* alive = otherEntity->getComponent<AliveComponent>()) {
		alive->takeDamage(damageAmount);
	}
	touchingPlayer = true;
	damageTickClock.restart();
}

void SpikeBehavior::endCollision(ACollider* _me, ACollider* _other) {
	Entity* otherEntity = _other->getParent();
	if (otherEntity->getComponent<PlayerBehavior>() != nullptr) {
		touchingPlayer = false;
	}
}
