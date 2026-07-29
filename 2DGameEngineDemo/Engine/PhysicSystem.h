#pragma once
#include "box2d/box2d.h"

class PhysicSystem
{
protected:
	b2Vec2 gravity;
	b2WorldId world;

	// Fixed-step accumulator: keeps Box2D stepping at a constant 1/60s
	// regardless of the actual frame rate, so physics stays stable when the
	// browser throttles frames (background tab, slow device, ...).
	float accumulator = 0.f;
	static constexpr float fixedTimestep = 1.f / 60.f;
	static constexpr float maxFrameTime = 0.25f;

public:
	PhysicSystem();
	b2WorldId* getWorld();
	void destroyBody(b2BodyId _body);
	void reset();
	void update(float _deltaTime);
	float static const worldScale;

private:
	// Box2D only exposes contact begin/end events for the most recent
	// b2World_Step call (they are not accumulated across steps), so this
	// must run after every single fixed sub-step - not once after the whole
	// accumulator loop - or a begin/end pair from an earlier sub-step within
	// the same frame gets silently dropped (e.g. landing then leaving the
	// ground within the same update(), which read as "still grounded" and
	// let the player re-jump mid-air).
	void processContactEvents();
};

