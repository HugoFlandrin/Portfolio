#include "FlagBehavior.h"
#include <SFML/System/RectUtils.hpp>
#include "PlayerBehavior.h"
#include "SceneManager.h"
#include "Render.h"
#include <optional>

void FlagBehavior::init() {
	transformComp = getParent()->getComponent<TransformComponent>();
}

void FlagBehavior::update(float _deltaTime) {
	if (triggered) {
		return;
	}

	AScene* scene = SceneManager::instance()->getCurrentScene();
	auto flagBound = transformComp->getTransform().transformRect(getParent()->getComponent<Render>()->getSprite().getLocalBounds());

	for (auto& it : *scene->getEntities()) {
		if (it->getComponent<PlayerBehavior>() == nullptr) {
			continue;
		}

		auto playerBound = it->getComponent<TransformComponent>()->getTransform().transformRect(it->getComponent<Render>()->getSprite().getLocalBounds());
		if (const auto intersection = sf::findIntersection(flagBound, playerBound)) {
			triggered = true;
			SceneManager::instance()->setLastRunWon(true);
			SceneManager::instance()->requestChangeScene("GameOver");
		}
	}
}
