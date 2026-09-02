#pragma once
#include "AComponent.h"
#include "TransformComponent.h"

class CoinBehavior : public AComponent
{
	TransformComponent* transformComp;
	int value = 10;
	// Tight pickup box instead of the raw sprite bounds, which include a lot
	// of transparent padding on both the coin and the player textures.
	float pickupRadius = 18.f;

public:
	void update(float _deltaTime) override;
	void init(int _value = 10);
};
