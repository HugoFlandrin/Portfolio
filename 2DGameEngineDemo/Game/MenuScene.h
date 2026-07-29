#pragma once
#include "AScene.h"

class MenuScene : public AScene
{
	sf::Font* uiFont;

public:
	MenuScene();
	void init() override;
};

