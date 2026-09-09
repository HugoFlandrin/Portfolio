#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include "AComponent.h"

class TextRenderer;

// One player's individual score (2-player co-op) - a smaller, per-player
// sibling of ScoreUpdate, meant to sit under that player's own health bar
// instead of the shared score at the top-center (see AScene::getPlayerScore(),
// ShmupScene::init()).
class PlayerScoreUI : public AComponent
{
	TextRenderer* scoreText = nullptr;
	int playerId = 0;
	int lastScore = -1;

	// Backing panel behind the score text, same look/ordering trick as
	// ScoreUpdate - must be created (and thus drawn) before scoreText is
	// added to the entity so it renders behind the text instead of over it.
	sf::RectangleShape panel{ {} };

public:
	// _playerId: 0 or 1 - which of AScene::getPlayerScore()'s two slots this
	// instance tracks and how it labels itself ("P1"/"J1" vs "P2"/"J2").
	void init(TextRenderer* _scoreText, int _playerId);
	void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;

private:
	void fitPanelToText();
};
