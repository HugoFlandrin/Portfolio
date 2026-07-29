#pragma once
#include <string>
#include <vector>
#include "Entity.h"
#include <SFML/Graphics/View.hpp>
#include <SFML/System/Clock.hpp>

class AScene
{
	std::string name;

	std::vector<Entity*> entities;
	std::vector<Entity*> entitiesToAdd;
	std::vector<Entity*> entitiesToDelete;

	// Entities drawn/updated with a fixed screen-space view instead of the
	// scrolling camera (HUD elements: score, health bar, ...).
	std::vector<Entity*> uiEntities;
	std::vector<Entity*> uiEntitiesToAdd;

	int entitiesId = 0;

	int score = 0;
	sf::Clock timer;
	sf::View camera;

	Entity* cameraTarget = nullptr;

	// Optional horizontal clamp for the follow camera, so it stops at the
	// edges of the level instead of showing whatever is beyond them (void,
	// or nothing at all). Unset by default - existing scenes are unaffected.
	bool hasCameraBounds = false;
	float cameraMinX = 0.f;
	float cameraMaxX = 0.f;

public:
	virtual void init() = 0;
	virtual void end();

	std::string getName();
	void setName(std::string _name);

	std::vector<Entity*>* getEntities();

	int getScore();
	void setScore();
	void addScore(int _amount);
	float getTimer();
	void startTimer();
	sf::View* getCamera();
	void setCamera(sf::Vec2f _center = {960, 540}, sf::Vec2f _size = {1920, 1080});
	void setViewFromPlayer(Entity* _player);

	// Clamp the follow camera's horizontal center so it never scrolls past
	// _minX or _maxX (in world px) - e.g. the start and end of a level.
	void setCameraBounds(float _minX, float _maxX);

	// Entity the camera should follow each frame. Set by whichever gameplay
	// component represents the player (see PlayerBehavior::init()) so the
	// engine-level scene never needs to know about game-specific classes.
	void setCameraTarget(Entity* _entity);
	Entity* getCameraTarget();

	Entity* createEntity();

	void addEntity(Entity* _entity);
	void removeEntity(Entity* _entity);
	void commitPendingEntities();

	// UI entities are drawn last, with a fixed screen-space view, so they
	// stay put on screen regardless of where the gameplay camera is looking.
	void addUIEntity(Entity* _entity);

	virtual void update(float _deltaTime);

	virtual void draw(sf::RenderTarget& _target, sf::RenderStates _states) const;
};
