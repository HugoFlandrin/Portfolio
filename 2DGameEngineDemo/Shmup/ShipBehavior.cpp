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

	// Mouse/touch input: dragging the held left mouse button/finger moves the
	// ship by exactly that drag distance, not toward the cursor/finger's
	// absolute position - touching down far from the ship and holding still
	// must not yank it across the screen (see the wasDown handling below).
	// The play area is a fixed virtual resolution letterboxed into the
	// actual window (see ShmupScene::init()/ShmupConstants.h), so a raw
	// window-space mouse/touch position is NOT a world position 1:1 - it
	// must go through the camera's viewport-aware screenToWorld (same as
	// ButtonComponent::update()).
	sf::RenderWindow* window = Engine::instance()->getWindow();
	AScene* scene = SceneManager::instance()->getCurrentScene();
	bool touchDown = TouchInput::instance()->isDown();
	bool mouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	bool isDown = mouseDown || touchDown;
	// Touch takes priority over mouse when both read as active: a touch
	// drag makes the browser synthesize a compatibility mousedown (so
	// mouseDown reads true too), but that synthetic mouse position is only
	// ever set once, at the touch's starting point - it never tracks the
	// finger moving, unlike TouchInput's position which updates on every
	// real TouchMoved event. Preferring mouse here froze rawPosition at the
	// initial touch point for the whole drag, which is why the old
	// chase-to-target model appeared to stop dead after reaching that point
	// and the current drag-delta model never sees a nonzero delta at all.
	sf::Vec2f rawScreenPosition = touchDown
		? TouchInput::instance()->getPosition()
		: sf::Mouse::getPosition(*window).toVec2f();
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
		if (!wasDown) {
			// Just pressed down this frame: there's no prior position yet to
			// diff against, so produce zero movement rather than a huge
			// synthetic delta (or worse, jumping toward wherever this point
			// is). Record it now so next frame's delta is measured from here.
			lastRawPosition = rawPosition;
			rb->setLinearVelocity({ 0.f, 0.f });
		}
		else {
			sf::Vec2f delta = rawPosition - lastRawPosition;
			lastRawPosition = rawPosition;

			// Reconstructing a velocity from a single frame's delta (instead
			// of directly moving the ship by delta) keeps this consistent
			// with the keyboard path below and with how Box2D expects to
			// receive movement, edge-clamp included. Guarded against a
			// near-zero deltaTime: dividing by it would produce a NaN
			// velocity that Box2D bakes into the body's position forever
			// (NaN + anything stays NaN on every future frame) instead of
			// just this one frame being a no-op.
			sf::Vec2f velocity = (_deltaTime > 0.0001f) ? (delta / _deltaTime) : sf::Vec2f{ 0.f, 0.f };

			// Cap to moveSpeed (same max speed as the keyboard path) - a fast
			// swipe/flick would otherwise reconstruct a much higher velocity
			// than the game is balanced around, since this is a 1:1 measure
			// of finger speed with no ceiling.
			float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
			if (speed > moveSpeed) {
				velocity = velocity * (moveSpeed / speed);
			}

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
	}
	else {
		rb->setLinearVelocity({ 0.f, 0.f });
	}
	wasDown = isDown;

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
