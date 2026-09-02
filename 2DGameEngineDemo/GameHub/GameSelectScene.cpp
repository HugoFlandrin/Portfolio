#include "GameSelectScene.h"
#include "ResourceManager.h"
#include "TextRenderer.h"
#include "TransformComponent.h"
#include "ButtonComponent.h"

GameSelectScene::GameSelectScene() {}

void GameSelectScene::init() {
    ResourceManager* rm = ResourceManager::instance();
    sf::Font* uiFont = rm->loadFont("Kenney Pixel.ttf");

    setCamera();

    Entity* titleText = createEntity();
    TextRenderer* titleTextRender = new TextRenderer({ 960.f, 250.f }, *uiFont, "Choisis ton jeu", 96, sf::Color::White);
    titleText->addComponent(titleTextRender);
    addEntity(titleText);

    Entity* platformerButton = createEntity();
    platformerButton->createComponent<ButtonComponent>()->init("FirstLevel");
    TextRenderer* platformerButtonRender = new TextRenderer({ 960.f, 500.f }, *uiFont, "Platformer");
    platformerButton->addComponent(platformerButtonRender);
    addEntity(platformerButton);

    Entity* shmupButton = createEntity();
    shmupButton->createComponent<ButtonComponent>()->init("ShmupLevel");
    TextRenderer* shmupButtonRender = new TextRenderer({ 960.f, 600.f }, *uiFont, "Shmup");
    shmupButton->addComponent(shmupButtonRender);
    addEntity(shmupButton);
}
