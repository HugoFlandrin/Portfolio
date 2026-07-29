#include "ScoreUpdate.h"
#include <SFML/System/Time.hpp>
#include <SFML/System/Utf8String.hpp>
#include "SceneManager.h"
#include "TextRenderer.h"
#include <cmath>
#include <numbers>

#ifdef __EMSCRIPTEN__
#include "WebBridge.h"
#endif

namespace {
	std::string scoreLabel(int _score) {
#ifdef __EMSCRIPTEN__
		bool english = WebBridge::isEnglish();
#else
		bool english = false;
#endif
		return (english ? "Score: " : "Score : ") + std::to_string(_score);
	}
}

void ScoreUpdate::init(TextRenderer* _scoreText) {
	scoreText = _scoreText;
	scoreText->setText(scoreLabel(lastScore).c_str());
	panel.setFillColor(sf::Color(0, 0, 0, 170));
	fitPanelToText();
}

void ScoreUpdate::update(float _deltaTime) {
	SceneManager* sm = SceneManager::instance();
	int score = sm->getCurrentScene()->getScore();
	sf::Text& text = scoreText->getText();

	if (score != lastScore) {
		lastScore = score;
		scoreText->setText(scoreLabel(score).c_str());
		fitPanelToText();
		punching = true;
		punchClock.restart();
	}

	if (punching) {
		float t = punchClock.getElapsedTime().asSeconds();
		if (t >= punchDuration) {
			punching = false;
			text.scale = { 1.f, 1.f };
		}
		else {
			float progress = t / punchDuration;
			float factor = 1.f + (punchScale - 1.f) * std::sin(std::numbers::pi_v<float> * progress);
			text.scale = { factor, factor };
		}
	}
}

void ScoreUpdate::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	_target.draw(panel, _states);
}

void ScoreUpdate::fitPanelToText() {
	sf::Text& text = scoreText->getText();
	sf::Rect2f bounds = text.getLocalBounds();

	const float paddingX = 16.f;
	const float paddingY = 10.f;

	panel.setSize({ bounds.size.x + paddingX * 2.f, bounds.size.y + paddingY * 2.f });
	panel.position = text.position;
	panel.origin = { panel.getSize().x / 2.f, panel.getSize().y / 2.f };
}
