#pragma once
#include "AScene.h"

class SettingsScene : public AScene
{
	sf::Font* uiFont;

public:
	SettingsScene();
	void init() override;
};

