#include "SettingsScene.h"
#include "TextRenderer.h"
#include "ResourceManager.h"
#include "ButtonComponent.h"

SettingsScene::SettingsScene() {

}
void SettingsScene::init() {
	ResourceManager* rm = ResourceManager::instance();
	uiFont = rm->loadFont("Kenney Pixel.ttf");

	setCamera();

	Entity* testText = createEntity();
	TextRenderer* testTextRender = new TextRenderer({ 960.f, 250.f }, *uiFont, "Ceci est la scène des options", 128, sf::Color::Red);
	testText->addComponent(testTextRender);
	addEntity(testText);

	Entity* menuButton = createEntity();
	menuButton->createComponent<ButtonComponent>()->init("Menu");
	TextRenderer* playButtonRender = new TextRenderer({ 960.f, 550.f }, *uiFont, "Retourner au menu");
	menuButton->addComponent(playButtonRender);
	addEntity(menuButton);
}