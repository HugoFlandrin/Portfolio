#include "MapBehavior.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "TransformComponent.h"
#include "AliveComponent.h"
#include "EnemyBehavior.h"
#include "Render.h"
#include "SpikeBehavior.h"
#include "RigidBody.h"
#include "BoxCollider.h"
#include <cmath>
#include <algorithm>

void MapBehavior::generateMap() {

    SceneManager* sm = SceneManager::instance();
    ResourceManager* rm = ResourceManager::instance();

    generateBackground(1920.f, 1080.f);


    float groundHeight = 770.f;

    // Grass
    //--1
    Entity* grassTile1 = sm->getCurrentScene()->createEntity();
    grassTile1->createComponent<TransformComponent>()->init({ 480.f, groundHeight - 64.f });
    
    sf::Rect2i grassTileRect1({ 0, 0 }, { 960, 64 });
    sf::Texture* grassTileTexture1 = rm->loadTexture("grassTile.png");
    grassTileTexture1->setWrapMode(sf::TextureWrapMode::Repeat);

    Render* grassTileRender1 = new Render(*grassTileTexture1, grassTileRect1, {480, 32});

    grassTile1->addComponent(grassTileRender1);
    grassTile1->createPhysics({ 960, 64 });
    sm->getCurrentScene()->addEntity(grassTile1);
    mapTiles.push_back(grassTile1);

    //--2
    Entity* grassTile2 = sm->getCurrentScene()->createEntity();
    grassTile2->createComponent<TransformComponent>()->init({ 1536.f, groundHeight - 64.f });

    sf::Rect2i grassTileRect2({ 0, 0 }, { 768, 64 });
    sf::Texture* grassTileTexture2 = rm->loadTexture("grassTile.png");
    grassTileTexture2->setWrapMode(sf::TextureWrapMode::Repeat);

    Render* grassTileRender2 = new Render(*grassTileTexture2, grassTileRect2, { 384, 32 });

    grassTile2->addComponent(grassTileRender2);
    grassTile2->createPhysics({ 768, 64 });
    sm->getCurrentScene()->addEntity(grassTile2);
    mapTiles.push_back(grassTile2);


    //Dirt 
    //--1
    Entity* dirtTile1 = sm->getCurrentScene()->createEntity();
    dirtTile1->createComponent<TransformComponent>()->init({ 480.f, groundHeight + 128 });

    sf::Rect2i dirtTileRect1({ 0, 0 }, { 960, 320 });
    sf::Texture* dirtTileTexture1 = rm->loadTexture("dirtTile.png");
    dirtTileTexture1->setWrapMode(sf::TextureWrapMode::Repeat);

    Render* dirtTileRender1 = new Render(*dirtTileTexture1, dirtTileRect1, { 480, 160 });

    dirtTile1->addComponent(dirtTileRender1);
    dirtTile1->createPhysics({ 960, 320 });
    sm->getCurrentScene()->addEntity(dirtTile1);
    mapTiles.push_back(dirtTile1);

    //--2
    Entity* dirtTile2 = sm->getCurrentScene()->createEntity();
    dirtTile2->createComponent<TransformComponent>()->init({ 1536.f, groundHeight + 128 });

    sf::Rect2i dirtTileRect2({ 0, 0 }, { 768, 320 });
    sf::Texture* dirtTileTexture2 = rm->loadTexture("dirtTile.png");
    dirtTileTexture2->setWrapMode(sf::TextureWrapMode::Repeat);

    Render* dirtTileRender2 = new Render(*dirtTileTexture2, dirtTileRect2, { 384, 160 });

    dirtTile2->addComponent(dirtTileRender2);
    dirtTile2->createPhysics({ 768, 320 });
    sm->getCurrentScene()->addEntity(dirtTile2);
    mapTiles.push_back(dirtTile2);

}

