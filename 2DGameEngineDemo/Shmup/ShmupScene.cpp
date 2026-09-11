#include "ShmupScene.h"
#include "Entity.h"
#include "TransformComponent.h"
#include "Render.h"
#include "ResourceManager.h"
#include "ShipBehavior.h"
#include "AliveComponent.h"
#include "EnemySpawner.h"
#include "PickupSpawner.h"
#include "ScrollingBackground.h"
#include "ScoreUpdate.h"
#include "PlayerScoreUI.h"
#include "TextRenderer.h"
#include "HealthBarUI.h"
#include "CountdownUI.h"
#include "ShmupControlHint.h"
#include "CoopControlsHint.h"
#include "CoopGameOverWatcher.h"
#include "AudioManager.h"
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

void ShmupScene::setInfiniteMode(bool _infinite) {
	infiniteMode = _infinite;
}

void ShmupScene::setTwoPlayer(bool _twoPlayer) {
	twoPlayer = _twoPlayer;
}

void ShmupScene::init() {
	// Starts on the very first run and just keeps looping - a no-op on every
	// later call (replays, switching modes) so the ambiance never restarts
	// or stacks a second copy on top of itself (see playMusicLooping()'s own
	// comment).
	AudioManager::instance()->playMusicLooping("Clement Panchout - Sweet 70s.mp3");

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

	// A previous run might have ended in a win (or been 2-player); make sure
	// a fresh run doesn't inherit either flag and show the wrong end-of-game
	// message/score breakdown.
	SceneManager::instance()->setLastRunWon(false);
	SceneManager::instance()->setLastRunTwoPlayer(twoPlayer);
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

	//Player ship(s). In 2-player co-op, player 0 switches from touch/mouse +
	// arrow keys to WASD/ZQSD only, and player 1 (arrow keys only) joins
	// alongside it - see ShipInputScheme's own comment for why touch/mouse
	// must stay off for both. Both get an empty deathScene there too, since
	// one ship dying must not end the run by itself (see CoopGameOverWatcher
	// below) - solo mode is unaffected, still "GameOver" as before.
	auto createShip = [&](float _x, int _playerId, ShipInputScheme _inputScheme, sf::Color _tint) {
		Entity* shipEntity = createEntity();
		shipEntity->createComponent<TransformComponent>()->init({ _x, playAreaHeight - 180.f }, { shipScale, shipScale });
		shipEntity->createComponent<ShipBehavior>()->init(playAreaWidth, playAreaHeight, _playerId, _inputScheme);
		// canRegen=false: a regenerating health bar would make it effectively
		// impossible to actually lose to enemy fire (chip damage would just
		// heal back between hits) - only physically colliding with an enemy
		// (an instant kill, see ShipBehavior::beginCollision()) should end
		// the run.
		shipEntity->createComponent<AliveComponent>()->init(100.f, twoPlayer ? "" : "GameOver", 0.9f, false);
		// Dynamic, not kinematic: see BulletBehavior::spawn() - Box2D never
		// generates contact events between two non-dynamic bodies, so the
		// ship would never detect touching an (also-dynamic) enemy
		// otherwise. Physics size slightly smaller than the sprite crop
		// (shipRectW/H, was even bigger than it) so a hit reads as landing
		// on the visible hull instead of registering just outside it.
		// groupIndex -1 (shared by every ship - see ACollider::init()):
		// ships must never physically collide with each other - not two
		// living ships pushing one another off the play area, and not a
		// downed ship (still solid, kept in the scene forever - see
		// ShipBehavior::update()) blocking the other. Harmless in solo mode,
		// where there's only ever one ship in that group.
		shipEntity->createPhysics({ 28.f * shipScale, 20.f * shipScale }, b2_dynamicBody, true, 1.f, 0.f, false, -1);
		shipEntity->getComponent<RigidBody>()->setGravityScale(0.f);

		Render* shipRender = new Render(*rm->loadTexture("ships.png"), sf::Rect2i({ shipRectX, shipRectY }, { shipRectW, shipRectH }), { shipRectW / 2.f, shipRectH / 2.f });
		// Tinted per-player so the two ships read as visually distinct at a
		// glance despite sharing the same sprite - white leaves player 0's
		// natural sprite color untouched.
		shipRender->getSprite().color = _tint;
		shipEntity->addComponent(shipRender);
		addEntity(shipEntity);
		return shipEntity;
	};

	ShipInputScheme player0Input;
	if (twoPlayer) {
		player0Input.up = sf::Keyboard::Scan::W;
		player0Input.down = sf::Keyboard::Scan::S;
		player0Input.left = sf::Keyboard::Scan::A;
		player0Input.right = sf::Keyboard::Scan::D;
		player0Input.enableTouchAndMouse = false;
	}
	Entity* ship = createShip(twoPlayer ? playAreaWidth * 0.3f : playAreaWidth / 2.f, 0, player0Input, sf::Color::White);

	Entity* ship2 = nullptr;
	if (twoPlayer) {
		ShipInputScheme player1Input; // defaults (arrow keys) are already right
		player1Input.enableTouchAndMouse = false;
		ship2 = createShip(playAreaWidth * 0.7f, 1, player1Input, sf::Color(255, 150, 150));
	}

	// Plays the GameOver sound the instant the run actually ends, in both
	// modes - in solo (ship2=nullptr), the scene change itself is still
	// driven by that ship's own AliveComponent deathScene set in createShip()
	// above, so triggerSceneChange is false there; in 2-player co-op this
	// still also owns the actual scene change, unchanged from before (see
	// CoopGameOverWatcher's own comment).
	Entity* watcherEntity = createEntity();
	watcherEntity->createComponent<CoopGameOverWatcher>()->init(ship, ship2, twoPlayer);
	addEntity(watcherEntity);

	//Wave director - no visuals of its own.
	Entity* spawner = createEntity();
	spawner->createComponent<EnemySpawner>()->init(playAreaWidth, playAreaHeight, infiniteMode);
	addEntity(spawner);

	//Coin pickups - active in every mode, entirely independent of the enemy
	// wave director above (see PickupSpawner's own comment).
	Entity* pickupSpawner = createEntity();
	pickupSpawner->createComponent<PickupSpawner>()->init(playAreaWidth, playAreaHeight);
	addEntity(pickupSpawner);

	//HUD - fixed to the screen regardless of where the gameplay camera looks.
	sf::Font* uiFont = rm->loadFont("Kenney Pixel.ttf");

	// ScoreUpdate is created before scoreRender is added so its backing
	// panel (drawn in ScoreUpdate::draw) renders behind the score text
	// instead of over it - same ordering trick used by the platformer. Used
	// for the shared total in both modes, same top-center spot - just a
	// bigger font in 2-player co-op, where it now shares the HUD with the
	// two per-player scores below instead of being the only number on
	// screen.
	Entity* scoreEntity = createEntity();
	// y=65, not the top edge (was 40): leaves clearance under the settings
	// gear/close buttons in the hosting page's overlay chrome, which sit
	// right above the canvas - see game-embed.css's .game-overlay-close/
	// .game-overlay-settings.
	TextRenderer* scoreRender = new TextRenderer({ playAreaWidth / 2.f, 65.f }, *uiFont, "Score : 0", twoPlayer ? 40 : 32);
	scoreEntity->createComponent<ScoreUpdate>()->init(scoreRender);
	scoreEntity->addComponent(scoreRender);
	addUIEntity(scoreEntity);

	// One health bar per ship, each reading straight from its own ship
	// entity (see HealthBarUI's Entity*-taking init() overload) instead of
	// the scene's single camera-target slot, which can only ever point at
	// one of the two ships in 2-player co-op. Each player's individual
	// score (PlayerScoreUI) sits right below their own bar, centered on it.
	if (twoPlayer) {
		sf::Vec2f healthBarSize{ 200.f, 24.f };
		// y=95, not 70 - see the score TextRenderer's own comment above.
		sf::Vec2f healthBar1Pos{ 20.f, 95.f };
		sf::Vec2f healthBar2Pos{ playAreaWidth - 220.f, 95.f };

		Entity* healthBar1 = createEntity();
		healthBar1->createComponent<HealthBarUI>()->init(ship, healthBar1Pos, healthBarSize);
		addUIEntity(healthBar1);

		Entity* healthBar2 = createEntity();
		healthBar2->createComponent<HealthBarUI>()->init(ship2, healthBar2Pos, healthBarSize);
		addUIEntity(healthBar2);

		const float scoreY = healthBar1Pos.y + healthBarSize.y + 26.f;
		const int playerScoreFontSize = 34;

		Entity* p1ScoreEntity = createEntity();
		TextRenderer* p1ScoreRender = new TextRenderer({ healthBar1Pos.x + healthBarSize.x / 2.f, scoreY }, *uiFont, "", playerScoreFontSize);
		p1ScoreEntity->createComponent<PlayerScoreUI>()->init(p1ScoreRender, 0);
		p1ScoreEntity->addComponent(p1ScoreRender);
		addUIEntity(p1ScoreEntity);

		Entity* p2ScoreEntity = createEntity();
		TextRenderer* p2ScoreRender = new TextRenderer({ healthBar2Pos.x + healthBarSize.x / 2.f, scoreY }, *uiFont, "", playerScoreFontSize);
		p2ScoreEntity->createComponent<PlayerScoreUI>()->init(p2ScoreRender, 1);
		p2ScoreEntity->addComponent(p2ScoreRender);
		addUIEntity(p2ScoreEntity);
	}
	else {
		Entity* healthBarEntity = createEntity();
		// y=95, not 70 - see the score TextRenderer's own comment above.
		healthBarEntity->createComponent<HealthBarUI>()->init({ 20.f, 95.f }, { 200.f, 24.f });
		addUIEntity(healthBarEntity);
	}

	// Infinite mode has no time limit to count down to, so there's nothing
	// meaningful for CountdownUI to show - just skip it there instead of
	// displaying a countdown that would never end.
	if (!infiniteMode) {
		// Same backing-panel-behind-text ordering trick as ScoreUpdate above.
		Entity* timerEntity = createEntity();
		// y=95, not 65 like the score text: at the right edge, y=65 still sits
		// inside the hosting page's close button (top-right, see
		// game-embed.css's .game-overlay-close) - the score avoids it purely by
		// being centered, far from either corner button, but the timer sits
		// right under it there. y=95 matches the solo health bar's row below,
		// which is already clear of it.
		TextRenderer* timerRender = new TextRenderer({ playAreaWidth - 110.f, 95.f }, *uiFont, "", 28);
		timerEntity->createComponent<CountdownUI>()->init(timerRender, ShmupConstants::gameDuration);
		timerEntity->addComponent(timerRender);
		addUIEntity(timerEntity);
	}

	// Desktop-only onboarding hint (see setShowControlHint()/ShmupScene.h) -
	// touch players already have an obvious, self-explanatory control
	// scheme (drag the ship around), so this would be redundant there. Not
	// used in 2-player co-op - see the CoopControlsHint block below instead.
	if (showControlHint && !twoPlayer) {
#ifdef __EMSCRIPTEN__
		bool english = WebBridge::isEnglish();
#else
		bool english = false;
#endif
		Entity* hintEntity = createEntity();
		// Mentions both control schemes now that solo also accepts ZQSD
		// alongside the arrow keys (see ShipInputScheme) - two lines and a
		// bigger size than before (28 -> 34) so it stays readable with the
		// extra text; the panel behind it (see ShmupControlHint::fitPanelToText())
		// grows to fit either language/size automatically.
		TextRenderer* hintRender = new TextRenderer(
			{ playAreaWidth / 2.f, playAreaHeight / 2.f },
			*uiFont,
			english ? "Move with the arrow keys\nor with WASD" : "Déplace-toi avec les flèches directionnelles\nou avec ZQSD",
			34
		);
		hintEntity->createComponent<ShmupControlHint>()->init(ship->getComponent<ShipBehavior>(), hintRender);
		hintEntity->addComponent(hintRender);
		addUIEntity(hintEntity);
	}

	// Co-op onboarding hint: a small keyboard diagram above each ship
	// instead of a text sentence (see CoopControlsHint's own comment for
	// why) - always shown regardless of device, since 2-player mode has no
	// touch fallback to fall back on the way solo mode does.
	if (twoPlayer) {
		constexpr float hintOffsetY = 130.f;
		const sf::Color player0Tint = sf::Color::White;
		const sf::Color player1Tint = sf::Color(255, 150, 150);

		Entity* hint1Entity = createEntity();
		hint1Entity->createComponent<CoopControlsHint>()->init(
			ship->getComponent<ShipBehavior>(), *uiFont,
			{ playAreaWidth * 0.3f, playAreaHeight - 180.f - hintOffsetY },
			{ "Z", "Q", "S", "D" }, player0Tint
		);
		addUIEntity(hint1Entity);

		Entity* hint2Entity = createEntity();
		hint2Entity->createComponent<CoopControlsHint>()->init(
			ship2->getComponent<ShipBehavior>(), *uiFont,
			{ playAreaWidth * 0.7f, playAreaHeight - 180.f - hintOffsetY },
			{ "^", "<", "v", ">" }, player1Tint
		);
		addUIEntity(hint2Entity);
	}
}
