#include "ShmupControlHint.h"
#include "ShipBehavior.h"
#include "TextRenderer.h"

void ShmupControlHint::init(ShipBehavior* _ship, TextRenderer* _hintText) {
	ship = _ship;
	hintText = _hintText;

	panel.setFillColor(sf::Color(0, 0, 0, 170));
	fitPanelToText();
}

void ShmupControlHint::update(float _deltaTime) {
	if (!hidden && ship && ship->hasMovedOnce()) {
		hintText->setText("");
		hidden = true;
	}
}

void ShmupControlHint::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	if (!hidden) {
		_target.draw(panel, _states);
	}
}

void ShmupControlHint::fitPanelToText() {
	sf::Text& text = hintText->getText();
	sf::Rect2f bounds = text.getLocalBounds();

	const float paddingX = 16.f;
	const float paddingY = 10.f;

	panel.setSize({ bounds.size.x + paddingX * 2.f, bounds.size.y + paddingY * 2.f });
	panel.position = text.position;
	panel.origin = { panel.getSize().x / 2.f, panel.getSize().y / 2.f };
}
