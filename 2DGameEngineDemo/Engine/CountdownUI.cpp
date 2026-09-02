#include "CountdownUI.h"
#include "SceneManager.h"
#include "TextRenderer.h"
#include <algorithm>
#include <cmath>

namespace {
	constexpr float lowTimeThreshold = 10.f;
}

void CountdownUI::init(TextRenderer* _timerText, float _duration) {
	timerText = _timerText;
	duration = _duration;
	panel.setFillColor(sf::Color(0, 0, 0, 170));
	// Populates the initial text and sizes the panel to it in one go (see
	// update()'s "changed" branch) - no separate fitPanelToText() call
	// needed here.
	update(0.f);
}

void CountdownUI::update(float _deltaTime) {
	float elapsed = SceneManager::instance()->getCurrentScene()->getTimer();
	float remaining = std::max(0.f, duration - elapsed);
	int secondsShown = static_cast<int>(std::ceil(remaining));

	if (secondsShown != lastSecondsShown) {
		lastSecondsShown = secondsShown;
		timerText->setText(("Temps : " + std::to_string(secondsShown) + "s").c_str());
		fitPanelToText();
	}

	timerText->getText().setFillColor(remaining <= lowTimeThreshold ? sf::Color::Red : sf::Color::White);
}

void CountdownUI::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	_target.draw(panel, _states);
}

void CountdownUI::fitPanelToText() {
	sf::Text& text = timerText->getText();
	sf::Rect2f bounds = text.getLocalBounds();

	const float paddingX = 16.f;
	const float paddingY = 10.f;

	panel.setSize({ bounds.size.x + paddingX * 2.f, bounds.size.y + paddingY * 2.f });
	panel.position = text.position;
	panel.origin = { panel.getSize().x / 2.f, panel.getSize().y / 2.f };
}
