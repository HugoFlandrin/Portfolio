#pragma once
#include "ACollider.h"
#include "box2d/box2d.h"

class BoxCollider : public ACollider
{
protected:
	b2Polygon shape;
	b2Vec2 size;
	b2Vec2 offset = { 0.f, 0.f };

public:

	void setShape(b2BodyId _body) override;
	void setSize(b2Vec2 _size);
	// Offsets the collider from the entity's TransformComponent origin (in
	// pixels), for shapes that don't fill their full texture rect - e.g. a
	// spike sprite with transparent padding above the actual spike graphic.
	void setOffset(b2Vec2 _offset);
	b2Vec2 getSize();
};

