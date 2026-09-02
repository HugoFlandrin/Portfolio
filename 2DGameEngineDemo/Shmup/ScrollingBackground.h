#pragma once
#include "AComponent.h"
#include "TransformComponent.h"

// Two stacked copies of the same ground-tile crop, both scrolling downward
// and looping (whichever tile scrolls fully past the bottom jumps back above
// the other one) - a simple two-tile scroller, cheaper than tiling the whole
// sheet and enough for a screen-sized backdrop.
class ScrollingBackground : public AComponent
{
	TransformComponent* transformComp = nullptr;
	float scrollSpeed = 0.f;
	float tileHeight = 0.f;
	float playAreaHeight = 0.f;

public:
	// _playAreaHeight: the actual play area (see ShmupScene::init()), used
	// for the wrap check below instead of a hardcoded design height.
	void init(float _tileHeight, float _playAreaHeight);
	void update(float _deltaTime) override;
};
