#include "AScene.h"
#include <SFML/System/Time.hpp>
#include "Render.h"
#include "SceneManager.h"
#include "TransformComponent.h"
#include <algorithm>

Entity* AScene::createEntity() {
    Entity* newEntity = new Entity(entitiesId);
    entitiesId++;
    return newEntity;
}

void AScene::addEntity(Entity* _entity) {
    entitiesToAdd.push_back(_entity);
}

void AScene::removeEntity(Entity* _entity) {
    entitiesToDelete.push_back(_entity);
}

void AScene::commitPendingEntities() {
    for (Entity* entity : entitiesToAdd) {
        entities.push_back(entity);
    }
    entitiesToAdd.clear();

    for (Entity* entity : uiEntitiesToAdd) {
        uiEntities.push_back(entity);
    }
    uiEntitiesToAdd.clear();
}

void AScene::addUIEntity(Entity* _entity) {
    uiEntitiesToAdd.push_back(_entity);
}

void AScene::end() {
    for (Entity* entity : entities) {
        delete entity;
    }
    for (Entity* entity : uiEntities) {
        delete entity;
    }

    entities.clear();
    entitiesToAdd.clear();
    entitiesToDelete.clear();
    uiEntities.clear();
    uiEntitiesToAdd.clear();
    cameraTarget = nullptr;
    hasCameraBounds = false;
    score = 0;
}

std::string AScene::getName() {
    return name;
}

void AScene::setName(std::string _name) {
    name = _name;
}

std::vector<Entity*>* AScene::getEntities() {
    return &entities;
}

int AScene::getScore() {
    return score;
}

void AScene::setScore() {
    score++;
}

void AScene::addScore(int _amount) {
    score += _amount;
}

float AScene::getTimer() {
    return timer.getElapsedTime().asSeconds();
}

void AScene::startTimer() {
    timer.start();
}

sf::View* AScene::getCamera() {
    return &camera;
}

void AScene::setCamera(sf::Vec2f _center, sf::Vec2f _size) {
    camera.center = _center;
    camera.size = _size;
}


void AScene::setCameraTarget(Entity* _entity) {
    cameraTarget = _entity;
}

Entity* AScene::getCameraTarget() {
    return cameraTarget;
}

void AScene::setCameraBounds(float _minX, float _maxX) {
    hasCameraBounds = true;
    cameraMinX = _minX;
    cameraMaxX = _maxX;
}

void AScene::setViewFromPlayer(Entity* _player) {
    TransformComponent* playerTransform = _player->getComponent<TransformComponent>();

    float centerX = playerTransform->getPosition().x + 500.f;

    if (hasCameraBounds) {
        float halfWidth = camera.size.x / 2.f;
        float minCenterX = cameraMinX + halfWidth;
        float maxCenterX = cameraMaxX - halfWidth;

        if (minCenterX <= maxCenterX) {
            centerX = std::clamp(centerX, minCenterX, maxCenterX);
        }
    }

    // Height is intentionally NOT tracked: the camera stays level regardless
    // of the player jumping or falling, so a fall into a pit just drops the
    // player out of frame instead of scrolling the camera down into the void
    // beneath the level's platforms.
    setCamera({ centerX, camera.center.y });
}

void AScene::draw(sf::RenderTarget& _target, sf::RenderStates _states) const
{
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        _target.draw(*(*it), _states);
    }

    if (!uiEntities.empty()) {
        sf::RenderStates uiStates = _states;
        uiStates.view = _target.computeView();

        for (auto it = uiEntities.begin(); it != uiEntities.end(); ++it) {
            _target.draw(*(*it), uiStates);
        }
    }
}

void AScene::update(float _deltaTime) {

    for (int i = 0; i < entities.size(); i++) {
        entities[i]->update(_deltaTime);
    }

    for (Entity* entity : uiEntities) {
        entity->update(_deltaTime);
    }

    for (Entity* entity : entitiesToAdd) {
        entities.push_back(entity);
    }

    for (Entity* entity : uiEntitiesToAdd) {
        uiEntities.push_back(entity);
    }
    uiEntitiesToAdd.clear();

    for (Entity* entity : entitiesToDelete) {
        if (entity == cameraTarget) {
            cameraTarget = nullptr;
        }
        auto it = find(entities.begin(), entities.end(), entity);
        if (it != entities.end()) {
            delete *it;
            entities.erase(it);
        }
    }

    entitiesToAdd.clear();
    entitiesToDelete.clear();

    if (cameraTarget) {
        setViewFromPlayer(cameraTarget);
    }
}
