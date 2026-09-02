#include "EnemyBehavior.h"
#include <SFML/System/Time.hpp>
#include "TransformComponent.h"
#include "MovableComponent.h"
#include "PlayerBehavior.h"
#include "AliveComponent.h"
#include "RigidBody.h"
#include "Render.h"
#include "SceneManager.h"
#include <cmath>

void EnemyBehavior::init(float _patrolRange, bool _killable) {
	transformComp = getParent()->getComponent<TransformComponent>();
	patrolRange = _patrolRange;
	killable = _killable;
	spawnX = transformComp->getPosition().x;

	// Patrol needs an initial direction, otherwise the enemy stays idle
	// until it happens to reach a patrol boundary (it starts on spawnX).
	lastDirection = 1;
	getParent()->getComponent<MovableComponent>()->setDirection({ 1.f, 0.f });
}

bool EnemyBehavior::isKillable() {
	return killable;
}

void EnemyBehavior::update(float deltaTime) {
	move();

	// Re-apply damage on a cooldown while the player keeps standing against
	// the enemy, instead of only once on the very first frame of contact.
	if (touchingPlayer && damageTickClock.getElapsedTime().asSeconds() >= damageTickInterval) {
		if (Entity* player = SceneManager::instance()->getCurrentScene()->getCameraTarget()) {
			if (AliveComponent* alive = player->getComponent<AliveComponent>()) {
				alive->takeDamage(damageAmount);
			}
		}
		damageTickClock.restart();
	}
}

void EnemyBehavior::move() {
	MovableComponent* mc = getParent()->getComponent<MovableComponent>();
	TransformComponent* tc = getParent()->getComponent<TransformComponent>();

	if (tc->getPosition().x <= spawnX - patrolRange) {
		mc->setDirection({ 1, 0 });
	}
	else if (tc->getPosition().x >= spawnX + patrolRange) {
		mc->setDirection({ -1, 0 });
	}

	int direction = mc->getDirection().x < 0.f ? -1 : 1;
	if (direction != lastDirection) {
		lastDirection = direction;
		if (Render* render = getParent()->getComponent<Render>()) {
			// The base enemies.png frame faces left, so flip when moving right.
			render->setFlipX(direction > 0);
		}
	}
}

void EnemyBehavior::beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal)
{
	Entity* otherEntity = _other->getParent();

	if (otherEntity->getComponent<PlayerBehavior>() == nullptr) {
		// Not the player: if it's solid geometry beside us (not the ground we're
		// standing on) or another enemy, bounce back the other way instead of
		// stalling (or, for two enemies meeting head-on, deadlocking on the spot).
		RigidBody* otherBody = otherEntity->getComponent<RigidBody>();
		bool isWallLike = otherBody && (otherBody->getBodyType() == b2_staticBody || otherEntity->getComponent<EnemyBehavior>() != nullptr);

		// _normal points from the other object toward us; a mostly-horizontal
		// normal means it's beside us (a wall), not below/above us.
		if (isWallLike && std::abs(_normal.x) > 0.5f) {
			MovableComponent* mc = getParent()->getComponent<MovableComponent>();
			sf::Vec2f direction = mc->getDirection();
			mc->setDirection({ -direction.x, direction.y });
		}
		return;
	}

	RigidBody* playerBody = otherEntity->getComponent<RigidBody>();
	// _normal points from the player toward us: pointing mostly down (+y)
	// means the player is above us, landing on our head.
	bool isStomp = _normal.y > 0.5f;

	if (killable && isStomp) {
		SceneManager::instance()->getCurrentScene()->addScore(50);
		SceneManager::instance()->getCurrentScene()->removeEntity(getParent());
		playerBody->setLinearVelocity({ playerBody->getLinearVelocity().x, -bounceSpeed });
	}
	else {
		otherEntity->getComponent<AliveComponent>()->takeDamage(damageAmount);
		touchingPlayer = true;
		damageTickClock.restart();
	}
}

void EnemyBehavior::endCollision(ACollider* _me, ACollider* _other)
{
	Entity* otherEntity = _other->getParent();
	if (otherEntity->getComponent<PlayerBehavior>() != nullptr) {
		touchingPlayer = false;
	}
}
