#include "CoopGameOverWatcher.h"
#include "Entity.h"
#include "AliveComponent.h"
#include "SceneManager.h"

namespace {
	// A ship counts as "down" once its death sequence has actually finished
	// playing (matches the delay ShipBehavior itself waits before removing
	// the entity - see ShipBehavior::update()), not the instant hp hits 0.
	bool isShipDown(Entity* _ship) {
		if (_ship == nullptr) {
			return true;
		}
		AliveComponent* alive = _ship->getComponent<AliveComponent>();
		return alive == nullptr || alive->isDeathSequenceComplete();
	}
}

void CoopGameOverWatcher::init(Entity* _ship1, Entity* _ship2) {
	ship1 = _ship1;
	ship2 = _ship2;
}

void CoopGameOverWatcher::update(float _deltaTime) {
	if (isShipDown(ship1) && isShipDown(ship2)) {
		// Surviving a timed run is the only way to "win" (see EnemySpawner) -
		// both ships going down, in either mode, is always a loss.
		SceneManager::instance()->setLastRunWon(false);
		SceneManager::instance()->requestChangeScene("GameOver");
	}
}
