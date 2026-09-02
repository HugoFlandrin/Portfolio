#pragma once

// Fixed virtual play area (mobile portrait) that every Shmup gameplay
// calculation is expressed in - ship/enemy/bullet speeds, spawn columns,
// off-screen despawn checks, etc. This is independent of the actual desktop
// window size (see ShmupScene::init(), which letterboxes this into whatever
// window it's actually shown in, with black bars filling the rest), so the
// game plays identically regardless of the window's real dimensions.
namespace ShmupConstants {
	constexpr float playAreaWidth = 720.f;
	constexpr float playAreaHeight = 1280.f;

	// Total run length: the whole point is scoring as high as possible
	// before time runs out, not clearing a fixed list of waves - see
	// EnemySpawner, which ends the run once this elapses.
	constexpr float gameDuration = 90.f;
}
