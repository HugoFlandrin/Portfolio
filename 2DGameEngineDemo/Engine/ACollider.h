#pragma once
#include "box2d/box2d.h"
#include "AComponent.h"
#include "RigidBody.h"

class ACollider : public AComponent
{
protected:
	b2ShapeDef shapeDef;
	b2ShapeId shapeId;

public:
	void init(RigidBody* _rigidBody);
	virtual void setShape(b2BodyId _body) = 0;

	void setDensity(float _density);
	void setFriction(float _friction);
	virtual void update(float _deltaTime);
	friend class PhysicSystem;
};

