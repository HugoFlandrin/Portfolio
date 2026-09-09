#include "PickupSpawner.h"
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "CoinPickup.h"

namespace {
	// A few seconds into the run before the first coin, then noticeably
	// rarer than any enemy type (see EnemySpawner) - a coin is a bonus to
	// notice and go out of your way for, not a constant stream.
	constexpr float firstSpawnDelay = 5.f;
	constexpr float minSpawnInterval = 6.f;
	constexpr float maxSpawnInterval = 12.f;
	// Keeps spawns off the very edges, same reasoning as EnemySpawner.
	constexpr float edgeMarginFraction = 0.12f;
}

void PickupSpawner::init(float _playAreaWidth, float _playAreaHeight) {
	playAreaWidth = _playAreaWidth;
	spawnY = -_playAreaHeight * 0.031f;
	nextSpawnTime = firstSpawnDelay;
}

void PickupSpawner::update(float _deltaTime) {
	AScene* scene = SceneManager::instance()->getCurrentScene();
	float elapsed = scene->getTimer();

	if (elapsed < nextSpawnTime) {
		return;
	}

	std::uniform_real_distribution<float> xDist(edgeMarginFraction * playAreaWidth, (1.f - edgeMarginFraction) * playAreaWidth);
	float x = xDist(rng);

	CoinPickup::spawn(scene, { x, spawnY });

	std::uniform_real_distribution<float> intervalDist(minSpawnInterval, maxSpawnInterval);
	nextSpawnTime = elapsed + intervalDist(rng);
}
