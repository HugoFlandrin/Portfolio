#include "BoxCollider.h"
#include "PhysicSystem.h"

void BoxCollider::setShape(b2BodyId _body) {
	b2Vec2 localOffset = { offset.x / PhysicSystem::worldScale, offset.y / PhysicSystem::worldScale };
	shape = b2MakeOffsetBox(size.x / PhysicSystem::worldScale / 2.f, size.y / PhysicSystem::worldScale / 2.f, localOffset, b2Rot_identity);
	shapeId = b2CreatePolygonShape(_body, &shapeDef, &shape);
}

void BoxCollider::setSize(b2Vec2 _size) {
	size = _size;
}

void BoxCollider::setOffset(b2Vec2 _offset) {
	offset = _offset;
}

b2Vec2 BoxCollider::getSize() {
	return size;
}