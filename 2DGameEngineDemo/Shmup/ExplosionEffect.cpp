#include "ExplosionEffect.h"
#include "Entity.h"
#include "AScene.h"
#include "SceneManager.h"
#include "Render.h"
#include "ResourceManager.h"
#include <cmath>

namespace {
	struct ExplosionPreset {
		int rectX;
		int rectY;
		float duration;
		float peakScale;
	};

	// Two visually distinct sprites from shmupTiles.png's burst cluster, not
	// just the same one at different scales - a bullet grazing something is
	// a different kind of event than a ship actually blowing up, so they
	// read differently: a small white flash vs. a big orange fireball.
	constexpr ExplosionPreset presetFor(ExplosionType _type) {
		switch (_type) {
			case ExplosionType::Impact:
				return { 128, 0, 0.2f, 1.6f };
			case ExplosionType::Destruction:
			default:
				return { 72, 0, 0.35f, 3.5f };
		}
	}

	constexpr int explosionRectSize = 16;
}

void ExplosionEffect::spawn(AScene* _scene, sf::Vec2f _position, ExplosionType _type) {
	ExplosionPreset preset = presetFor(_type);

	Entity* explosion = _scene->createEntity();

	explosion->createComponent<TransformComponent>()->init(_position, { 0.f, 0.f });
	explosion->createComponent<ExplosionEffect>()->init(preset.duration, preset.peakScale);

	Render* explosionRender = new Render(
		*ResourceManager::instance()->loadTexture("shmupTiles.png"),
		sf::Rect2i({ preset.rectX, preset.rectY }, { explosionRectSize, explosionRectSize }),
		{ explosionRectSize / 2.f, explosionRectSize / 2.f }
	);
	explosion->addComponent(explosionRender);

	_scene->addEntity(explosion);
}

void ExplosionEffect::init(float _duration, float _peakScale) {
	transformComp = getParent()->getComponent<TransformComponent>();
	duration = _duration;
	peakScale = _peakScale;
}

void ExplosionEffect::update(float _deltaTime) {
	elapsed += _deltaTime;
	if (elapsed >= duration) {
		SceneManager::instance()->getCurrentScene()->removeEntity(getParent());
		return;
	}

	// 0 -> peakScale at the midpoint -> 0 again: a single sine half-wave
	// over the effect's lifetime, giving the expand-then-fade "pop" without
	// any actual frame animation.
	float t = elapsed / duration;
	float scale = peakScale * std::sin(t * 3.14159265f);
	transformComp->setScale({ scale, scale });
}
