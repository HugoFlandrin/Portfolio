#include "ShipBehavior.h"
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "RigidBody.h"
#include "PhysicSystem.h"
#include "TouchInput.h"
#include "AliveComponent.h"
#include "ShmupEnemyBehavior.h"
#include "BulletBehavior.h"
#include "ExplosionEffect.h"
#include "Engine.h"
#include <SFML/System/Time.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>

void ShipBehavior::init(float _playAreaWidth, float _playAreaHeight) {
	transformComp = getParent()->getComponent<TransformComponent>();
	// followWithCamera=false: ShmupScene's camera is fixed to the whole play
	// area (see ShmupScene::init()) and the background scrolls instead of the
	// camera (see ScrollingBackground) - the target is still recorded because
	// ShmupEnemyBehavior::update() looks the ship up via getCameraTarget().
	// Actually following it applied AScene::setViewFromPlayer()'s horizontal
	// platformer-scroll formula (player X + a fixed offset) to the ship's
	// small on-screen X range, which sent the camera drifting sideways out of
	// the play area every frame.
	SceneManager::instance()->getCurrentScene()->setCameraTarget(getParent(), false);

	playAreaWidth = _playAreaWidth;
	playAreaHeight = _playAreaHeight;

	// Speeds/offsets are expressed as fractions of the actual play area
	// instead of fixed pixel counts, so the game feels the same regardless
	// of what size SDL actually reports for the canvas on a given device.
	moveSpeed = playAreaHeight * 0.55f;
	touchOffset = { 0.f, -playAreaHeight * 0.055f };
	bulletSpeed = playAreaHeight * 0.586f;
}

void ShipBehavior::update(float _deltaTime) {
	RigidBody* rb = getParent()->getComponent<RigidBody>();

	// Freeze in place the instant the ship is destroyed (collision or a
	// lethal bullet) instead of still reacting to input during
	// AliveComponent's short death delay - otherwise the ship could drift
	// well past wherever it actually died before the scene transition, so
	// the explosion (spawned right at the point of death) would end up
	// nowhere near where the ship visibly ends up.
	if (AliveComponent* alive = getParent()->getComponent<AliveComponent>()) {
		if (alive->isDying()) {
			rb->setLinearVelocity({ 0.f, 0.f });
			return;
		}
	}

	float shipHalfSize = playAreaWidth * shipHalfSizeFraction;

	// Desktop keyboard input: the arrow keys move the ship directly at
	// moveSpeed in whatever direction is held (diagonals normalized), taking
	// priority over mouse/touch so it doesn't fight a stale cursor position.
	sf::Vec2f keyDirection{ 0.f, 0.f };
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Left))  keyDirection.x -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Right)) keyDirection.x += 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Up))    keyDirection.y -= 1.f;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::Down))  keyDirection.y += 1.f;
	bool keyboardActive = keyDirection.x != 0.f || keyDirection.y != 0.f;

	// Mouse/touch input: holding the left mouse button steers the ship
	// toward the cursor, same as a held touch on mobile. The play area is a
	// fixed virtual resolution letterboxed into the actual window (see
	// ShmupScene::init()/ShmupConstants.h), so a raw window-space
	// mouse/touch position is NOT a world position 1:1 anymore - it must go
	// through the camera's viewport-aware screenToWorld (same as
	// ButtonComponent::update()).
	sf::RenderWindow* window = Engine::instance()->getWindow();
	AScene* scene = SceneManager::instance()->getCurrentScene();
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	bool isDown = mouseDown || TouchInput::instance()->isDown();
	sf::Vec2f rawScreenPosition = mouseDown
		? sf::Mouse::getPosition(*window).toVec2f()
		: TouchInput::instance()->getPosition();
	sf::Vec2f rawPosition = scene->getCamera()->screenToWorld(rawScreenPosition, window->getSize().toVec2f());

	if (keyboardActive || isDown) {
		movedOnce = true;
	}

	if (keyboardActive) {
		float length = std::sqrt(keyDirection.x * keyDirection.x + keyDirection.y * keyDirection.y);
		sf::Vec2f direction = keyDirection / length;
		sf::Vec2f velocity = direction * moveSpeed;

		// Stop dead at the play area edges instead of drifting the ship
		// (and by extension the world position clamp everyone else assumes)
		// past them - the mouse/touch path gets this for free from its
		// target clamp, direct velocity control doesn't.
		sf::Vec2f position = transformComp->getPosition();
		if ((position.x <= shipHalfSize && velocity.x < 0.f) || (position.x >= playAreaWidth - shipHalfSize && velocity.x > 0.f)) {
			velocity.x = 0.f;
		}
		if ((position.y <= shipHalfSize && velocity.y < 0.f) || (position.y >= playAreaHeight - shipHalfSize && velocity.y > 0.f)) {
			velocity.y = 0.f;
		}

		rb->setLinearVelocity({
			velocity.x / PhysicSystem::worldScale,
			velocity.y / PhysicSystem::worldScale
		});
	}
	else if (isDown) {
		sf::Vec2f target = rawPosition + touchOffset;
		target.x = std::clamp(target.x, shipHalfSize, playAreaWidth - shipHalfSize);
		target.y = std::clamp(target.y, shipHalfSize, playAreaHeight - shipHalfSize);

		sf::Vec2f toTarget = target - transformComp->getPosition();
		float distance = std::sqrt(toTarget.x * toTarget.x + toTarget.y * toTarget.y);

		if (distance > 1.f) {
			sf::Vec2f direction = toTarget / distance;
			// Capped chase toward the target: covers the remaining distance
			// this frame if that's within moveSpeed, otherwise moves at
			// moveSpeed - avoids overshoot/oscillation right on top of the
			// finger while still feeling immediate.
			float speed = std::min(moveSpeed, distance / _deltaTime);
			rb->setLinearVelocity({
				direction.x * speed / PhysicSystem::worldScale,
				direction.y * speed / PhysicSystem::worldScale
			});
		}
		else {
			rb->setLinearVelocity({ 0.f, 0.f });
		}
	}
	else {
		rb->setLinearVelocity({ 0.f, 0.f });
	}

	if (fireClock.getElapsedTime().asSeconds() >= fireInterval) {
		fireBullet();
		fireClock.restart();
	}
}

