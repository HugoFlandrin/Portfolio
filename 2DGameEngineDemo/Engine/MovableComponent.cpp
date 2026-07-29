#include "MovableComponent.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "RigidBody.h"
#include "PhysicSystem.h"

void MovableComponent::update(float _deltaTime) {
	RigidBody* rb = getParent()->getComponent<RigidBody>();
	rb->setLinearVelocity({ direction.x * speed / PhysicSystem::worldScale, rb->getLinearVelocity().y });
}

void MovableComponent::move(sf::Vec2f _direction) {
	getParent()->getComponent<TransformComponent>()->move(_direction);
}

void MovableComponent::init(sf::Vec2f _direction, float _speed) {
	direction = _direction;
	speed = _speed;
}

sf::Vec2f MovableComponent::getDirection() {
	return direction;
}

void MovableComponent::setDirection(sf::Vec2f _direction) {
	direction = _direction;
}

void MovableComponent::setSpeed(float _speed) {
	speed = _speed;
}