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
	// A proper bullet icon from shmupTiles.png's icon row (16x16 cells,
	// y=16..32, "ligne 2, colonne 1").
	constexpr int bulletRectX = 0;
	constexpr int bulletRectY = 16;
	constexpr int bulletRectSize = 16;

	// Slightly smaller than the crop itself for a snug hitbox - same
	// absolute hitbox size as before this sprite swap (5*3 = 15), so hit
	// detection feel is unchanged even though the sprite/scale changed.
	constexpr float bulletWidth = 10.f;
	constexpr float bulletHeight = 10.f;

	// The new sprite already reads clearly at native size (unlike the old
	// 6x6 ring glyph, which needed heavy magnification just to be visible) -
	// drawn a bit above native size to stay a similar on-screen footprint to
	// before (was 6*3 = 18px).
	constexpr float bulletScale = 1.5f;
}

void BulletBehavior::spawn(AScene* _scene, sf::Vec2f _position, sf::Vec2f _direction, float _speed, float _damage, BulletOwner _owner, int _ownerId) {
	Entity* bullet = _scene->createEntity();

	bullet->createComponent<TransformComponent>()->init(_position, { bulletScale, bulletScale });
	bullet->createComponent<BulletBehavior>()->init(_damage, _owner, _ownerId);
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

void BulletBehavior::init(float _damage, BulletOwner _owner, int _ownerId) {
	transformComp = getParent()->getComponent<TransformComponent>();
	damage = _damage;
	owner = _owner;
	ownerId = _ownerId;
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
		// Credit whoever fired this bullet for the kill (2-player co-op
		// individual scoring - see ShmupEnemyBehavior::update()). Enemy
		// bullets killing the ship don't go through here - the ship isn't
		// scored via ShmupEnemyBehavior at all - so this is only ever
		// reached for owner == Player, but the check stays explicit rather
		// than assumed.
		if (owner == BulletOwner::Player) {
			if (ShmupEnemyBehavior* enemyBehavior = otherEntity->getComponent<ShmupEnemyBehavior>()) {
				enemyBehavior->setKilledByPlayer(ownerId);
			}
		}
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
