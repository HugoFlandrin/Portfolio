#pragma once
#include <random>
#include "AComponent.h"

// Drives Shmup's score-coin spawns (see CoinPickup): a coin at a random
// column, on its own randomized timer, in every mode (timed and infinite,
// solo and 2-player). Deliberately a separate spawner from EnemySpawner
// instead of folded into it - coins are on their own, much rarer cadence,
// unrelated to the enemy wave/difficulty logic, and keeping them apart
// avoids piling more responsibilities onto a component that's already
// juggling enemy type/spacing/difficulty-ramp logic.
class PickupSpawner : public AComponent
{
	std::mt19937 rng{ std::random_device{}() };
	float playAreaWidth = 0.f;
	float spawnY = 0.f;
	float nextSpawnTime = 0.f;

public:
	// _playAreaWidth/_playAreaHeight: the actual play area (see
	// ShmupScene::init()) - same reasoning as EnemySpawner's own init().
	void init(float _playAreaWidth, float _playAreaHeight);
	void update(float _deltaTime) override;
};
