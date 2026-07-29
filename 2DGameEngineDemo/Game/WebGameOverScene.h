#pragma once
#include "AScene.h"

// Web-only replacement for GameOverScene: instead of drawing an in-canvas
// end-of-game menu, it notifies the hosting web page (score + win/loss) and
// stops the engine loop, letting the page show its own HTML popup.
class WebGameOverScene : public AScene
{
public:
	WebGameOverScene();
	void init() override;
};