void ShipBehavior::fireBullet() {
	AScene* scene = SceneManager::instance()->getCurrentScene();
	sf::Vec2f nose = transformComp->getPosition() + sf::Vec2f(0.f, -playAreaHeight * 0.023f);
	BulletBehavior::spawn(scene, nose, { 0.f, -1.f }, bulletSpeed, bulletDamage);
}

void ShipBehavior::beginCollision(ACollider* _me, ACollider* _other, b2Vec2 _normal) {
	Entity* otherEntity = _other->getParent();
	if (otherEntity->getComponent<ShmupEnemyBehavior>() == nullptr) {
		return;
	}

	// Physically touching an enemy is an instant kill for both, not a
	// graze - no damage ticking, no surviving a bump. AliveComponent::update()
	// still runs its short death delay before the scene transition, so
	// there's a brief beat for the hit to read before cutting to GameOver.
	if (AliveComponent* enemyAlive = otherEntity->getComponent<AliveComponent>()) {
		enemyAlive->takeDamage(enemyAlive->getMaxHp());
	}
	if (AliveComponent* alive = getParent()->getComponent<AliveComponent>()) {
		alive->takeDamage(alive->getMaxHp());
	}

	// Both explode: one burst per ship, at each one's own center.
	AScene* scene = SceneManager::instance()->getCurrentScene();
	ExplosionEffect::spawn(scene, transformComp->getPosition(), ExplosionType::Destruction);
	ExplosionEffect::spawn(scene, otherEntity->getComponent<TransformComponent>()->getPosition(), ExplosionType::Destruction);
}

void ShipBehavior::endCollision(ACollider* _me, ACollider* _other) {
}
