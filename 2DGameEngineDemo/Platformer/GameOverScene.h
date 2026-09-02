#pragma once
#include "AScene.h"

class GameOverScene : public AScene
{

	sf::Font* uiFont;

public:
	GameOverScene();
	void init() override;
};

