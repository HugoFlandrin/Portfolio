#include "ButtonComponent.h"
#include "TextRenderer.h"
#include "SceneManager.h"
#include "Engine.h"
#include <SFML/Window/Mouse.hpp>

void ButtonComponent::init(std::string _sceneName) {
	sceneToOpen = _sceneName;
}

void ButtonComponent::update(float _deltaTime) {
	Engine* e = Engine::instance();
	sf::Text& text = getParent()->getComponent<TextRenderer>()->getText();
	sf::RenderWindow* window = e->getWindow();
	AScene* scene = SceneManager::instance()->getCurrentScene();
	sf::Vec2f mousePosition = scene->getCamera()->screenToWorld(sf::Mouse::getPosition(*window).toVec2f(), window->getSize().toVec2f());

	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && text.getGlobalBounds().contains(mousePosition)) {
		action();
	}
}

void ButtonComponent::action() {
	SceneManager* sm = SceneManager::instance();
	sm->requestChangeScene(sceneToOpen);
}