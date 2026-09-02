#include "RigidBody.h"
#include "Engine.h"
#include "TransformComponent.h"
#include "PhysicSystem.h"

void RigidBody::init(bool _fixedRotation) {
	Engine* engine = Engine::instance();

	bodyDef = b2DefaultBodyDef();
	
	sf::Vec2f position = getParent()->getComponent<TransformComponent>()->getPosition();
	bodyDef.position = { position.x / PhysicSystem::worldScale, position.y / PhysicSystem::worldScale };
	bodyDef.fixedRotation = _fixedRotation;

	body = b2CreateBody(*engine->getPhysicSystem()->getWorld(), &bodyDef);
}

RigidBody::~RigidBody() {
	Engine::instance()->getPhysicSystem()->destroyBody(body);
}

void RigidBody::setBodyType(b2BodyType _type) {
	bodyType = _type;
	b2Body_SetType(body, _type);
}
void RigidBody::setBodyPosition(b2Vec2 _position) {
	bodyDef.position = _position;
}
void RigidBody::setLinearVelocity(b2Vec2 _linearVelocity) {
	b2Body_SetLinearVelocity(body, _linearVelocity);
}
void RigidBody::setAngularVelocity(float _angularVelocity) {
	bodyDef.angularVelocity = _angularVelocity;
}
void RigidBody::setGravityScale(float _scale) {
	b2Body_SetGravityScale(body, _scale);
}

b2Vec2 RigidBody::getBodyPosition() {
	return bodyDef.position;
}
float RigidBody::getBodyAngle() {
	return bodyDef.angularDamping;
}
b2Vec2 RigidBody::getLinearVelocity() {
	return b2Body_GetLinearVelocity(body);
}
float RigidBody::getAngularVelocity() {
	return bodyDef.angularVelocity;
}
b2BodyId RigidBody::getBody() {
	return body;
}
b2BodyType RigidBody::getBodyType() {
	return bodyType;
}

void RigidBody::update(float _deltaTime) {

}