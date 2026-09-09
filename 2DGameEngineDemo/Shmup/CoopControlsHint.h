#pragma once
#include <array>
#include <optional>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include "AComponent.h"

class ShipBehavior;

// Onboarding hint for 2-player co-op: a small keyboard diagram (up/left/
// down/right keycaps, drawn by hand rather than as a texture asset) shown
// just above a ship until it first moves - see ShipBehavior::hasMovedOnce().
// Solo mode keeps its own text-sentence hint (ShmupControlHint); co-op skips
// that one instead of reusing it because naming two different key sets in
// one line doesn't read cleanly, and unlike solo mode there's no touch
// fallback to fall back on here, so this always shows regardless of device.
class CoopControlsHint : public AComponent
{
	ShipBehavior* ship = nullptr;
	bool hidden = false;

	struct Key
	{
		sf::RectangleShape cap{ {} };
		std::optional<sf::Text> label;
	};
	// Fixed order: up, left, down, right - laid out in update()/init() as a
	// plus shape (up centered above a left/down/right row), matching how the
	// keys sit on a real keyboard (WASD/ZQSD and the arrow cluster both use
	// this same physical layout).
	std::array<Key, 4> keys;

public:
	// _anchor: UI-space center of the key cluster (the up key sits above it,
	// the left/down/right row is centered on it). _labels are the 4 glyphs
	// in up/left/down/right order (plain ASCII - e.g. {"Z","Q","S","D"} or
	// {"^","<","v",">"} - not Unicode arrows, so they render on any font
	// without a missing-glyph risk). _tint colors the keycaps to match this
	// ship's own tint (see ShmupScene::init()'s createShip tint parameter).
	void init(ShipBehavior* _ship, sf::Font& _font, sf::Vec2f _anchor, const std::array<sf::Utf8String, 4>& _labels, sf::Color _tint);
	void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;
};
