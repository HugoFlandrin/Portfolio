#include "Entity.h"
#include "TransformComponent.h"
#include "Render.h"
#include "box2d/box2d.h"
#include "RigidBody.h"
#include "BoxCollider.h"

Entity::Entity(int _id) : ID(_id) {}

Entity::~Entity()
{
	for (AComponent* c : components)
		delete c;

	components.clear();
}

int Entity::getId() const {
	return ID;
}

void Entity::setId(int _id) {
	ID = _id;
}

std::vector<AComponent*>* Entity::getComponents() {
	return &components;
}

void Entity::update(float _deltaTime) {
	for (auto it : components) {
		(it)->update(_deltaTime);
	}
}

void Entity::draw(sf::RenderTarget& _target, sf::RenderStates _states) const
{
	for (auto it : components) {
		_target.draw(*it, _states);
	}
}

void Entity::addComponent(AComponent* _component) {
	_component->parent = this;
	components.push_back(_component);
}

void Entity::createPhysics(b2Vec2 _size, b2BodyType _bodyType, bool _fixedRotation, float _density, float _friction) {
	RigidBody* rigidBody = createComponent<RigidBody>();
	rigidBody->init(_fixedRotation);
	rigidBody->setBodyType(_bodyType);

	BoxCollider* collider = createComponent<BoxCollider>();
	collider->setSize(_size);
	collider->init(rigidBody);
	collider->setDensity(_density);
	collider->setFriction(_friction);
}