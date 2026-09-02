#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include "AComponent.h"

class TextRenderer;

// Shows a scene's remaining time - init()'s _duration minus the scene
// timer's elapsed time (see AScene::startTimer()/getTimer()) - as whole
// seconds, with a dark backing panel for readability over gameplay (same
// styling as ScoreUpdate). Turns red once time is running low as a cheap
// urgency cue. Doesn't end the scene itself - whoever owns the actual
// duration constant is responsible for that once it elapses (see
// EnemySpawner in the Shmup).
class CountdownUI : public AComponent
{
	TextRenderer* timerText = nullptr;
	sf::RectangleShape panel{ {} };
	float duration = 0.f;
	int lastSecondsShown = -1;

public:
	void init(TextRenderer* _timerText, float _duration);
	void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;

private:
	void fitPanelToText();
};
