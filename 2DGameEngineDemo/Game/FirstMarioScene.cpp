#include "FirstMarioScene.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include "TransformComponent.h"
#include "Render.h"
#include "MapBehavior.h"
#include "ResourceManager.h"
#include "PlayerBehavior.h"
#include "MovableComponent.h"
#include "AliveComponent.h"
#include "EnemyBehavior.h"
#include "CoinBehavior.h"
#include "FlagBehavior.h"
#include "TextRenderer.h"
#include "ScoreUpdate.h"
#include "HealthBarUI.h"
#include "SceneManager.h"
#include <SFML/System/Utf8String.hpp>

#ifdef __EMSCRIPTEN__
#include "TutorialHints.h"
#include "WebBridge.h"
#endif

FirstMarioScene::FirstMarioScene() {}

// Tile ids used by the map grid below (see MapBehavior::newGenerateMap):
// 0 = empty, 1 = grass, 2 = dirt, 3 = solid obstacle (blocks jumps, no damage),
// 4 = spike pointing up, 5 = spike pointing down (mounted under a platform).
namespace {
	constexpr int TILE_EMPTY = 0;
	constexpr int TILE_GRASS = 1;
	constexpr int TILE_DIRT = 2;
	constexpr int TILE_OBSTACLE = 3;
	constexpr int TILE_SPIKE_UP = 4;
	constexpr int TILE_SPIKE_DOWN = 5;

	constexpr int mapWidth = 110;
	// Deep enough that solid ground never runs out of dirt tiles before the
	// bottom of the (now vertically-fixed) camera view, even though the
	// visible play area only shows a fraction of this depth.
	constexpr int mapHeight = 20;
	constexpr int groundRow = 8;
	constexpr float tileSize = 64.f;
}

