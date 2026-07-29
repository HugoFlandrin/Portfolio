#pragma once
#include "AComponent.h"
#include "TransformComponent.h"

class ProjectileBehavior : public AComponent
{
	TransformComponent* transformComp;

public:
	void update(float _deltaTime) override;
	void init();
};

