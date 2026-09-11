#include "CoopGameOverWatcher.h"
#include "Entity.h"
#include "AliveComponent.h"
#include "SceneManager.h"
#include "AudioManager.h"

namespace {
	// A ship counts as "down" once its death sequence has actually finished
	// playing (matches the delay ShipBehavior itself waits before removing
	// the entity - see ShipBehavior::update()), not the instant hp hits 0.
	// A null ship (solo mode's unused second slot) counts as already down.
	bool isShipDown(Entity* _ship) {
		if (_ship == nullptr) {
			return true;
		}
		AliveComponent* alive = _ship->getComponent<AliveComponent>();
		return alive == nullptr || alive->isDeathSequenceComplete();
	}
}

void CoopGameOverWatcher::init(Entity* _ship1, Entity* _ship2, bool _triggerSceneChange) {
	ship1 = _ship1;
	ship2 = _ship2;
	triggerSceneChange = _triggerSceneChange;
}

void CoopGameOverWatcher::update(float _deltaTime) {
	if (gameOverSoundPlayed) {
		return;
	}

	if (isShipDown(ship1) && isShipDown(ship2)) {
		gameOverSoundPlayed = true;
		// Play the cue BEFORE stopping the music, not after: stopping one
		// sound right before starting another on the same shared device can
		// leave the new one silently failing to actually start (play()
		// returns a success bool this code doesn't check) - once it's
		// already playing, stopping something else afterward doesn't
		// retroactively affect it.
		AudioManager::instance()->playSound("GameOver.mp3");
		AudioManager::instance()->stopMusic();

		if (triggerSceneChange) {
			// Surviving a timed run is the only way to "win" (see EnemySpawner) -
			// both ships going down, in either mode, is always a loss.
			SceneManager::instance()->setLastRunWon(false);
			SceneManager::instance()->requestChangeScene("GameOver");
		}
	}
}
