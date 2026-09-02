#include "ShmupScene.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "Render.h"
#include "ResourceManager.h"
#include "ShipBehavior.h"
#include "AliveComponent.h"
#include "EnemySpawner.h"
#include "ScrollingBackground.h"
#include "ScoreUpdate.h"
#include "TextRenderer.h"
#include "HealthBarUI.h"
#include "CountdownUI.h"
#include "ShmupControlHint.h"
#include "SceneManager.h"
#include "Engine.h"
#include "RigidBody.h"
#include "ShmupConstants.h"
#ifdef __EMSCRIPTEN__
#include "WebBridge.h"
#endif

namespace {
	// Flat, seamless-enough patch of grass picked out of shmupTiles.png's
	// terrain area (see ScrollingBackground) - a first-pass backdrop, meant
	// to be swapped for a purpose-made tile once the gameplay is validated.
	constexpr int bgRectX = 30;
	constexpr int bgRectY = 60;
	constexpr int bgRectSize = 20;

	// Player ship sprite: the bright blue hull, first variant in ships.png.
	constexpr int shipRectX = 0;
	constexpr int shipRectY = 4;
	constexpr int shipRectW = 32;
	constexpr int shipRectH = 24;

	// ships.png's sprites read as too small at actual play area scale -
	// drawn 2x native size, with the physics box scaled to match so the
	// hitbox agrees with what's on screen.
	constexpr float shipScale = 2.f;
}

ShmupScene::ShmupScene() {}

void ShmupScene::setShowControlHint(bool _show) {
	showControlHint = _show;
}

