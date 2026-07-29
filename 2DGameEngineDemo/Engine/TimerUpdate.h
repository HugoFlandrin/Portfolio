#pragma once
#include "AComponent.h"

class TimerUpdate : public AComponent
{
public:
	void update(float _deltaTime) override;
};

