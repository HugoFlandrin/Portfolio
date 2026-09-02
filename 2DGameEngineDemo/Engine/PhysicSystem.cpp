#include "PhysicSystem.h"
#include "Engine.h"
#include "RigidBody.h"
#include "ACollider.h"
#include "TransformComponent.h"
#include "ICollisionEvent.h"
#include "SceneManager.h"
#include <algorithm>

const float PhysicSystem::worldScale = 128.f;

PhysicSystem::PhysicSystem() {
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = {0.f, 9.8f};
	gravity = { 0.f, 9.8f };
	b2WorldId worldId = b2CreateWorld(&worldDef);
	world = worldId;
}

void PhysicSystem::update(float _deltaTime) {
	SceneManager* sm = SceneManager::instance();

	for (Entity* entity : *sm->getCurrentScene()->getEntities()) {
		if (RigidBody* rb = entity->getComponent<RigidBody>()) {
			TransformComponent* transformComp = entity->getComponent<TransformComponent>();
			b2Body_SetTransform(rb->getBody(), { transformComp->getPosition().x / worldScale, transformComp->getPosition().y / worldScale }, b2MakeRot(transformComp->getRotation().asRadians()));
		}
	}

	accumulator += std::min(_deltaTime, maxFrameTime);
	while (accumulator >= fixedTimestep) {
		b2World_Step(world, fixedTimestep, 4);
		accumulator -= fixedTimestep;
		// Must run once per sub-step (see header comment) - not after the loop.
		processContactEvents();
		processSensorEvents();
	}

	for (Entity* entity : *sm->getCurrentScene()->getEntities()) {
		if (RigidBody* rb = entity->getComponent<RigidBody>()) {
			TransformComponent* transformComp = entity->getComponent<TransformComponent>();
			b2Vec2 position = b2Body_GetPosition(rb->getBody());
			b2Rot rotation = b2Body_GetRotation(rb->getBody());

			transformComp->setPosition({position.x * worldScale, position.y * worldScale });
			transformComp->setRotation(sf::radians(b2Rot_GetAngle(rotation)));
		}
	}
}

void PhysicSystem::processContactEvents() {
	SceneManager* sm = SceneManager::instance();
	b2ContactEvents contactEvents = b2World_GetContactEvents(world);

	for (int i = 0; i < contactEvents.beginCount; i++) {
		auto event = contactEvents.beginEvents[i];
		ACollider* colliderA = nullptr;
		ACollider* colliderB = nullptr;
		for (Entity* entity : *sm->getCurrentScene()->getEntities()) {
			if (ACollider* c = entity->getComponent<ACollider>()) {
				if (B2_ID_EQUALS(c->shapeId, event.shapeIdA)) {
					colliderA = c;
				}
				if (B2_ID_EQUALS(c->shapeId, event.shapeIdB)) {
					colliderB = c;
				}
			}
		}
		if (colliderA != nullptr && colliderB != nullptr) {
			// manifold.normal points from shape A to shape B.
			b2Vec2 normalAtoB = event.manifold.normal;
			if (ICollisionEvent* t = colliderA->getParent()->getComponent<ICollisionEvent>()) {
				t->beginCollision(colliderA, colliderB, { -normalAtoB.x, -normalAtoB.y });
			}
			if (ICollisionEvent* t = colliderB->getParent()->getComponent<ICollisionEvent>()) {
				t->beginCollision(colliderB, colliderA, normalAtoB);
			}
		}
	}

	for (int i = 0; i < contactEvents.endCount; i++) {
		auto event = contactEvents.endEvents[i];
		ACollider* colliderA = nullptr;
		ACollider* colliderB = nullptr;
		for (Entity* entity : *sm->getCurrentScene()->getEntities()) {
			if (ACollider* c = entity->getComponent<ACollider>()) {
				if (B2_ID_EQUALS(c->shapeId, event.shapeIdA)) {
					colliderA = c;
				}
				if (B2_ID_EQUALS(c->shapeId, event.shapeIdB)) {
					colliderB = c;
				}
			}
		}
		if (colliderA != nullptr && colliderB != nullptr) {
			if (ICollisionEvent* t = colliderA->getParent()->getComponent<ICollisionEvent>()) {
				t->endCollision(colliderA, colliderB);
			}
			if (ICollisionEvent* t = colliderB->getParent()->getComponent<ICollisionEvent>()) {
				t->endCollision(colliderB, colliderA);
			}
		}
	}
}

void PhysicSystem::processSensorEvents() {
	SceneManager* sm = SceneManager::instance();
	b2SensorEvents sensorEvents = b2World_GetSensorEvents(world);

	for (int i = 0; i < sensorEvents.beginCount; i++) {
		auto event = sensorEvents.beginEvents[i];
		ACollider* sensorCollider = nullptr;
		ACollider* visitorCollider = nullptr;
		for (Entity* entity : *sm->getCurrentScene()->getEntities()) {
			if (ACollider* c = entity->getComponent<ACollider>()) {
				if (B2_ID_EQUALS(c->shapeId, event.sensorShapeId)) {
					sensorCollider = c;
				}
				if (B2_ID_EQUALS(c->shapeId, event.visitorShapeId)) {
					visitorCollider = c;
				}
			}
		}
		if (sensorCollider != nullptr && visitorCollider != nullptr) {
			// Sensor overlaps carry no manifold, so there's no meaningful
			// normal to report - callers of a sensor pair don't use it
			// (see BulletBehavior/ShmupEnemyBehavior::beginCollision()).
			if (ICollisionEvent* t = sensorCollider->getParent()->getComponent<ICollisionEvent>()) {
				t->beginCollision(sensorCollider, visitorCollider, { 0.f, 0.f });
			}
			if (ICollisionEvent* t = visitorCollider->getParent()->getComponent<ICollisionEvent>()) {
				t->beginCollision(visitorCollider, sensorCollider, { 0.f, 0.f });
			}
		}
	}

	for (int i = 0; i < sensorEvents.endCount; i++) {
		auto event = sensorEvents.endEvents[i];
		ACollider* sensorCollider = nullptr;
		ACollider* visitorCollider = nullptr;
		for (Entity* entity : *sm->getCurrentScene()->getEntities()) {
			if (ACollider* c = entity->getComponent<ACollider>()) {
				if (B2_ID_EQUALS(c->shapeId, event.sensorShapeId)) {
					sensorCollider = c;
				}
				if (B2_ID_EQUALS(c->shapeId, event.visitorShapeId)) {
					visitorCollider = c;
				}
			}
		}
		if (sensorCollider != nullptr && visitorCollider != nullptr) {
			if (ICollisionEvent* t = sensorCollider->getParent()->getComponent<ICollisionEvent>()) {
				t->endCollision(sensorCollider, visitorCollider);
			}
			if (ICollisionEvent* t = visitorCollider->getParent()->getComponent<ICollisionEvent>()) {
				t->endCollision(visitorCollider, sensorCollider);
			}
		}
	}
}

b2WorldId* PhysicSystem::getWorld() {
	return &world;
}

void PhysicSystem::destroyBody(b2BodyId _body)
{
	if (b2Body_IsValid(_body))
		b2DestroyBody(_body);
}

void PhysicSystem::reset() {
	b2DestroyWorld(world);

	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = { 0.f, 9.8f };
	gravity = { 0.f, 9.8f };
	b2WorldId worldId = b2CreateWorld(&worldDef);
	world = worldId;
}
