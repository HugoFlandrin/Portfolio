#include "PowerUpPickup.h"
#include <SFML/System/RectUtils.hpp>
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "ShipBehavior.h"
#include "AliveComponent.h"
#include "BoxCollider.h"
#include "Render.h"
#include "ResourceManager.h"
#include "AudioManager.h"
#include "ShmupConstants.h"
#include <optional>

namespace {
	// Icon crops from shmupTiles.png (16x16 cells): an orange lightning bolt
	// for FasterFire (reads as "speed", ligne 1 colonne 1), a 3-bullet
	// cluster with the middle one advanced for MultiShot (ligne 2 colonne 4),
	// a blue heater-shield glyph for Shield (ligne 3 colonne 3), and a red
	// medkit cross for Heal (ligne 3 colonne 1).
	constexpr int fasterFireRectX = 0;
	constexpr int fasterFireRectY = 0;
	constexpr int multiShotRectX = 48;
	constexpr int multiShotRectY = 16;
	constexpr int shieldRectX = 32;
	constexpr int shieldRectY = 32;
	constexpr int healRectX = 0;
	constexpr int healRectY = 32;
	constexpr int iconRectSize = 16;

	// Drawn a bit above native size, same reasoning as CoinPickup.
	constexpr float pickupScale = 2.5f;
	// Falls at the same gentle pace as coins - also meant to be easy to
	// grab, not another threat.
	constexpr float fallSpeedFraction = 0.04f;
}

void PowerUpPickup::spawn(AScene* _scene, sf::Vec2f _position, PowerUpType _type) {
	Entity* powerUp = _scene->createEntity();

	powerUp->createComponent<TransformComponent>()->init(_position, { pickupScale, pickupScale });
	powerUp->createComponent<PowerUpPickup>()->init(_type, ShmupConstants::playAreaHeight * fallSpeedFraction);

	int rectX = fasterFireRectX;
	int rectY = fasterFireRectY;
	if (_type == PowerUpType::MultiShot) {
		rectX = multiShotRectX;
		rectY = multiShotRectY;
	}
	else if (_type == PowerUpType::Shield) {
		rectX = shieldRectX;
		rectY = shieldRectY;
	}
	else if (_type == PowerUpType::Heal) {
		rectX = healRectX;
		rectY = healRectY;
	}
	Render* powerUpRender = new Render(
		*ResourceManager::instance()->loadTexture("shmupTiles.png"),
		sf::Rect2i({ rectX, rectY }, { iconRectSize, iconRectSize }),
		{ iconRectSize / 2.f, iconRectSize / 2.f }
	);
	powerUp->addComponent(powerUpRender);

	_scene->addEntity(powerUp);
}

void PowerUpPickup::init(PowerUpType _type, float _fallSpeed) {
	transformComp = getParent()->getComponent<TransformComponent>();
	type = _type;
	fallSpeed = _fallSpeed;
}

void PowerUpPickup::update(float _deltaTime) {
	transformComp->move({ 0.f, fallSpeed * _deltaTime });

	AScene* scene = SceneManager::instance()->getCurrentScene();

	// Missed - despawn once well past the bottom, same threshold/reasoning
	// as CoinPickup (no penalty either - it's a bonus, not a threat).
	if (transformComp->getPosition().y > ShmupConstants::playAreaHeight + 70.f) {
		scene->removeEntity(getParent());
		return;
	}

	sf::Vec2f powerUpPos = transformComp->getPosition();
	sf::Rect2f powerUpBounds(powerUpPos - sf::Vec2f(pickupRadius, pickupRadius), sf::Vec2f(pickupRadius, pickupRadius) * 2.f);

	for (auto& it : *scene->getEntities()) {
		ShipBehavior* ship = it->getComponent<ShipBehavior>();
		if (ship == nullptr) {
			continue;
		}
		// A downed ship shouldn't still be able to grab pickups - same
		// reasoning/consistency as CoinPickup.
		if (AliveComponent* alive = it->getComponent<AliveComponent>()) {
			if (alive->isDying()) {
				continue;
			}
		}

		TransformComponent* shipTransform = it->getComponent<TransformComponent>();
		BoxCollider* shipCollider = it->getComponent<BoxCollider>();
		sf::Vec2f shipSize = shipCollider ? sf::Vec2f(shipCollider->getSize().x, shipCollider->getSize().y) : sf::Vec2f(56.f, 40.f);
		sf::Rect2f shipBounds(shipTransform->getPosition() - shipSize / 2.f, shipSize);

		if (const auto intersection = sf::findIntersection(powerUpBounds, shipBounds)) {
			if (type == PowerUpType::FasterFire) {
				AudioManager::instance()->playSound("PowerUp-ShootSpeed.mp3");
				ship->applyFasterFirePowerUp();
			}
			else if (type == PowerUpType::MultiShot) {
				AudioManager::instance()->playSound("PowerUp-ShootSplit.mp3");
				ship->applyMultiShotPowerUp();
			}
			else if (type == PowerUpType::Shield) {
				AudioManager::instance()->playSound("PowerUp-Shield.mp3");
				ship->applyShieldPowerUp();
			}
			else {
				AudioManager::instance()->playSound("PowerUp-Regeneration.mp3");
				ship->applyHealPowerUp();
			}
			scene->removeEntity(getParent());
			return;
		}
	}
}
