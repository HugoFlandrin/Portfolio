#pragma once
#include <SFML/Graphics/Text.hpp>
#include "AComponent.h"

class TextRenderer : public AComponent
{
	sf::Text text;

public:
	TextRenderer(sf::Vec2f _position, sf::Font& _font, sf::Utf8String _text, int _characterSize = 64, sf::Color _color = sf::Color::White);

	sf::Text& getText();
	void setText(sf::Utf8String _text);
	virtual void update(float _deltaTime) override;
	void draw(sf::RenderTarget& _target, sf::RenderStates _states) const override;
};
