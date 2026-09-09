#pragma once
#include "AScene.h"

// Vertical top-down shmup, single scene: fixed camera (the backdrop scrolls,
// not the view - see ScrollingBackground), touch-controlled player ship
// (ShipBehavior) that fires automatically, and a fixed wave table
// (EnemySpawner). Ends via the same "GameOver" scene name / WebGameOverScene
// flow as the platformer.
//
// Two instances of this class are registered under different scene names
// (see Shmup.cpp): the default timed "Score Attack" run, and an infinite
// survival run with a ramping difficulty curve - see setInfiniteMode().
class ShmupScene : public AScene
{
	// Defaults to true (native desktop, and any other launch path that never
	// calls the setter below, e.g. GameHub's) - see Shmup.cpp for the one
	// case that turns it off (a web launch from a touch device).
	bool showControlHint = true;

	// Defaults to false (the existing timed Score Attack behavior) - see
	// setInfiniteMode().
	bool infiniteMode = false;

	// Defaults to false (the existing single-ship behavior) - see
	// setTwoPlayer().
	bool twoPlayer = false;

public:
	ShmupScene();
	// Must be called before init() to take effect. Whether to show the
	// desktop-only "use the arrow keys" onboarding hint (see
	// ShmupControlHint) - showing it to a touch player would be actively
	// wrong advice, so callers that know they're on a touch device (see
	// Shmup.cpp's web path) should pass false.
	void setShowControlHint(bool _show);
	// Must be called before init() to take effect. Switches this scene
	// instance from the default timed "survive 90s" run to an endless
	// survival run: no win condition, and EnemySpawner ramps its spawn
	// rate/type mix up with elapsed time instead of holding a flat rate -
	// see EnemySpawner::update().
	void setInfiniteMode(bool _infinite);
	// Must be called before init() to take effect. Spawns a second ship
	// (player 1, WASD/ZQSD - see ShipInputScheme) alongside the first
	// (player 0, switched from arrow keys + touch/mouse to arrow keys only),
	// each with its own health bar, no touch/mouse input at all (see
	// ShipInputScheme's own comment on why), no ally collision (there never
	// was any ship-vs-ship collision logic to begin with), and a death that
	// doesn't end the run by itself - see CoopGameOverWatcher.
	void setTwoPlayer(bool _twoPlayer);
	void init() override;
};
