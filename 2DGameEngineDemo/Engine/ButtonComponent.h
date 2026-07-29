#pragma once
#include <string>
#include "AComponent.h"

class ButtonComponent : public AComponent
{
	std::string sceneToOpen;

public:
	void init(std::string _sceneName);
	void update(float _deltaTime) override;
	void action();
};

