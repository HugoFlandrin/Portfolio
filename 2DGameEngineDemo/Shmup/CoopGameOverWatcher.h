#pragma once
#include "AComponent.h"

class Entity;

// 2-player co-op: neither ship's AliveComponent has a deathScene set (see
// ShmupScene::init()), so a single ship dying no longer ends the run on its
// own - this component is what actually does, once BOTH ships have finished
// their death sequence (see AliveComponent::isDeathSequenceComplete()).
// Solo mode also uses this (ship2=nullptr, isDown() below treats a null ship
// as already down) purely to play the GameOver sound at the moment the run
// ends - the scene change itself is still driven directly by that lone
// ship's own AliveComponent deathScene, so triggerSceneChange=false there to
// avoid a second, redundant requestChangeScene call. No visuals of its own,
// same pattern as EnemySpawner.
class CoopGameOverWatcher : public AComponent
{
	Entity* ship1 = nullptr;
	Entity* ship2 = nullptr;
	bool triggerSceneChange = true;
	bool gameOverSoundPlayed = false;

public:
	void init(Entity* _ship1, Entity* _ship2, bool _triggerSceneChange);
	void update(float _deltaTime) override;
};