void ShmupScene::init() {
	// A fixed mobile-portrait play area (see ShmupConstants.h), independent
	// of the actual (usually much wider, desktop) window size - every
	// gameplay bound in this scene/its entities is derived from this instead
	// of the real window, so the game plays identically regardless of the
	// window's real dimensions.
	float playAreaWidth = ShmupConstants::playAreaWidth;
	float playAreaHeight = ShmupConstants::playAreaHeight;
	// Full play area height per tile (was 0.55x) - two of them stacked cover
	// twice the visible area from the very first frame instead of just
	// barely over it, so there's no way for the wrap logic below to ever
	// leave a sliver of black showing at the top while catching up.
	float bgTileHeight = playAreaHeight;

	setCamera({ playAreaWidth / 2.f, playAreaHeight / 2.f }, { playAreaWidth, playAreaHeight });
	// Pin the HUD to the true content area now, before the camera gets
	// widened below - see setUIFrame()'s own comment.
	setUIFrame({ playAreaWidth / 2.f, playAreaHeight / 2.f }, { playAreaWidth, playAreaHeight });

	// Letterbox the play area into whatever the actual window turns out to
	// be: widen the camera to exactly match the window's own aspect ratio
	// (so nothing stretches), centered on the true content - the extra
	// margin this reveals has nothing drawn in it, so it just stays the
	// window's clear color (black), producing bars on the window's wider
	// axis (typically left/right, a desktop window vs. this portrait
	// content). See setCameraLetterboxSize()'s own comment for why this
	// isn't done via sf::View::viewport instead.
	sf::Vec2f windowSize = Engine::instance()->getWindow()->getSize().toVec2f();
	float windowAspect = windowSize.x / windowSize.y;
	float playAreaAspect = playAreaWidth / playAreaHeight;
	sf::Vec2f letterboxedSize = windowAspect >= playAreaAspect
		? sf::Vec2f{ playAreaHeight * windowAspect, playAreaHeight }
		: sf::Vec2f{ playAreaWidth, playAreaWidth / windowAspect };
	setCameraLetterboxSize(letterboxedSize);

	// A previous run might have ended in a win; make sure a fresh death
	// doesn't inherit that flag and show the wrong end-of-game message.
	SceneManager::instance()->setLastRunWon(false);
	startTimer();

	ResourceManager* rm = ResourceManager::instance();
	sf::Texture* tilesTexture = rm->loadTexture("shmupTiles.png");

	// Scrolling backdrop: two stacked copies of the same patch, offset by
	// exactly one tile height, both scrolling down and looping (see
	// ScrollingBackground) - stretched to cover the play area, so it reads
	// as a soft ground texture rather than a crisp tile at this scale.
	for (int i = 0; i < 2; i++) {
		Entity* bgTile = createEntity();
		TransformComponent* bgTransform = bgTile->createComponent<TransformComponent>();
		// Stacked with zero gap: together the pair spans [0, 2*bgTileHeight],
		// comfortably covering the play area height with buffer to spare.
		bgTransform->init({ playAreaWidth / 2.f, bgTileHeight / 2.f + i * bgTileHeight });
		bgTransform->setScale({ playAreaWidth / bgRectSize, bgTileHeight / bgRectSize });
		bgTile->createComponent<ScrollingBackground>()->init(bgTileHeight, playAreaHeight);

		Render* bgRender = new Render(*tilesTexture, sf::Rect2i({ bgRectX, bgRectY }, { bgRectSize, bgRectSize }), { bgRectSize / 2.f, bgRectSize / 2.f });
		bgTile->addComponent(bgRender);
		addEntity(bgTile);
	}

	//Player ship
	Entity* ship = createEntity();
	ship->createComponent<TransformComponent>()->init({ playAreaWidth / 2.f, playAreaHeight - 180.f }, { shipScale, shipScale });
	ship->createComponent<ShipBehavior>()->init(playAreaWidth, playAreaHeight);
	// canRegen=false: a regenerating health bar would make it effectively
	// impossible to actually lose to enemy fire (chip damage would just heal
	// back between hits) - only physically colliding with an enemy (an
	// instant kill, see ShipBehavior::beginCollision()) should end the run.
	ship->createComponent<AliveComponent>()->init(100.f, "GameOver", 0.9f, false);
	// Dynamic, not kinematic: see BulletBehavior::spawn() - Box2D never
	// generates contact events between two non-dynamic bodies, so the ship
	// would never detect touching an (also-dynamic) enemy otherwise.
	// Physics size slightly smaller than the sprite crop (shipRectW/H, was
	// even bigger than it) so a hit reads as landing on the visible hull
	// instead of registering just outside it.
	ship->createPhysics({ 28.f * shipScale, 20.f * shipScale }, b2_dynamicBody, true, 1.f, 0.f);
	ship->getComponent<RigidBody>()->setGravityScale(0.f);

	Render* shipRender = new Render(*rm->loadTexture("ships.png"), sf::Rect2i({ shipRectX, shipRectY }, { shipRectW, shipRectH }), { shipRectW / 2.f, shipRectH / 2.f });
	ship->addComponent(shipRender);
	addEntity(ship);

	//Wave director - no visuals of its own.
	Entity* spawner = createEntity();
	spawner->createComponent<EnemySpawner>()->init(playAreaWidth, playAreaHeight);
	addEntity(spawner);

	//HUD - fixed to the screen regardless of where the gameplay camera looks.
	sf::Font* uiFont = rm->loadFont("Kenney Pixel.ttf");

	// ScoreUpdate is created before scoreRender is added so its backing
	// panel (drawn in ScoreUpdate::draw) renders behind the score text
	// instead of over it - same ordering trick used by the platformer.
	Entity* scoreEntity = createEntity();
	TextRenderer* scoreRender = new TextRenderer({ playAreaWidth / 2.f, 40.f }, *uiFont, "Score : 0", 32);
	scoreEntity->createComponent<ScoreUpdate>()->init(scoreRender);
	scoreEntity->addComponent(scoreRender);
	addUIEntity(scoreEntity);

	Entity* healthBarEntity = createEntity();
	healthBarEntity->createComponent<HealthBarUI>()->init({ 20.f, 70.f }, { 200.f, 24.f });
	addUIEntity(healthBarEntity);

	// Same backing-panel-behind-text ordering trick as ScoreUpdate above.
	Entity* timerEntity = createEntity();
	TextRenderer* timerRender = new TextRenderer({ playAreaWidth - 110.f, 40.f }, *uiFont, "", 28);
	timerEntity->createComponent<CountdownUI>()->init(timerRender, ShmupConstants::gameDuration);
	timerEntity->addComponent(timerRender);
	addUIEntity(timerEntity);

	// Desktop-only onboarding hint (see setShowControlHint()/ShmupScene.h) -
	// touch players already have an obvious, self-explanatory control
	// scheme (drag the ship around), so this would be redundant there.
	if (showControlHint) {
#ifdef __EMSCRIPTEN__
		bool english = WebBridge::isEnglish();
#else
		bool english = false;
#endif
		Entity* hintEntity = createEntity();
		TextRenderer* hintRender = new TextRenderer(
			{ playAreaWidth / 2.f, playAreaHeight / 2.f },
			*uiFont,
			english ? "Use the arrow keys to move" : "Utilise les flèches directionnelles",
			28
		);
		hintEntity->createComponent<ShmupControlHint>()->init(ship->getComponent<ShipBehavior>(), hintRender);
		hintEntity->addComponent(hintRender);
		addUIEntity(hintEntity);
	}
}
