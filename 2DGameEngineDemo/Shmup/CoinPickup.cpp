#include "CoinPickup.h"
#include <SFML/System/RectUtils.hpp>
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "ShipBehavior.h"
#include "AliveComponent.h"
#include "BoxCollider.h"
#include "Render.h"
#include "ResourceManager.h"
#include "ShmupConstants.h"
#include <optional>

namespace {
	// A proper gold coin icon out of tiles.png (Platformer's tileset, copied
	// into Shmup/Assets so it's preloaded here too - see ResourceManager's
	// desktop/web path resolution) - a much bigger sheet than shmupTiles.png,
	// nominally 64x64 cells vs 16x16, but the coin glyph itself only occupies
	// roughly the center 38x40 of its cell. The full 64x64 cell isn't used
	// as the crop: its very top row bleeds one line of the icon above's
	// border (tiles.png has no padding between cells), which showed up as a
	// dark sliver on the rendered coin - this rect is inset to the glyph's
	// own bounding box (with a few px of margin on every side, verified
	// pixel-by-pixel to be fully transparent) so nothing from a neighboring
	// cell is ever sampled.
	constexpr int coinRectX = 8;
	constexpr int coinRectY = 138;
	constexpr int coinRectSize = 48;

	// tiles.png's cells are much bigger than shmupTiles.png's - scaled down
	// so the coin reads at the same on-screen footprint as before (was
	// 16px * 2.5 = 40px), with the pickup radius below still sized to
	// roughly match what's on screen.
	constexpr float coinScale = 40.f / coinRectSize;

	constexpr int coinValue = 50;
	// Falls noticeably slower than every enemy type (see ShmupEnemyBehavior's
	// statsFor()) - a coin should be easy to line up and grab, not another
	// threat to dodge.
	constexpr float coinFallSpeedFraction = 0.04f;
}

void CoinPickup::spawn(AScene* _scene, sf::Vec2f _position) {
	Entity* coin = _scene->createEntity();

	coin->createComponent<TransformComponent>()->init(_position, { coinScale, coinScale });
	coin->createComponent<CoinPickup>()->init(coinValue, ShmupConstants::playAreaHeight * coinFallSpeedFraction);

	Render* coinRender = new Render(
		*ResourceManager::instance()->loadTexture("tiles.png"),
		sf::Rect2i({ coinRectX, coinRectY }, { coinRectSize, coinRectSize }),
		{ coinRectSize / 2.f, coinRectSize / 2.f }
	);
	coin->addComponent(coinRender);

	_scene->addEntity(coin);
}

void CoinPickup::init(int _value, float _fallSpeed) {
	transformComp = getParent()->getComponent<TransformComponent>();
	value = _value;
	fallSpeed = _fallSpeed;
}

void CoinPickup::update(float _deltaTime) {
	transformComp->move({ 0.f, fallSpeed * _deltaTime });

	AScene* scene = SceneManager::instance()->getCurrentScene();

	// Missed - despawn once well past the bottom instead of falling
	// forever, same threshold as ShmupEnemyBehavior. No score penalty for
	// missing a coin, unlike letting an enemy through - it's a bonus, not a
	// threat.
	if (transformComp->getPosition().y > ShmupConstants::playAreaHeight + 70.f) {
		scene->removeEntity(getParent());
		return;
	}

	sf::Vec2f coinPos = transformComp->getPosition();
	sf::Rect2f coinBounds(coinPos - sf::Vec2f(pickupRadius, pickupRadius), sf::Vec2f(pickupRadius, pickupRadius) * 2.f);

	for (auto& it : *scene->getEntities()) {
		ShipBehavior* ship = it->getComponent<ShipBehavior>();
		if (ship == nullptr) {
			continue;
		}
		// A downed ship (see ShipBehavior::update()) shouldn't still be able
		// to grab pickups - it's already physically/visually inert to
		// everything else, this keeps that consistent.
		if (AliveComponent* alive = it->getComponent<AliveComponent>()) {
			if (alive->isDying()) {
				continue;
			}
		}

		TransformComponent* shipTransform = it->getComponent<TransformComponent>();
		BoxCollider* shipCollider = it->getComponent<BoxCollider>();
		sf::Vec2f shipSize = shipCollider ? sf::Vec2f(shipCollider->getSize().x, shipCollider->getSize().y) : sf::Vec2f(56.f, 40.f);
		sf::Rect2f shipBounds(shipTransform->getPosition() - shipSize / 2.f, shipSize);

		if (const auto intersection = sf::findIntersection(coinBounds, shipBounds)) {
			scene->addPlayerScore(ship->getPlayerId(), value);
			scene->removeEntity(getParent());
			return;
		}
	}
}
