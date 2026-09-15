#include "EnemySpawner.h"
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "ShmupEnemyBehavior.h"
#include "ShmupConstants.h"
#include "AudioManager.h"
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

	// Infinite mode only - everything below ramps the spawn rate/type mix up
	// with elapsed time instead of holding the timed mode's flat rate, so a
	// run starts gentler than Score Attack and gradually grows past it
	// rather than being unplayable from the first spawn. First-pass numbers,
	// meant to be tuned by playtesting like the constants above.
	constexpr float infiniteRampDuration = 240.f;
	constexpr float infiniteMinIntervalStart = 1.2f;
	constexpr float infiniteMaxIntervalStart = 2.4f;
	constexpr float infiniteMinIntervalEnd = 0.5f;
	constexpr float infiniteMaxIntervalEnd = 1.2f;
	// Same 4 types as the timed mode's weights, drifting from that same
	// {50,25,15,10} start toward a meaningfully more dangerous mix.
	constexpr std::array<double, 4> infiniteWeightsStart{ 50.0, 25.0, 15.0, 10.0 };
	constexpr std::array<double, 4> infiniteWeightsEnd{ 20.0, 30.0, 25.0, 25.0 };

	// A second, much longer-running ramp layered on top of the spawn-rate/
	// type-mix one above: individual enemies also get objectively tougher
	// and faster the longer a run goes, not just "a scarier mix of the same
	// fixed archetypes" - without this, once the type-mix ramp caps out
	// (see infiniteRampDuration) a run stacked with power-ups could be
	// played indefinitely with no further escalation. Deliberately much
	// slower than infiniteRampDuration (minutes, not seconds) so early runs
	// still feel like the original balance - this is specifically the
	// long-haul counter-pressure against power-up stacking. First-pass
	// numbers, meant to be tuned by playtesting like everything else here.
	constexpr float infiniteStatRampDuration = 600.f;
	constexpr float infiniteMaxHpMultiplier = 3.f;
	constexpr float infiniteMaxSpeedMultiplier = 2.f;

	// Solo infinite's own spawn-interval curve, rather than scaling the duo
	// one by a flat multiplier: that first attempt started even softer than
	// the timed 90s demo's own flat [0.7s, 1.8s] rate (already well tuned by
	// playtesting - see the non-infinite branch below), leaving early/mid
	// solo runs with long idle stretches waiting on the next enemy. Matching
	// the demo's own rate exactly then turned out too strong right from the
	// opening seconds of an infinite run (no 90s end in sight to play toward,
	// unlike the demo) - starting a bit softer than the demo but ramping
	// down past it gives a gentle opening while still escalating for real.
	constexpr float soloMinIntervalStart = 0.9f;
	constexpr float soloMaxIntervalStart = 2.1f;
	constexpr float soloMinIntervalEnd = 0.45f;
	constexpr float soloMaxIntervalEnd = 1.0f;
	// The long-haul HP/speed ramp's ceiling is still pulled in for solo
	// (applied as a scale on how far it climbs *above* 1x, not a flat
	// number, so it still starts at the same 1x baseline as duo) - a lone
	// ship can only ever be in one place to catch a falling power-up, so it
	// can't out-stack duo's power-up economy to match duo's own ceiling.
	constexpr float soloStatRampScale = 0.65f;

	float lerp(float _a, float _b, float _t) {
		return _a + (_b - _a) * _t;
	}

	EnemyType pickRandomTypeRamped(std::mt19937& _rng, float _difficultyT) {
		static const std::array<EnemyType, 4> types{ EnemyType::Basic, EnemyType::Fast, EnemyType::Tank, EnemyType::Shooter };
		std::array<double, 4> weights;
		for (std::size_t i = 0; i < weights.size(); i++) {
			weights[i] = lerp(static_cast<float>(infiniteWeightsStart[i]), static_cast<float>(infiniteWeightsEnd[i]), _difficultyT);
		}
		std::discrete_distribution<int> dist(weights.begin(), weights.end());
		return types[dist(_rng)];
	}

	// Shared by both modes - purely spatial (keeps two spawns from landing on
	// top of each other), nothing here depends on timed vs. infinite.
	float pickSpawnX(std::mt19937& _rng, float _playAreaWidth, float _lastSpawnX) {
		std::uniform_real_distribution<float> xDist(edgeMarginFraction * _playAreaWidth, (1.f - edgeMarginFraction) * _playAreaWidth);
		float x = xDist(_rng);
		for (int attempt = 0; attempt < maxSpacingRetries && std::abs(x - _lastSpawnX) < minSpawnXDistance; attempt++) {
			x = xDist(_rng);
		}
		return x;
	}
}

