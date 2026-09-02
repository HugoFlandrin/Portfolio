#include "BulletBehavior.h"
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "RigidBody.h"
#include "PhysicSystem.h"
#include "Render.h"
#include "ResourceManager.h"
#include "ShmupEnemyBehavior.h"
#include "ShipBehavior.h"
#include "AliveComponent.h"
#include "ExplosionEffect.h"
#include "ShmupConstants.h"

namespace {
	// A small ring glyph, not a purpose-made projectile sprite - a first-pass
	// crop that happens to read fine as a bullet at this size, easy to
	// retarget once shmupTiles.png gets a real one.
	constexpr int bulletRectX = 149;
	constexpr int bulletRectY = 8;
	constexpr int bulletRectSize = 6;

	// Was 12x18 - much bigger than the 6x6 sprite crop below once scaled up,
	// so a hit registered well before the bullet visually reached anything.
	// Slightly smaller than the crop itself for a snug hitbox.
	constexpr float bulletWidth = 5.f;
	constexpr float bulletHeight = 5.f;

	// The source crop is a tiny 6x6 icon - barely visible at play area
	// scale, so it's drawn 3x native size, with the physics box scaled to
	// match so the hitbox agrees with what's on screen.
	constexpr float bulletScale = 3.f;
}

void BulletBehavior::spawn(AScene* _scene, sf::Vec2f _position, sf::Vec2f _direction, float _speed, float _damage, BulletOwner _owner) {
	Entity* bullet = _scene->createEntity();

	bullet->createComponent<TransformComponent>()->init(_position, { bulletScale, bulletScale });
	bullet->createComponent<BulletBehavior>()->init(_damage, _owner);
	// Dynamic, not kinematic: Box2D never generates contact/sensor events
	// between two non-dynamic bodies (kinematic-vs-kinematic included), so a
	// bullet would silently pass through enemies otherwise. Gravity is
	// zeroed out since movement here is purely velocity-driven, not
	// force-driven.
	// isSensor=true: a bullet must only ever detect what it hits, never push
	// it - with a normal solid shape, Box2D's solver resolves the deep
	// penetration from a fast bullet with a real impulse, which was flinging
	// enemies off in essentially a random direction (including back upward,
	// toward the player) the instant they got shot. Sensors report the
	// overlap (see PhysicSystem::processSensorEvents()) without any physical
	// response at all.
	bullet->createPhysics({ bulletWidth * bulletScale, bulletHeight * bulletScale }, b2_dynamicBody, true, 1.f, 0.f, true);
	bullet->getComponent<RigidBody>()->setGravityScale(0.f);

	RigidBody* rb = bullet->getComponent<RigidBody>();
	rb->setLinearVelocity({
		_direction.x * _speed / PhysicSystem::worldScale,
		_direction.y * _speed / PhysicSystem::worldScale
	});

	Render* bulletRender = new Render(
		*ResourceManager::instance()->loadTexture("shmupTiles.png"),
		sf::Rect2i({ bulletRectX, bulletRectY }, { bulletRectSize, bulletRectSize }),
		{ bulletRectSize / 2.f, bulletRectSize / 2.f }
	);
	// Purely cosmetic: distinguishes enemy shots (falling, flipped) from the
	// player's (rising) at a glance, since both currently reuse the same
	// symmetric ring sprite.
	bulletRender->setFlipY(_owner == BulletOwner::Enemy);
	bullet->addComponent(bulletRender);

	_scene->addEntity(bullet);
}

void BulletBehavior::init(float _damage, BulletOwner _owner) {
	transformComp = getParent()->getComponent<TransformComponent>();
	damage = _damage;
	owner = _owner;
}

void BulletBehavior::update(float _deltaTime) {
	float y = transformComp->getPosition().y;
	if (y < -40.f || y > ShmupConstants::playAreaHeight + 40.f) {
		SceneManager::instance()->getCurrentScene()->removeEntity(getParent());
	}
}

void BulletBehavior::beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) {
	if (consumed) {
		return;
	}

	Entity* otherEntity = _other->getParent();
	bool isValidTarget = owner == BulletOwner::Player
		? otherEntity->getComponent<ShmupEnemyBehavior>() != nullptr
		: otherEntity->getComponent<ShipBehavior>() != nullptr;
	if (!isValidTarget) {
		return;
	}

	AScene* scene = SceneManager::instance()->getCurrentScene();
	AliveComponent* alive = otherEntity->getComponent<AliveComponent>();
	bool isKillingBlow = false;
	if (alive != nullptr) {
		alive->takeDamage(damage);
		isKillingBlow = alive->getHp() <= 0.f;
	}

	// A killing blow gets the big destruction burst, centered on whatever it
	// just destroyed; anything that survives just gets a small flash right
	// where the bullet hit.
	if (isKillingBlow) {
		ExplosionEffect::spawn(scene, otherEntity->getComponent<TransformComponent>()->getPosition(), ExplosionType::Destruction);
	}
	else {
		ExplosionEffect::spawn(scene, transformComp->getPosition(), ExplosionType::Impact);
	}

	consumed = true;
	scene->removeEntity(getParent());
}

void BulletBehavior::endCollision(ACollider* _me, ACollider* _other) {
}
