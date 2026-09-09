#pragma once
#include "AComponent.h"

class Entity;

// 2-player co-op only: neither ship's AliveComponent has a deathScene set
// (see ShmupScene::init()), so a single ship dying no longer ends the run on
// its own - this component is what actually does, once BOTH ships have
// finished their death sequence (see AliveComponent::isDeathSequenceComplete()).
// No visuals of its own, same pattern as EnemySpawner.
class CoopGameOverWatcher : public AComponent
{
	Entity* ship1 = nullptr;
	Entity* ship2 = nullptr;

public:
	void init(Entity* _ship1, Entity* _ship2);
	void update(float _deltaTime) override;
};
