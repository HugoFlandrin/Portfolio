#include "Engine.h"
#include "FirstMarioScene.h"
#include "SecondMarioScene.h"
#include "SceneManager.h"
#include "GameOverScene.h"
#include "MenuScene.h"
#include "SettingsScene.h"

#ifdef __EMSCRIPTEN__
#include "WebGameOverScene.h"
#endif

int main()
{
    //Engine
    Engine* engine = Engine::instance();

    //Scenes
    SceneManager* sceneManager = SceneManager::instance();

#ifdef __EMSCRIPTEN__
    // Web build: no start/settings menu - the level starts immediately when
    // the page embeds the game. End-of-run notifies the hosting page instead
    // of drawing an in-canvas game-over menu (see WebGameOverScene).
    //
    // These MUST be `static` (not plain locals): emscripten_set_main_loop_arg
    // unwinds main()'s stack immediately after being called (even with
    // "simulate infinite loop"), which would destroy stack-local scene
    // objects before the first frame ever runs, leaving SceneManager with
    // dangling pointers. `static` gives them the same effectively-forever
    // lifetime that the desktop build gets for free from its blocking
    // while-loop (main() never returning).
    static FirstMarioScene level1;
    level1.setName("FirstLevel");
    sceneManager->addScene(&level1);

    // Deuxieme niveau desactive : le jeu ne comporte pour l'instant qu'un seul niveau.
    // SecondMarioScene.h/.cpp restent dans le projet pour une reactivation future.
    //SecondMarioScene level2;
    //level2.setName("SecondLevel");
    //sceneManager->addScene(&level2);

    static WebGameOverScene gameOver;
    gameOver.setName("GameOver");
    sceneManager->addScene(&gameOver);

    sceneManager->setCurrentScene(&level1);
#else
    MenuScene menu;
    menu.setName("Menu");
    sceneManager->addScene(&menu);

    SettingsScene settings;
    settings.setName("Settings");
    sceneManager->addScene(&settings);

    FirstMarioScene level1;
    level1.setName("FirstLevel");
    sceneManager->addScene(&level1);

    // Deuxieme niveau desactive : le jeu ne comporte pour l'instant qu'un seul niveau.
    // SecondMarioScene.h/.cpp restent dans le projet pour une reactivation future.
    //SecondMarioScene level2;
    //level2.setName("SecondLevel");
    //sceneManager->addScene(&level2);

    GameOverScene gameOver;
    gameOver.setName("GameOver");
    sceneManager->addScene(&gameOver);

    sceneManager->setCurrentScene(&menu);
#endif

    sceneManager->getCurrentScene()->init();
    sceneManager->getCurrentScene()->commitPendingEntities();

    //Run
    engine->run();
}
