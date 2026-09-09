#include "ShieldEffect.h"
#include <cmath>
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include "ShipBehavior.h"

namespace {
	// Comfortably bigger than the ship's own sprite (64x48 at its render
	// scale) so the bubble reads as enclosing it rather than clipping
	// through its edges.
	constexpr float shieldRadius = 42.f;
	constexpr sf::Color shieldFillColor(90, 200, 255, 60);
	constexpr sf::Color shieldOutlineColor(150, 230, 255, 200);
	constexpr float shieldOutlineThickness = 3.f;
	// Slow continuous shimmer (not a one-shot pulse like ExplosionEffect) -
	// a subtle radius/alpha oscillation so the bubble reads as an energy
	// field rather than a flat, static circle for its whole 8s duration.
	constexpr float pulseSpeed = 4.f;
	constexpr float pulseRadiusAmount = 3.f;
}

void ShieldEffect::spawn(AScene* _scene, Entity* _shipEntity) {
	Entity* effect = _scene->createEntity();
	effect->createComponent<ShieldEffect>()->init(_shipEntity);
	_scene->addEntity(effect);
}

void ShieldEffect::init(Entity* _shipEntity) {
	shipEntity = _shipEntity;
	shipTransform = shipEntity->getComponent<TransformComponent>();
	ship = shipEntity->getComponent<ShipBehavior>();

	circle.setRadius(shieldRadius);
	circle.origin = { shieldRadius, shieldRadius };
	circle.setPointCount(32);
	circle.setFillColor(shieldFillColor);
	circle.setOutlineColor(shieldOutlineColor);
	circle.setOutlineThickness(shieldOutlineThickness);
}

void ShieldEffect::update(float _deltaTime) {
	if (ship == nullptr || !ship->isShieldActive()) {
		SceneManager::instance()->getCurrentScene()->removeEntity(getParent());
		return;
	}

	pulseElapsed += _deltaTime;
	float pulse = std::sin(pulseElapsed * pulseSpeed);
	circle.setRadius(shieldRadius + pulse * pulseRadiusAmount);
	circle.origin = { circle.getRadius(), circle.getRadius() };
	circle.position = shipTransform->getPosition();
}

void ShieldEffect::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	_target.draw(circle, _states);
}
