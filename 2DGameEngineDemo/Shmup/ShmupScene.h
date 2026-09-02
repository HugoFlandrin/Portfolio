#pragma once
#include "AScene.h"

// Vertical top-down shmup, single scene: fixed camera (the backdrop scrolls,
// not the view - see ScrollingBackground), touch-controlled player ship
// (ShipBehavior) that fires automatically, and a fixed wave table
// (EnemySpawner). Ends via the same "GameOver" scene name / WebGameOverScene
// flow as the platformer.
class ShmupScene : public AScene
{
	// Defaults to true (native desktop, and any other launch path that never
	// calls the setter below, e.g. GameHub's) - see Shmup.cpp for the one
	// case that turns it off (a web launch from a touch device).
	bool showControlHint = true;

public:
	ShmupScene();
	// Must be called before init() to take effect. Whether to show the
	// desktop-only "use the arrow keys" onboarding hint (see
	// ShmupControlHint) - showing it to a touch player would be actively
	// wrong advice, so callers that know they're on a touch device (see
	// Shmup.cpp's web path) should pass false.
	void setShowControlHint(bool _show);
	void init() override;
};
