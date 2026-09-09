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
	// Same idea as lastScore, for Shmup's 2-player co-op individual scores
	// (AScene::getPlayerScore()) - both stay 0 for any scene that never
	// calls addPlayerScore(), which is every scene except that one.
	int lastScoreP1 = 0;
	int lastScoreP2 = 0;

	// Whether the run that led to the current end-of-game scene was a win
	// (reached the flag) rather than a death, so that scene can show the
	// right message. Set explicitly by whoever triggers the scene change.
	bool lastRunWon = false;
	// Whether that run was Shmup's 2-player co-op, so GameOver knows
	// whether to show the P1/P2 breakdown - set defensively at the top of
	// ShmupScene::init() (same pattern as lastRunWon's own reset there),
	// not just at the end, so a later solo run doesn't inherit a stale
	// `true` left over from an earlier 2-player one.
	bool lastRunTwoPlayer = false;

public:
	SceneManager();
	void addScene(AScene* _scene);
	void removeScene(AScene* _scene);
	void requestChangeScene(std::string _name);
	void processChangeScene();
	AScene* getCurrentScene();
	void setCurrentScene(AScene* _scene);
	int getLastScore();
	int getLastScoreP1();
	int getLastScoreP2();
	void setLastRunWon(bool _won);
	bool getLastRunWon();
	void setLastRunTwoPlayer(bool _twoPlayer);
	bool getLastRunTwoPlayer();
	static SceneManager* instance();
};

