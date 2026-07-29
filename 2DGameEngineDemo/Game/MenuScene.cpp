#include "MenuScene.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "TransformComponent.h"
#include "ButtonComponent.h"

MenuScene::MenuScene() {}

void MenuScene::init() {
    ResourceManager* rm = ResourceManager::instance();
    uiFont = rm->loadFont("Kenney Pixel.ttf");

    setCamera();

    Entity* titleText = createEntity();
    TextRenderer* titleTextRender = new TextRenderer({ 960.f, 250.f }, *uiFont, "MARIO", 128, sf::Color::Red);
    titleText->addComponent(titleTextRender);
    addEntity(titleText);

    Entity* playButton = createEntity();
    playButton->createComponent<ButtonComponent>()->init("FirstLevel");
    TextRenderer* playButtonRender = new TextRenderer({ 960.f, 550.f }, *uiFont, "Jouer");
    playButton->addComponent(playButtonRender);
    addEntity(playButton);

    Entity* settingsButton = createEntity();
    settingsButton->createComponent<ButtonComponent>()->init("Settings");
    TextRenderer* settingsButtonRendere = new TextRenderer({ 960.f, 650.f }, *uiFont, "Options");
    settingsButton->addComponent(settingsButtonRendere);
    addEntity(settingsButton);
}