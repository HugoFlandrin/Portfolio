#include "ACollider.h"

void ACollider::init(RigidBody* _rigidBody, bool _isSensor) {
	shapeDef = b2DefaultShapeDef();
	shapeDef.enableContactEvents = true;
	shapeDef.isSensor = _isSensor;
	// Box2D disables sensor overlap reporting by default on EVERY shape, be
	// it the sensor itself or whatever it's meant to detect ("this applies
	// to sensors and non-sensors" per b2ShapeDef::enableSensorEvents) - so
	// without this, a sensor bullet would silently stop generating any hit
	// at all instead of just losing its physical push (confirmed: this was
	// the actual cause of bullets no longer damaging enemies).
	shapeDef.enableSensorEvents = true;
	setShape(_rigidBody->getBody());
}

void  ACollider::setDensity(float _density) {
	b2Shape_SetDensity(shapeId, _density, true);
	
}
void  ACollider::setFriction(float _friction) {
	b2Shape_SetFriction(shapeId, _friction);
}

void ACollider::update(float _deltaTime) {}