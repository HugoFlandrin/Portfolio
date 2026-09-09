#include "CoopControlsHint.h"
#include "ShipBehavior.h"
#include "Entity.h"
#include "AliveComponent.h"

namespace {
	constexpr float keySize = 56.f;
	constexpr float gap = 8.f;
	constexpr float step = keySize + gap;
	constexpr unsigned int labelCharacterSize = 30;

	// up, left, down, right - a plus/T shape, matching the physical layout
	// of both WASD/ZQSD and the arrow cluster on a real keyboard.
	constexpr std::array<sf::Vec2f, 4> keyOffsets{
		sf::Vec2f{ 0.f, -step },
		sf::Vec2f{ -step, 0.f },
		sf::Vec2f{ 0.f, 0.f },
		sf::Vec2f{ step, 0.f },
	};
}

void CoopControlsHint::init(ShipBehavior* _ship, sf::Font& _font, sf::Vec2f _anchor, const std::array<sf::Utf8String, 4>& _labels, sf::Color _tint) {
	ship = _ship;

	for (std::size_t i = 0; i < keys.size(); i++) {
		Key& key = keys[i];
		sf::Vec2f center = _anchor + keyOffsets[i];

		key.cap.setSize({ keySize, keySize });
		key.cap.origin = { keySize / 2.f, keySize / 2.f };
		key.cap.position = center;
		key.cap.setFillColor(sf::Color(10, 12, 20, 200));
		key.cap.setOutlineColor(_tint);
		key.cap.setOutlineThickness(3.f);

		key.label.emplace(_font, sf::Text::Data{
			.position = center,
			.string = _labels[i],
			.characterSize = labelCharacterSize,
			.fillColor = sf::Color::White,
		});
		key.label->origin = key.label->getLocalBounds().getCenter();
	}
}

void CoopControlsHint::update(float _deltaTime) {
	if (hidden || !ship) {
		return;
	}

	if (ship->hasMovedOnce()) {
		hidden = true;
		return;
	}

	// A ship can die without ever moving (e.g. a Shooter enemy's bullet
	// finds it at spawn) - without this, the hint would sit forever over an
	// inert, dead ship instead of clearing along with it.
	if (AliveComponent* alive = ship->getParent()->getComponent<AliveComponent>()) {
		if (alive->isDying()) {
			hidden = true;
		}
	}
}

void CoopControlsHint::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	if (hidden) {
		return;
	}

	for (const Key& key : keys) {
		_target.draw(key.cap, _states);
		if (key.label) {
			_target.draw(*key.label, _states);
		}
	}
}
