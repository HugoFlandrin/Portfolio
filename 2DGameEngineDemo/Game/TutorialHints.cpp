#include "TutorialHints.h"
#include "PlayerBehavior.h"
#include "TextRenderer.h"
#include "WebBridge.h"

void TutorialHints::init(PlayerBehavior* _player, TextRenderer* _moveHintText, TextRenderer* _jumpHintText) {
	player = _player;
	moveHintText = _moveHintText;
	jumpHintText = _jumpHintText;

	moveHintPanel.setFillColor(sf::Color(0, 0, 0, 170));
	jumpHintPanel.setFillColor(sf::Color(0, 0, 0, 170));

	// moveHintText is visible from the start; jumpHintText starts empty
	// (hidden) and gets its panel sized once its text is actually set, in
	// update() below.
	fitPanelToText(moveHintPanel, *moveHintText);
}

void TutorialHints::update(float _deltaTime) {
	if (!player) {
		return;
	}

	if (!moveHintHidden && player->hasMovedOnce()) {
		moveHintText->setText("");
		moveHintHidden = true;
	}

	if (!jumpHintHidden) {
		if (player->hasJumpedOnce()) {
			jumpHintText->setText("");
			jumpHintHidden = true;
		}
		else if (!jumpHintShown && moveHintHidden) {
			jumpHintText->setText(WebBridge::isEnglish() ? "Press 'SPACE' to jump" : "Appuyez sur 'ESPACE' pour sauter");
			fitPanelToText(jumpHintPanel, *jumpHintText);
			jumpHintShown = true;
		}
	}
}

void TutorialHints::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	if (!moveHintHidden) {
		_target.draw(moveHintPanel, _states);
	}
	if (jumpHintShown && !jumpHintHidden) {
		_target.draw(jumpHintPanel, _states);
	}
}

void TutorialHints::fitPanelToText(sf::RectangleShape& _panel, TextRenderer& _hintText) {
	sf::Text& text = _hintText.getText();
	sf::Rect2f bounds = text.getLocalBounds();

	const float paddingX = 16.f;
	const float paddingY = 10.f;

	_panel.setSize({ bounds.size.x + paddingX * 2.f, bounds.size.y + paddingY * 2.f });
	_panel.position = text.position;
	_panel.origin = { _panel.getSize().x / 2.f, _panel.getSize().y / 2.f };
}
