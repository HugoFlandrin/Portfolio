#include "SceneManager.h"
#include <iostream>
#include "Engine.h";

SceneManager* SceneManager::m_instance = nullptr;

SceneManager::SceneManager() : currentScene(nullptr) {
	if (m_instance == nullptr) {
		m_instance = this;
	}
}

void SceneManager::addScene(AScene* _scene)
{
	scenes.push_back(_scene);
}

void SceneManager::removeScene(AScene* _scene)
{
	scenes.erase(find(scenes.begin(), scenes.end(), _scene));
	delete _scene;
}

void SceneManager::requestChangeScene(std::string _name) {	
	for (int i = 0; i < scenes.size(); i++) {
		if (scenes[i]->getName() == _name) {
			nextScene = scenes[i];
		}
	}
}

void SceneManager::processChangeScene() {
	if (nextScene != nullptr) {

		lastScore = currentScene->getScore();
		lastScoreP1 = currentScene->getPlayerScore(0);
		lastScoreP2 = currentScene->getPlayerScore(1);
		currentScene->end();
		Engine::instance()->getPhysicSystem()->reset();
		currentScene = nextScene;
		currentScene->init();
		currentScene->commitPendingEntities();
		nextScene = nullptr;
	}
}

AScene* SceneManager::getCurrentScene()
{
	if (currentScene) {
		return currentScene;
	}
	else {
		return scenes[0];
	}
}

void SceneManager::setCurrentScene(AScene* _scene) {
	currentScene = _scene;
}

int SceneManager::getLastScore() {
	return lastScore;
}

int SceneManager::getLastScoreP1() {
	return lastScoreP1;
}

int SceneManager::getLastScoreP2() {
	return lastScoreP2;
}

void SceneManager::setLastRunWon(bool _won) {
	lastRunWon = _won;
}

bool SceneManager::getLastRunWon() {
	return lastRunWon;
}

void SceneManager::setLastRunTwoPlayer(bool _twoPlayer) {
	lastRunTwoPlayer = _twoPlayer;
}

bool SceneManager::getLastRunTwoPlayer() {
	return lastRunTwoPlayer;
}

SceneManager* SceneManager::instance()
{
	if (m_instance == nullptr) {
		m_instance = new SceneManager();
	}
	return m_instance;
}
