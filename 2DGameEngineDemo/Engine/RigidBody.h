#pragma once
#include "box2d/box2d.h"
#include "AComponent.h"

class RigidBody : public AComponent
{
protected:
	b2BodyDef bodyDef;
	b2BodyId body;
	b2BodyType bodyType = b2_staticBody;

public:
	~RigidBody() override;

	void init(bool _fixedRotation);

	void setBodyType(b2BodyType _type);
	void setBodyPosition(b2Vec2 _position);
	void setLinearVelocity(b2Vec2 _linearVelocity);
	void setAngularVelocity(float _angularVelocity);

	b2Vec2 getBodyPosition();
	float getBodyAngle();
	b2Vec2 getLinearVelocity();
	float getAngularVelocity();
	b2BodyId getBody();
	b2BodyType getBodyType();

	virtual void update(float _deltaTime);
};