void MapBehavior::newGenerateMap(std::vector<std::vector<int>> _map) {

    SceneManager* sm = SceneManager::instance();
    AScene* scene = sm->getCurrentScene();

    float tileSize = 64.f;

    int mapHeight = _map.size();
    int mapWidth = (_map)[0].size();

    generateBackground(mapWidth * tileSize, mapHeight * tileSize);

    for (int y = 0; y < mapHeight; y++)
    {
        int blocStartX = -1;
        int blocCurrentType = 0;

        for (int x = 0; x <= mapWidth; x++)
        {
            int cellValue = 0;

            if (x < mapWidth)
            {
                cellValue = (_map)[y][x];
            }

            // Si on est dans un bloc du m�me type, on ne fait rien
            if (cellValue != blocCurrentType || cellValue == 0)
            {
                if (blocCurrentType != 0)
                {
                    int blocLength = x - blocStartX;
                    float blocWidth = blocLength * tileSize;
                    float blocHeight = tileSize;

                    float blocCenterX = blocStartX * tileSize + blocWidth / 2.f;
                    float blocCenterY = y * tileSize + blocHeight / 2.f;

                    Entity* blocEntity = scene->createEntity();
                    blocEntity->createComponent<TransformComponent>()
                        ->init({ blocCenterX, blocCenterY });

                    const char* texturePath;
                    bool isSpike = false;
                    bool spikeFacesDown = false;

                    if (blocCurrentType == 1) {
                        texturePath = "grassTile.png";
                    }
                    else if (blocCurrentType == 2) {
                        texturePath = "dirtTile.png";
                    }
                    else if (blocCurrentType == 3) {
                        // Plain solid obstacle: blocks movement/jumps, deals no damage.
                        texturePath = "obstacleTile.png";
                    }
                    else if (blocCurrentType == 4) {
                        // Spike hazard pointing up, sitting on the ground.
                        texturePath = "spikesTile.png";
                        isSpike = true;
                    }
                    else if (blocCurrentType == 5) {
                        // Spike hazard pointing down, meant to be placed on the
                        // underside of a floating platform.
                        texturePath = "spikesTile.png";
                        isSpike = true;
                        spikeFacesDown = true;
                    }
                    else
                        texturePath = "dirtTile.png";

                    ResourceManager* rm = ResourceManager::instance();
                    sf::Texture* texture = rm->loadTexture(texturePath);
                    texture->setWrapMode(sf::TextureWrapMode::Repeat);

                    Render* blocRender = new Render(*texture, sf::Rect2i({ 0, 0 }, {(int)blocWidth, (int)blocHeight}), {blocWidth / 2.f, blocHeight / 2.f});
                    if (spikeFacesDown) {
                        blocRender->setFlipY(true);
                    }
                    blocEntity->addComponent(blocRender);

                    if (isSpike) {
                        // spikesTile.png only has ~34px of actual spike art (the rest
                        // is transparent padding above/below it depending on facing),
                        // so the collider must be shrunk and offset to match, or the
                        // player would visibly float above/below the drawn spikes.
                        const float spikeVisibleHeight = 34.f;
                        const float spikePadding = tileSize - spikeVisibleHeight;
                        float spikeOffsetY = spikeFacesDown ? -spikePadding / 2.f : spikePadding / 2.f;

                        RigidBody* rigidBody = blocEntity->createComponent<RigidBody>();
                        rigidBody->init(true);
                        rigidBody->setBodyType(b2_staticBody);

                        BoxCollider* collider = blocEntity->createComponent<BoxCollider>();
                        collider->setSize({ blocWidth, spikeVisibleHeight });
                        collider->setOffset({ 0.f, spikeOffsetY });
                        collider->init(rigidBody);
                        collider->setDensity(0.f);
                        collider->setFriction(0.f);

                        blocEntity->createComponent<SpikeBehavior>()->init();
                    }
                    else {
                        blocEntity->createPhysics({ blocWidth, blocHeight });
                    }
                    scene->addEntity(blocEntity);
                }

                // D�marrage d�un nouveau bloc
                if (cellValue != 0)
                {
                    blocStartX = x;
                    blocCurrentType = cellValue;
                }
                else
                {
                    blocStartX = -1;
                    blocCurrentType = 0;
                }
            }
        }
    }

}

void MapBehavior::generateBackground(float _mapWidthPx, float _mapHeightPx) {

    SceneManager* sm = SceneManager::instance();
    ResourceManager* rm = ResourceManager::instance();

    // Background tiles are drawn from a 256x256 source rect, spaced 256px
    // apart in world space. Bounds are derived from the actual map size
    // (in px) plus a safety margin, so the sky/ground backdrop always
    // covers the whole scrollable level instead of a single hardcoded
    // screen-sized area.
    const int tileSpacing = 256;
    const int marginTiles = 3;
    // Always cover well past the y > 1080 death boundary below the ground,
    // regardless of how tall the actual tile map is.
    const float minCoveredHeightPx = 1800.f;

    int minX = -marginTiles;
    int maxX = static_cast<int>(std::ceil(_mapWidthPx / tileSpacing)) + marginTiles;
    int minY = -marginTiles;
    int maxY = static_cast<int>(std::ceil(std::max(_mapHeightPx, minCoveredHeightPx) / tileSpacing)) + marginTiles;

    for (int x = minX; x < maxX; ++x)
    {
        for (int y = minY; y < maxY; ++y)
        {
            Entity* bgTile = sm->getCurrentScene()->createEntity();
            bgTile->createComponent<TransformComponent>()->init({ float(x * tileSpacing), float(y * tileSpacing) });

            // {257,514} is a blank white region of backgrounds.png - it must
            // never be used as a filler. Flat sky-blue is the default for every
            // row except the cloud/horizon strip that sits just above the ground.
            sf::Rect2i rect({ 257, 769 }, { 256, 256 });

            if (y == 1) {
                rect.position = { 0, 0 };
            }
            else if (y > 1) {
                rect.position = { 257, 515 };
            }

            Render* bgRender = new Render(*rm->loadTexture("backgrounds.png"), rect, { 128.f, 128.f });
            // The camera center is a float that's essentially never pixel-aligned,
            // so exactly-abutting tiles can show a 1px gap (the window's black
            // clear color) between them at render time. Scaling each tile up
            // slightly makes it overlap its neighbours by a few pixels, hiding
            // that rounding seam.
            bgRender->getSprite().scale = { 1.01f, 1.01f };

            bgTile->addComponent(bgRender);
            sm->getCurrentScene()->addEntity(bgTile);
        }
    }
}

void MapBehavior::draw(sf::RenderTarget& _target, sf::RenderStates _states) const {
	for (Entity* it : mapTiles) {
		_target.draw(*it->getComponent<Render>());
	}
}

void MapBehavior::update(float _deltaTime) {

}