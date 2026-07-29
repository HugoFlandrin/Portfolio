#pragma once
#include <string>
#include <vector>
#include "AScene.h"

class SceneManager
{
	static SceneManager* m_instance;
	std::vector<AScene*> scenes;
	AScene* currentScene;
	AScene* nextScene = nullptr;

	// Score of the scene we just left, captured on scene switch. AScene::score
	// is per-instance, so this is how a scene like GameOver can display the
	// score the player earned in the level that sent them here.
	int lastScore = 0;

	// Whether the run that led to the current end-of-game scene was a win
	// (reached the flag) rather than a death, so that scene can show the
	// right message. Set explicitly by whoever triggers the scene change.
	bool lastRunWon = false;

public:
	SceneManager();
	void addScene(AScene* _scene);
	void removeScene(AScene* _scene);
	void requestChangeScene(std::string _name);
	void processChangeScene();
	AScene* getCurrentScene();
	void setCurrentScene(AScene* _scene);
	int getLastScore();
	void setLastRunWon(bool _won);
	bool getLastRunWon();
	static SceneManager* instance();
};

