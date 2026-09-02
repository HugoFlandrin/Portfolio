#include "EnemySpawner.h"
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "ShmupEnemyBehavior.h"
#include "ShmupConstants.h"
#include <array>
#include <cmath>

namespace {
	constexpr float firstSpawnDelay = 3.f;
	constexpr float minSpawnInterval = 0.7f;
	constexpr float maxSpawnInterval = 1.8f;
	// Keeps spawns off the very edges, where an enemy's own half-width would
	// otherwise stick out past the play area.
	constexpr float edgeMarginFraction = 0.12f;
	// An enemy is ~64px wide at 2x scale (see ShmupEnemyBehavior) - reject a
	// freshly-rolled column closer than this to the last spawn so two
	// enemies never land overlapping each other.
	constexpr float minSpawnXDistance = 120.f;
	constexpr int maxSpacingRetries = 5;

	// Basic is the common case; Fast/Tank meaningfully rarer, Shooter
	// rarest since it's the most dangerous type left alone.
	EnemyType pickRandomType(std::mt19937& _rng) {
		static const std::array<EnemyType, 4> types{ EnemyType::Basic, EnemyType::Fast, EnemyType::Tank, EnemyType::Shooter };
		std::discrete_distribution<int> dist({ 50, 25, 15, 10 });
		return types[dist(_rng)];
	}
}

void EnemySpawner::init(float _playAreaWidth, float _playAreaHeight) {
	playAreaWidth = _playAreaWidth;
	spawnY = -_playAreaHeight * 0.031f;
	nextSpawnTime = firstSpawnDelay;
}

void EnemySpawner::update(float _deltaTime) {
	AScene* scene = SceneManager::instance()->getCurrentScene();
	float elapsed = scene->getTimer();

	if (elapsed >= ShmupConstants::gameDuration) {
		// Surviving the clock is the win condition - see ShmupConstants.h.
		SceneManager::instance()->setLastRunWon(true);
		SceneManager::instance()->requestChangeScene("GameOver");
		return;
	}

	if (elapsed < nextSpawnTime) {
		return;
	}

	std::uniform_real_distribution<float> xDist(edgeMarginFraction * playAreaWidth, (1.f - edgeMarginFraction) * playAreaWidth);
	float x = xDist(rng);
	for (int attempt = 0; attempt < maxSpacingRetries && std::abs(x - lastSpawnX) < minSpawnXDistance; attempt++) {
		x = xDist(rng);
	}
	lastSpawnX = x;

	ShmupEnemyBehavior::spawn(scene, { x, spawnY }, pickRandomType(rng));

	std::uniform_real_distribution<float> intervalDist(minSpawnInterval, maxSpawnInterval);
	nextSpawnTime = elapsed + intervalDist(rng);
}
