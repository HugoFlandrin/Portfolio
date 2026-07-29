#pragma once
#include "ACollider.h"

class ICollisionEvent
{
public:

	// _normal points from _other toward _me in world space (unit vector), taken
	// straight from Box2D's contact manifold - e.g. ~(0,-1) means _other is
	// directly below _me (a floor), ~(1,0)/(−1,0) means _other is beside _me
	// (a wall). Reliable even for corner/grazing contact, unlike a
	// position-based guess.
	virtual void beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) = 0;
	virtual void endCollision(ACollider* _me, ACollider* _other) = 0;
};

