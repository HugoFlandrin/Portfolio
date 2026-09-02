#pragma once
#include <random>
#include "AComponent.h"

// Drives enemy spawns for the whole run: a random enemy type at a random
// (minimum-spaced) column, on a randomized timer, for as long as
// ShmupConstants::gameDuration hasn't elapsed - once it has, the run ends
// (a win: surviving the clock is the actual goal, see ShmupConstants.h).
class EnemySpawner : public AComponent
{
	std::mt19937 rng{ std::random_device{}() };
	float playAreaWidth = 0.f;
	float spawnY = 0.f;
	float nextSpawnTime = 0.f;
	// Rejects a freshly-rolled column too close to the last spawn so two
	// enemies don't land on top of each other - see update().
	float lastSpawnX = -10000.f;

public:
	// _playAreaWidth/_playAreaHeight: the actual play area (see
	// ShmupScene::init()) - spawn columns are laid out as fractions of the
	// width so they stay sensibly placed regardless of what size SDL
	// actually reports for the canvas on a given device.
	void init(float _playAreaWidth, float _playAreaHeight);
	void update(float _deltaTime) override;
};
