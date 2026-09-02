#pragma once
#include <SFML/Graphics/RectangleShape.hpp>
#include "AComponent.h"

class PlayerBehavior;
class TextRenderer;

// Web-only onboarding hints for the first level: "press D to move" shown at
// spawn until the player first moves, then "press SPACE to jump" shown right
// after (until they first jump). Draws an opaque backing panel behind each
// hint's text so it stays legible over any background (the plain white fill
// color used to blend into the sky). Only instantiated by FirstMarioScene
// under __EMSCRIPTEN__ - desktop is unaffected. Must be added to the scene
// as a UI entity (addUIEntity), and before the hint text entities, so its
// panels are drawn in screen space and behind the text.
class TutorialHints : public AComponent
{
	PlayerBehavior* player = nullptr;
	TextRenderer* moveHintText = nullptr;
	TextRenderer* jumpHintText = nullptr;

	sf::RectangleShape moveHintPanel{ {} };
	sf::RectangleShape jumpHintPanel{ {} };

	bool moveHintHidden = false;
	bool jumpHintShown = false;
	bool jumpHintHidden = false;

public:
	void init(PlayerBehavior* _player, TextRenderer* _moveHintText, TextRenderer* _jumpHintText);
	void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;

private:
	// Resizes/positions a hint's backing panel to fit its (already set) text.
	static void fitPanelToText(sf::RectangleShape& _panel, TextRenderer& _hintText);
};