void EnemySpawner::init(float _playAreaWidth, float _playAreaHeight, bool _infiniteMode, bool _soloDifficulty) {
	playAreaWidth = _playAreaWidth;
	spawnY = -_playAreaHeight * 0.031f;
	nextSpawnTime = firstSpawnDelay;
	infiniteMode = _infiniteMode;
	soloDifficulty = _soloDifficulty;
}

void EnemySpawner::update(float _deltaTime) {
	AScene* scene = SceneManager::instance()->getCurrentScene();
	float elapsed = scene->getTimer();

	if (infiniteMode) {
		if (elapsed < nextSpawnTime) {
			return;
		}

		float x = pickSpawnX(rng, playAreaWidth, lastSpawnX);
		lastSpawnX = x;

		float difficultyT = std::min(elapsed / infiniteRampDuration, 1.f);
		float statScaleT = std::min(elapsed / infiniteStatRampDuration, 1.f);
		float statScale = soloDifficulty ? soloStatRampScale : 1.f;
		float hpMultiplier = 1.f + (lerp(1.f, infiniteMaxHpMultiplier, statScaleT) - 1.f) * statScale;
		float speedMultiplier = 1.f + (lerp(1.f, infiniteMaxSpeedMultiplier, statScaleT) - 1.f) * statScale;
		ShmupEnemyBehavior::spawn(scene, { x, spawnY }, pickRandomTypeRamped(rng, difficultyT), hpMultiplier, speedMultiplier);

		float minInterval = soloDifficulty
			? lerp(soloMinIntervalStart, soloMinIntervalEnd, difficultyT)
			: lerp(infiniteMinIntervalStart, infiniteMinIntervalEnd, difficultyT);
		float maxInterval = soloDifficulty
			? lerp(soloMaxIntervalStart, soloMaxIntervalEnd, difficultyT)
			: lerp(infiniteMaxIntervalStart, infiniteMaxIntervalEnd, difficultyT);
		std::uniform_real_distribution<float> intervalDist(minInterval, maxInterval);
		nextSpawnTime = elapsed + intervalDist(rng);
		return;
	}

	if (elapsed >= ShmupConstants::gameDuration) {
		// Surviving the clock is the win condition - see ShmupConstants.h.
		// Victory instead of the CoopGameOverWatcher's GameOver cue - this
		// branch is only ever reached on a win (a loss always goes through
		// that watcher instead - see its own comment), so the two sounds
		// never compete for the same moment.
		SceneManager::instance()->setLastRunWon(true);
		// Play before stopping the music, not after - see
		// CoopGameOverWatcher.cpp's own comment on this exact ordering.
		AudioManager::instance()->playSound("Victory.mp3");
		AudioManager::instance()->stopMusic();
		SceneManager::instance()->requestChangeScene("GameOver");
		return;
	}

	if (elapsed < nextSpawnTime) {
		return;
	}

	float x = pickSpawnX(rng, playAreaWidth, lastSpawnX);
	lastSpawnX = x;

	ShmupEnemyBehavior::spawn(scene, { x, spawnY }, pickRandomType(rng));

	std::uniform_real_distribution<float> intervalDist(minSpawnInterval, maxSpawnInterval);
	nextSpawnTime = elapsed + intervalDist(rng);
}
