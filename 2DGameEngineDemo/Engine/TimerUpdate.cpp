#include "TimerUpdate.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include <SFML/System/Utf8String.hpp>

void TimerUpdate::update(float _deltaTime) {
	SceneManager* sm = SceneManager::instance();
	getParent()->getComponent<TextRenderer>()->setText(("Timer : " + std::to_string(sm->getCurrentScene()->getTimer())).c_str());
}