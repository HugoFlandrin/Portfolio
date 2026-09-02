#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include "AComponent.h"

class ShipBehavior;
class TextRenderer;

// Desktop-only onboarding hint: "use the arrow keys to move", shown at spawn
// until the ship first moves (see ShipBehavior::hasMovedOnce() - by ANY
// input method, not just the keyboard the hint names, so it doesn't linger
// once the player has clearly found a way to move). Same backing-panel
// pattern as the Platformer's TutorialHints. Must be added to the scene as a
// UI entity (addUIEntity), and before the hint text entity, so its panel is
// drawn in screen space and behind the text.
class ShmupControlHint : public AComponent
{
	ShipBehavior* ship = nullptr;
	TextRenderer* hintText = nullptr;
	sf::RectangleShape panel{ {} };
	bool hidden = false;

public:
	void init(ShipBehavior* _ship, TextRenderer* _hintText);
	void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;

private:
	void fitPanelToText();
};
