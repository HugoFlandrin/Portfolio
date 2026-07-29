#include "CoinBehavior.h"
#include <SFML/System/RectUtils.hpp>
#include "PlayerBehavior.h"
#include "SceneManager.h"
#include "Render.h"
#include "BoxCollider.h"
#include <optional>

void CoinBehavior::init(int _value) {
	transformComp = getParent()->getComponent<TransformComponent>();
	value = _value;
}

void CoinBehavior::update(float _deltaTime) {
	AScene* scene = SceneManager::instance()->getCurrentScene();

	sf::Vec2f coinPos = transformComp->getPosition();
	sf::Rect2f coinBounds(coinPos - sf::Vec2f(pickupRadius, pickupRadius), sf::Vec2f(pickupRadius, pickupRadius) * 2.f);

	for (auto& it : *scene->getEntities()) {
		if (it->getComponent<PlayerBehavior>() == nullptr) {
			continue;
		}

		TransformComponent* playerTransform = it->getComponent<TransformComponent>();
		BoxCollider* playerCollider = it->getComponent<BoxCollider>();
		sf::Vec2f playerSize = playerCollider ? sf::Vec2f(playerCollider->getSize().x, playerCollider->getSize().y) : sf::Vec2f(50.f, 80.f);
		sf::Rect2f playerBounds(playerTransform->getPosition() - playerSize / 2.f, playerSize);

		if (const auto intersection = sf::findIntersection(coinBounds, playerBounds)) {
			scene->addScore(value);
			scene->removeEntity(getParent());
		}
	}
}
