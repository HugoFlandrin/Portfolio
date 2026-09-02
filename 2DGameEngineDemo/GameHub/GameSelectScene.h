#pragma once
#include "AScene.h"

// Desktop-only entry screen: lets the player pick which game to play
// (Platformer or Shmup) before either one's scene is entered. Both games'
// "GameOver" flow (see GameOverScene) sends the player back here instead of
// straight back into the same game, via its retry button.
class GameSelectScene : public AScene
{
public:
	GameSelectScene();
	void init() override;
};
