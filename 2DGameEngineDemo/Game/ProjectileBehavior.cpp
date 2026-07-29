#include "ProjectileBehavior.h"
#include <SFML/System/RectUtils.hpp>
#include "EnemyBehavior.h"
#include "PlayerBehavior.h"
#include "SceneManager.h"
#include "Render.h"
#include "AliveComponent.h"

void ProjectileBehavior::init() {
	transformComp = getParent()->getComponent<TransformComponent>();
}

void ProjectileBehavior::update(float _deltaTime) {
	SceneManager* sm = SceneManager::instance();

	//Collisions
	for (auto& it : *sm->getCurrentScene()->getEntities()) {
		
		auto globalBound = transformComp->getTransform().transformRect(getParent()->getComponent<Render>()->getSprite().getLocalBounds());

		//Collision with player
		if (it->getComponent<PlayerBehavior>() != nullptr && getParent()->getComponent<ProjectileBehavior>() != nullptr) {
			auto globalBoundPlayer = (*it).getComponent<TransformComponent>()->getTransform().transformRect((*it).getComponent<Render>()->getSprite().getLocalBounds());
			if (const auto intersection = sf::findIntersection(globalBound, globalBoundPlayer)) {
				it->getComponent<AliveComponent>()->takeDamage(1.f);
				if (it->getComponent<AliveComponent>()->getHp() <= 0.f) {
					sm->getCurrentScene()->removeEntity(it);
				}
			}
		}
	}

	//Manage when the entity leave the map
	if (getParent()->getComponent<TransformComponent>()->getPosition().y < 0 || getParent()->getComponent<TransformComponent>()->getPosition().y > 1040) {
		sm->getCurrentScene()->removeEntity(getParent());
	}
}