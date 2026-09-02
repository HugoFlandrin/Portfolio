#include "Engine.h"
#include "SceneManager.h"
#include "GameSelectScene.h"
#include "FirstMarioScene.h"
#include "GameOverScene.h"
#include "ShmupScene.h"

// Single desktop entry point for both games: starts on GameSelectScene,
// which sends the player into either FirstMarioScene (Platformer) or
// ShmupScene (Shmup) via its buttons. Both games' "GameOver" flow shares
// the same GameOverScene (fully generic - just reads score/win-loss off
// SceneManager) and its retry button returns to GameSelect, not straight
// back into whichever game just ended.
int main()
{
    // A normal desktop window, shared by every scene (GameSelect, the
    // Platformer, and the Shmup) - the Shmup's mobile-portrait look is
    // achieved with a letterboxed camera viewport instead of resizing this
    // shared window (see ShmupScene::init()), so the Platformer/menus render
    // at their intended landscape size regardless of it.
    Engine::configureWindow({ 1920u, 1080u }, "GameHub");
    Engine* engine = Engine::instance();
    SceneManager* sceneManager = SceneManager::instance();

    static GameSelectScene selectScene;
    selectScene.setName("GameSelect");
    sceneManager->addScene(&selectScene);

    static FirstMarioScene platformerLevel;
    platformerLevel.setName("FirstLevel");
    sceneManager->addScene(&platformerLevel);

    static ShmupScene shmupLevel;
    shmupLevel.setName("ShmupLevel");
    sceneManager->addScene(&shmupLevel);

    static GameOverScene gameOver;
    gameOver.setName("GameOver");
    sceneManager->addScene(&gameOver);

    sceneManager->setCurrentScene(&selectScene);
    sceneManager->getCurrentScene()->init();
    sceneManager->getCurrentScene()->commitPendingEntities();

    engine->run();
}
