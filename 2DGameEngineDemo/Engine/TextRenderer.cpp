#include "TextRenderer.h"
#include "TransformComponent.h"
#include "Entity.h"

TextRenderer::TextRenderer(sf::Vec2f _position, sf::Font& _font, sf::Utf8String _text, int _characterSize, sf::Color _color) :
	text(_font, { .position = _position, .string = _text, .characterSize = static_cast<unsigned int>(_characterSize), .fillColor = _color }) {
	text.origin = text.getLocalBounds().getCenter();
}

sf::Text& TextRenderer::getText() {
	return text;
}

void TextRenderer::setText(sf::Utf8String _text) {
	text.setString(_text);
	// Re-center on the (possibly now very different) bounds - without this,
	// text set after construction keeps the origin computed for whatever
	// string it was created with, drifting off its intended position.
	text.origin = text.getLocalBounds().getCenter();
}

void TextRenderer::update(float _deltaTime) {

}

void TextRenderer::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	_target.draw(text, _states);
}
