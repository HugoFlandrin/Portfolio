#include "PlayerBehavior.h"
#include "TransformComponent.h"
#include "ProjectileBehavior.h"
#include "Render.h"
#include "MovableComponent.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include <SFML/Window/Keyboard.hpp>
#include <iostream>
#include <algorithm>

void PlayerBehavior::init() {
	transformComp = getParent()->getComponent<TransformComponent>();
	SceneManager::instance()->getCurrentScene()->setCameraTarget(getParent());
}

bool PlayerBehavior::isGrounded() {
	return !floorContacts.empty();
}

void PlayerBehavior::beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) {
	RigidBody* otherBody = _other->getParent()->getComponent<RigidBody>();
	// _normal points from the other object toward the player: close to
	// straight up (0,-1) means the other object is beneath our feet. A wall
	// beside us gives a mostly-horizontal normal instead, so it won't count -
	// this is what stops the player from jumping off a wall mid-air.
	if (otherBody && otherBody->getBodyType() == b2_staticBody && _normal.y < -0.5f) {
		floorContacts.push_back(_other);
	}
}

void PlayerBehavior::endCollision(ACollider* _me, ACollider* _other) {
	auto it = std::find(floorContacts.begin(), floorContacts.end(), _other);
	if (it != floorContacts.end()) {
		floorContacts.erase(it);
	}
}

void PlayerBehavior::update(float _deltaTime) {
	MovableComponent* mc = getParent()->getComponent<MovableComponent>();
	RigidBody* rb = getParent()->getComponent<RigidBody>();
	SceneManager* sm = SceneManager::instance();

	//Set movement direction (independent from jumping)
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::D)) {
		mc->setDirection({1, 0});
		movedOnce = true;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::A)) {
		mc->setDirection({ -1, 0 });
		movedOnce = true;
	}
	else {
		mc->setDirection({ 0, 0 });
	}

	//Jump, only allowed while touching the ground
	if (isGrounded() && (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Space))) {
		rb->setLinearVelocity({ rb->getLinearVelocity().x, -jumpSpeed });
		jumpedOnce = true;
	}

	TransformComponent* tc = getParent()->getComponent<TransformComponent>();

	if (tc->getPosition().y > 1080) {
		sm->requestChangeScene("GameOver");
	}
}