void FirstMarioScene::init() {

    setCamera();

    // Keep the camera from ever showing past the first/last ground block -
    // the player can still walk into that margin, only the camera freezes.
    setCameraBounds(0.f, mapWidth * tileSize);

    // Invisible walls at those same bounds so the player can't wander past
    // where the camera stops either.
    const float wallThickness = 40.f;
    const float wallHeight = 4000.f;

    Entity* leftWall = createEntity();
    leftWall->createComponent<TransformComponent>()->init({ -wallThickness / 2.f, 0.f });
    leftWall->createPhysics({ wallThickness, wallHeight });
    addEntity(leftWall);

    Entity* rightWall = createEntity();
    rightWall->createComponent<TransformComponent>()->init({ mapWidth * tileSize + wallThickness / 2.f, 0.f });
    rightWall->createPhysics({ wallThickness, wallHeight });
    addEntity(rightWall);

    // A previous run might have ended in a win; make sure a fresh death
    // doesn't inherit that flag and show the wrong end-of-game message.
    SceneManager::instance()->setLastRunWon(false);

    //Resources
    ResourceManager* rm = ResourceManager::instance();

    Entity* map = createEntity();

    // Built section by section instead of a hand-typed grid literal, since a
    // 110x14 matrix is too easy to get wrong by hand. Ground is solid from
    // groundRow down (grass on top, dirt below); everything left at 0 is a
    // pit (bottomless - matches PlayerBehavior's y > 1080 game-over check).
    std::vector<std::vector<int>> mapVector(mapHeight, std::vector<int>(mapWidth, TILE_EMPTY));

    auto setCell = [&](int col, int row, int value) {
        if (col >= 0 && col < mapWidth && row >= 0 && row < mapHeight) {
            mapVector[row][col] = value;
        }
    };

    auto fillGround = [&](int fromCol, int toCol) {
        for (int c = fromCol; c <= toCol; c++) {
            setCell(c, groundRow, TILE_GRASS);
            for (int r = groundRow + 1; r < mapHeight; r++) {
                setCell(c, r, TILE_DIRT);
            }
        }
    };

    // Ground sections, separated by pits (3-column pits are a comfortable
    // single jump; two 7-column pits below get a floating stepping stone).
    fillGround(0, 9);
    fillGround(13, 21);
    fillGround(25, 33);
    // 34-40: wide pit, bridged by a single floating platform at col 37, one
    // row below the main ground - reachable both ways with plenty of margin
    // (a short drop in, a small 1-tile hop back out). Uses the obstacle tile
    // rather than grass so an isolated single-tile platform doesn't read as
    // a stray chunk of ground.
    setCell(37, groundRow + 1, TILE_OBSTACLE);
    fillGround(41, 49);
    fillGround(53, 61);
    fillGround(65, 75);
    // 76-82: wide pit, bridged by a single floating platform at col 79.
    setCell(79, groundRow, TILE_OBSTACLE);
    fillGround(83, 95);
    fillGround(96, 109);

    // Ground-level spikes: jump over them.
    setCell(17, groundRow - 1, TILE_SPIKE_UP);
    setCell(56, groundRow - 1, TILE_SPIKE_UP);
    setCell(57, groundRow - 1, TILE_SPIKE_UP);
    setCell(92, groundRow - 1, TILE_SPIKE_UP);

    // Purely decorative low ceiling over the start of the level (well clear
    // of any jump players actually need to make).
    for (int c = 5; c <= 7; c++) {
        setCell(c, groundRow - 3, TILE_OBSTACLE);
    }

    // A spike hangs over the approach to the second wide pit's stone platform,
    // mounted on its own obstacle block (flush, no gap) instead of floating
    // unsupported. It sits close to where a standing jump naturally peaks, so
    // clearing it takes a deliberately timed jump rather than a blind one.
    setCell(77, groundRow - 2, TILE_OBSTACLE);
    setCell(77, groundRow - 1, TILE_SPIKE_DOWN);

    map->createComponent<MapBehavior>()->newGenerateMap(mapVector);
    addEntity(map);

    //Player
    Entity* player = createEntity();

    player->createComponent<TransformComponent>()->init({ 4.f * tileSize, 440.f });
    PlayerBehavior* playerBehavior = player->createComponent<PlayerBehavior>();
    playerBehavior->init();
    player->createComponent<MovableComponent>()->init({ 0.f, 0.f }, 300.f);
    player->createComponent<AliveComponent>()->init(100.f, "GameOver");
    player->createPhysics({ 50.f, 80.f }, b2_dynamicBody, true, 10);

    Render* playerRender = new Render(*rm->loadTexture("characters.png"), sf::Rect2i({ 512, 516 }, { 128, 128 }), {64, 64 + 20});
    player->addComponent(playerRender);
    addEntity(player);

    //Enemies - spawn positions are hardcoded, patrol is relative to each spawn point.
    // Some are killable (stomp on top to defeat them), others are hazards that
    // always hurt the player on contact and are tinted red to read as dangerous.
    struct EnemySpawn { float x; float y; float patrolRange; bool killable; };
    std::vector<EnemySpawn> enemySpawns = {
        { 28.f * tileSize, 440.f, 130.f, true },
        { 44.f * tileSize, 440.f, 130.f, true },
        { 48.f * tileSize, 440.f, 100.f, false },
        { 68.f * tileSize, 440.f, 130.f, true },
        { 87.f * tileSize, 440.f, 130.f, false },
    };

    for (const EnemySpawn& spawn : enemySpawns) {
        Entity* enemy = createEntity();

        enemy->createComponent<TransformComponent>()->init({ spawn.x, spawn.y });
        enemy->createComponent<MovableComponent>()->init({ 0.f, 0.f }, 150.f);
        enemy->createComponent<EnemyBehavior>()->init(spawn.patrolRange, spawn.killable);
        enemy->createComponent<AliveComponent>()->init(100.f);
        enemy->createPhysics({ 56.f, 42.f }, b2_dynamicBody, true, 1);

        Render* enemyRender = new Render(*rm->loadTexture("enemies.png"), sf::Rect2i({ 192, 325 }, { 64, 64 }), { 32, 32 + 10 });
        if (!spawn.killable) {
            enemyRender->getSprite().color = sf::Color(255, 110, 110);
        }
        enemy->addComponent(enemyRender);
        addEntity(enemy);
    }

    //Coins - hardcoded positions above ground stretches and the stone bridges.
    std::vector<sf::Vec2f> coinPositions = {
        { 3.f * tileSize, 400.f }, { 6.f * tileSize, 400.f },
        { 20.f * tileSize, 400.f },
        { 31.f * tileSize, 400.f }, { 32.f * tileSize, 400.f },
        { 42.f * tileSize, 400.f },
        { 60.f * tileSize, 400.f },
        { 71.f * tileSize, 400.f }, { 73.f * tileSize, 400.f },
        { 79.f * tileSize, 350.f },
        { 90.f * tileSize, 400.f }, { 94.f * tileSize, 400.f },
        // Kept a safe few columns clear of the flag at col 106, since touching
        // it ends the level immediately - anything past it is unreachable.
        { 99.f * tileSize, 400.f }, { 101.f * tileSize, 400.f }, { 103.f * tileSize, 400.f },
    };

    for (const sf::Vec2f& pos : coinPositions) {
        Entity* coin = createEntity();
        coin->createComponent<TransformComponent>()->init(pos);
        coin->createComponent<CoinBehavior>()->init(10);

        Render* coinRender = new Render(*rm->loadTexture("coin.png"), sf::Rect2i({ 0, 0 }, { 64, 64 }), { 32.f, 32.f });
        coin->addComponent(coinRender);
        addEntity(coin);
    }

    //End-of-level flag.
    Entity* flag = createEntity();
    flag->createComponent<TransformComponent>()->init({ 106.f * tileSize, 480.f });
    flag->createComponent<FlagBehavior>()->init();

    Render* flagRender = new Render(*rm->loadTexture("flag.png"), sf::Rect2i({ 0, 0 }, { 64, 64 }), { 32.f, 32.f });
    flag->addComponent(flagRender);
    addEntity(flag);

    //HUD - fixed to the screen regardless of where the gameplay camera looks.
    sf::Font* uiFont = rm->loadFont("Kenney Pixel.ttf");

    // ScoreUpdate is created before scoreRender is added so its backing
    // panel (drawn in ScoreUpdate::draw) renders behind the score text
    // instead of over it - same ordering trick as the tutorial hints below.
    Entity* scoreEntity = createEntity();
    TextRenderer* scoreRender = new TextRenderer({ 1700.f, 60.f }, *uiFont, "Score : 0", 48);
    scoreEntity->createComponent<ScoreUpdate>()->init(scoreRender);
    scoreEntity->addComponent(scoreRender);
    addUIEntity(scoreEntity);

    Entity* healthBarEntity = createEntity();
    healthBarEntity->createComponent<HealthBarUI>()->init();
    addUIEntity(healthBarEntity);

#ifdef __EMSCRIPTEN__
    // Onboarding hints for first-time web players - hidden by default (empty
    // string) until TutorialHints shows/hides them based on player progress.
    // The text entities are constructed (but not yet added to the scene) so
    // TutorialHints can size its backing panels from their bounds; it is
    // then added as a UI entity *before* the text entities, so its panels
    // are drawn first and sit behind the text instead of covering it.
    Entity* moveHintEntity = createEntity();
    sf::Utf8String moveHintStr = WebBridge::isEnglish()
        ? "Press 'A' and 'D' to move"
        : "Appuyez sur 'D' et 'Q' pour vous déplacer";
    TextRenderer* moveHintRender = new TextRenderer({ 960.f, 900.f }, *uiFont, moveHintStr, 40);
    moveHintEntity->addComponent(moveHintRender);

    Entity* jumpHintEntity = createEntity();
    TextRenderer* jumpHintRender = new TextRenderer({ 960.f, 900.f }, *uiFont, "", 40);
    jumpHintEntity->addComponent(jumpHintRender);

    Entity* tutorialHintsEntity = createEntity();
    tutorialHintsEntity->createComponent<TutorialHints>()->init(playerBehavior, moveHintRender, jumpHintRender);
    addUIEntity(tutorialHintsEntity);

    addUIEntity(moveHintEntity);
    addUIEntity(jumpHintEntity);
#endif
}
