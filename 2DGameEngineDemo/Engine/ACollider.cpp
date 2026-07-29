#include "ACollider.h"

void ACollider::init(RigidBody* _rigidBody) {
	shapeDef = b2DefaultShapeDef();
	shapeDef.enableContactEvents = true;
	setShape(_rigidBody->getBody());
}

void  ACollider::setDensity(float _density) {
	b2Shape_SetDensity(shapeId, _density, true);
	
}
void  ACollider::setFriction(float _friction) {
	b2Shape_SetFriction(shapeId, _friction);
}

void ACollider::update(float _deltaTime) {}