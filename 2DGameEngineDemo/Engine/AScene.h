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

	// Optional UI (HUD) reference frame override - see setUIFrame(). Unused
	// (falls back to camera.size/2, camera.size, the pre-existing behavior)
	// unless a scene calls it.
	bool hasCustomUIFrame = false;
	sf::Vec2f uiCenter;
	sf::Vec2f uiSize;

	Entity* cameraTarget = nullptr;
	// Whether AScene::update() repositions the camera to follow cameraTarget
	// each frame (setViewFromPlayer()'s horizontal-scroll formula). Scenes
	// with a fixed camera (e.g. ShmupScene) still set a cameraTarget - other
	// gameplay code looks entities up that way (see
	// ShmupEnemyBehavior::update()) - but must opt out of the follow itself.
	bool followCameraTarget = true;

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
	// Widens the gameplay camera's shown extent beyond what setCamera()
	// established, to letterbox a scene designed for a different aspect
	// ratio into the actual (usually wider desktop) window - the extra
	// margin this reveals has nothing drawn in it, so it just stays the
	// window's clear color (black), producing bars on whichever axis ends
	// up oversized. Center stays whatever setCamera() set (the true
	// content's own center), only size grows. Pair with setUIFrame() so the
	// HUD stays pinned to the true content instead of this wider camera.
	//
	// Deliberately NOT implemented via sf::View::viewport - confirmed (see
	// glViewport dumped from the browser's WebGL context) that VRSFML's
	// Emscripten/WebGL2 backend applies a stale target size the first time a
	// non-default viewport reaches RenderTarget::applyView(), permanently
	// mis-sizing it (glViewport() ends up stuck at [0,0,1,1] - the literal
	// *fraction* rounded as if it were already pixels). A viewport that
	// always stays the harmless default {0,0,1,1} sidesteps that failure
	// mode entirely - this call only ever touches camera.size, never
	// .viewport.
	void setCameraLetterboxSize(sf::Vec2f _widerSize);
	// Pins the UI (HUD) view to an explicit center/size instead of the
	// default camera.size/2 / camera.size - needed once a scene's gameplay
	// camera has been widened by setCameraLetterboxSize() beyond its own
	// true content area, so the HUD stays aligned with that content instead
	// of following the wider camera's own midpoint.
	void setUIFrame(sf::Vec2f _center, sf::Vec2f _size);
	void setViewFromPlayer(Entity* _player);

	// Clamp the follow camera's horizontal center so it never scrolls past
	// _minX or _maxX (in world px) - e.g. the start and end of a level.
	void setCameraBounds(float _minX, float _maxX);

	// Entity the camera should follow each frame. Set by whichever gameplay
	// component represents the player (see PlayerBehavior::init()) so the
	// engine-level scene never needs to know about game-specific classes.
	// _followWithCamera=false still records the target (for gameplay code
	// that looks entities up via getCameraTarget(), e.g.
	// ShmupEnemyBehavior::update()) without repositioning the camera to it -
	// for scenes with a fixed camera instead of a follow camera.
	void setCameraTarget(Entity* _entity, bool _followWithCamera = true);
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
