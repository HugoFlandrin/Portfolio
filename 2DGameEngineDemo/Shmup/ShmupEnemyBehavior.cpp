#include "ShmupEnemyBehavior.h"
#include <SFML/System/Time.hpp>
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "RigidBody.h"
#include "PhysicSystem.h"
#include "Render.h"
#include "ResourceManager.h"
#include "AliveComponent.h"
#include "BulletBehavior.h"
#include "ShmupConstants.h"

namespace {
	constexpr int enemyRectW = 32;
	constexpr int enemyRectH = 24;
	// Slightly smaller than the sprite crop above (was even bigger than it,
	// 32x28) so a bullet reads as landing on the visible hull instead of
	// registering just outside it.
	constexpr float enemyPhysicsWidth = 28.f;
	constexpr float enemyPhysicsHeight = 20.f;
	// ships.png's sprites read as too small at actual play area scale -
	// drawn 2x native size (matches ShipBehavior's own ship), with the
	// physics box scaled down further so the hitbox sits inside the visible
	// sprite instead of poking out past it.
	constexpr float enemyScale = 2.f;

	// Roughly the midpoint of ExplosionEffect's Destruction duration (see
	// ExplosionEffect.cpp) - the enemy lingers, frozen, until then (see
	// update()) instead of vanishing the instant hp hits 0, which used to
	// cut it from the scene before its own explosion had even started.
	constexpr float enemyDeathDelay = 0.18f;

	// Flat penalty (regardless of type) for letting an enemy fall off the
	// bottom of the screen instead of shooting it down - meaningful (about a
	// third of the cheapest kill's reward) without being so harsh that one
	// miss undoes several good hits. AScene::addScore() clamps at 0, so this
	// never takes the player negative.
	constexpr int missedPenalty = 30;

	struct EnemyStats {
		int rectX;
		int rectY;
		float hp;
		// Fraction of the play area's height crossed per second (see
		// ShmupScene::init() for why this isn't a fixed pixel speed).
		float fallSpeedFraction;
		int scoreValue;
	};

	// One distinct gray/metallic hull silhouette per type (ships.png rows
	// 4-6, picked from the sheet's grid so each type reads as a different
	// threat at a glance, not just a recolor) - hp/speed/score chosen so
	// each is a meaningfully different trade-off, not just a stat multiplier:
	// Fast is fragile but hard to hit in time, Tank is slow but soaks up
	// hits, Shooter is a real threat if left alone (see update()'s firing).
	constexpr EnemyStats statsFor(EnemyType _type) {
		switch (_type) {
			case EnemyType::Fast:    return { 66, 136, 30.f, 0.14f, 150 };
			case EnemyType::Tank:    return { 66, 103, 140.f, 0.045f, 250 };
			case EnemyType::Shooter: return { 0, 169, 70.f, 0.05f, 300 };
			case EnemyType::Basic:
			default:                 return { 0, 103, 60.f, 0.07f, 100 };
		}
	}
}

void ShmupEnemyBehavior::spawn(AScene* _scene, sf::Vec2f _position, EnemyType _type) {
	Entity* enemy = _scene->createEntity();
	EnemyStats stats = statsFor(_type);

	enemy->createComponent<TransformComponent>()->init(_position, { enemyScale, enemyScale });
	enemy->createComponent<ShmupEnemyBehavior>()->init(stats.scoreValue, _type);
	enemy->createComponent<AliveComponent>()->init(stats.hp, "", enemyDeathDelay);
	// Dynamic, not kinematic: see BulletBehavior::spawn() - Box2D never
	// generates contact events between two non-dynamic bodies, so neither
	// bullets nor the (also-dynamic) ship would ever detect touching an
	// enemy otherwise.
	enemy->createPhysics({ enemyPhysicsWidth * enemyScale, enemyPhysicsHeight * enemyScale }, b2_dynamicBody, true, 1.f, 0.f);
	enemy->getComponent<RigidBody>()->setGravityScale(0.f);

	RigidBody* rb = enemy->getComponent<RigidBody>();
	rb->setLinearVelocity({ 0.f, (ShmupConstants::playAreaHeight * stats.fallSpeedFraction) / PhysicSystem::worldScale });

	Render* enemyRender = new Render(
		*ResourceManager::instance()->loadTexture("ships.png"),
		sf::Rect2i({ stats.rectX, stats.rectY }, { enemyRectW, enemyRectH }),
		{ enemyRectW / 2.f, enemyRectH / 2.f }
	);
	// All ships in the sheet face up (nose toward the top of the sheet) -
	// flip vertically so enemies visually face down, toward the player.
	enemyRender->setFlipY(true);
	enemy->addComponent(enemyRender);

	_scene->addEntity(enemy);
}

void ShmupEnemyBehavior::init(int _scoreValue, EnemyType _type) {
	transformComp = getParent()->getComponent<TransformComponent>();
	scoreValue = _scoreValue;
	type = _type;
}

void ShmupEnemyBehavior::update(float _deltaTime) {
	AScene* scene = SceneManager::instance()->getCurrentScene();
	AliveComponent* alive = getParent()->getComponent<AliveComponent>();

	if (alive != nullptr && alive->isDying()) {
		// Frozen mid-explosion (see enemyDeathDelay, passed to
		// AliveComponent::init() above) instead of continuing to fall/fire
		// while already destroyed.
		if (RigidBody* rb = getParent()->getComponent<RigidBody>()) {
			rb->setLinearVelocity({ 0.f, 0.f });
		}
		if (alive->isDeathSequenceComplete()) {
			scene->addScore(scoreValue);
			scene->removeEntity(getParent());
		}
		return;
	}

	// Missed by the player - despawn once well past the bottom of the
	// portrait screen instead of falling forever, and dock some score for
	// letting a live threat through.
	if (transformComp->getPosition().y > ShmupConstants::playAreaHeight + 70.f) {
		scene->addScore(-missedPenalty);
		scene->removeEntity(getParent());
		return;
	}

	if (type == EnemyType::Shooter && fireClock.getElapsedTime().asSeconds() >= fireInterval) {
		sf::Vec2f muzzle = transformComp->getPosition() + sf::Vec2f(0.f, 30.f);
		BulletBehavior::spawn(scene, muzzle, { 0.f, 1.f }, ShmupConstants::playAreaHeight * 0.4f, 15.f, BulletOwner::Enemy);
		fireClock.restart();
	}
}

// Ship contact is handled entirely from the ship's side (see
// ShipBehavior::beginCollision()), which already receives this same contact
// event for its own collider - nothing to do here.
void ShmupEnemyBehavior::beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) {
}

void ShmupEnemyBehavior::endCollision(ACollider* _me, ACollider* _other) {
}
