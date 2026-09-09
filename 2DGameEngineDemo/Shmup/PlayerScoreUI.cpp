#include "PlayerScoreUI.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include <string>

#ifdef __EMSCRIPTEN__
#include "WebBridge.h"
#endif

namespace {
	std::string playerScoreLabel(int _playerId, int _score) {
#ifdef __EMSCRIPTEN__
		bool english = WebBridge::isEnglish();
#else
		bool english = false;
#endif
		return (english ? "P" : "J") + std::to_string(_playerId + 1) + " : " + std::to_string(_score);
	}
}

void PlayerScoreUI::init(TextRenderer* _scoreText, int _playerId) {
	scoreText = _scoreText;
	playerId = _playerId;
	scoreText->setText(playerScoreLabel(playerId, 0).c_str());
	panel.setFillColor(sf::Color(0, 0, 0, 170));
	fitPanelToText();
}

void PlayerScoreUI::update(float _deltaTime) {
	AScene* scene = SceneManager::instance()->getCurrentScene();
	int score = scene->getPlayerScore(playerId);

	if (score != lastScore) {
		lastScore = score;
		scoreText->setText(playerScoreLabel(playerId, score).c_str());
		fitPanelToText();
	}
}

void PlayerScoreUI::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	_target.draw(panel, _states);
}

void PlayerScoreUI::fitPanelToText() {
	sf::Text& text = scoreText->getText();
	sf::Rect2f bounds = text.getLocalBounds();

	const float paddingX = 12.f;
	const float paddingY = 8.f;

	panel.setSize({ bounds.size.x + paddingX * 2.f, bounds.size.y + paddingY * 2.f });
	panel.position = text.position;
	panel.origin = { panel.getSize().x / 2.f, panel.getSize().y / 2.f };
}
