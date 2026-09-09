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
	// _groupIndex: Box2D collision filter group (b2Filter::groupIndex) -
	// 0 (default) uses Box2D's normal category/mask filtering, unchanged
	// from before this parameter existed. A shared NEGATIVE value on two
	// colliders makes Box2D skip collision between exactly that pair
	// entirely (no contact/sensor event, no physical push), regardless of
	// what else they'd otherwise collide with - the standard Box2D idiom
	// for "these specific bodies should ignore each other" (e.g. two
	// player-controlled ships that must never push one another around or
	// block one another, dead or alive - see ShmupScene::init()).
	void init(RigidBody* _rigidBody, bool _isSensor = false, int _groupIndex = 0);
	virtual void setShape(b2BodyId _body) = 0;

	void setDensity(float _density);
	void setFriction(float _friction);
	// Turns this shape physically inert - it stops colliding with
	// literally everything (contact and sensor events both go quiet) -
	// without destroying the body/shape itself, so the entity keeps
	// existing and rendering exactly as before. For an entity that should
	// become a purely visual leftover, e.g. a downed co-op ship (see
	// ShipBehavior::update()) that must stop being a physics-relevant
	// obstacle enemies collide with, but is deliberately never removed
	// from the scene (see that same call site's own comment for why).
	// Box2D v3 has no runtime sensor/solid toggle, so this works by
	// zeroing the shape's collision mask instead (b2Shape_SetFilter) -
	// functionally equivalent for this purpose and, unlike a group-index
	// trick, not limited to a specific other shape/group.
	void disableCollision();
	virtual void update(float _deltaTime);
	friend class PhysicSystem;
};

