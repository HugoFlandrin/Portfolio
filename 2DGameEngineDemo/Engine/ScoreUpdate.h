#pragma once
#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include "AComponent.h"

class TextRenderer;

class ScoreUpdate : public AComponent
{
	TextRenderer* scoreText = nullptr;
	int lastScore = 0;
	sf::Clock punchClock;
	bool punching = false;

	// Backing panel behind the score text, same look as the web tutorial
	// hints' panels. Must be created (and thus drawn) before scoreText is
	// added to the entity so it renders behind the text instead of over it.
	sf::RectangleShape panel{ {} };

	static constexpr float punchDuration = 0.25f;
	static constexpr float punchScale = 1.3f;

public:
	void init(TextRenderer* _scoreText);
	void update(float deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;

private:
	void fitPanelToText();
};
