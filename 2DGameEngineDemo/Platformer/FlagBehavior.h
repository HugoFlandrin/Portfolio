#pragma once
#include "AComponent.h"
#include "TransformComponent.h"

// End-of-level trigger: touching it sends the player to the end scene as a win.
class FlagBehavior : public AComponent
{
	TransformComponent* transformComp;
	bool triggered = false;

public:
	void update(float _deltaTime) override;
	void init();
};
