#include "Engine.h"
#include "SceneManager.h"
#include "ShmupScene.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "WebGameOverScene.h"
#else
// Native desktop has no surrounding web page to notify (see
// WebGameOverScene/WebBridge, both Emscripten-only) - reuse the Platformer's
// fully generic GameOverScene instead. Its retry button targets "GameSelect"
// (see GameOverScene.cpp), which this standalone exe doesn't register - a
// no-op here, same accepted tradeoff as the standalone Platformer.exe.
#include "GameOverScene.h"
#endif

// No desktop menu/settings equivalent: this game is touch-only by design
// (see ShipBehavior/TouchInput), so unlike the platformer there's no
// meaningful non-web entry point to branch to - it always starts directly
// on ShmupScene, same as the platformer's web build does.
int main()
{
    // Must happen before the first Engine::instance() call below - that's
    // when the singleton (and its OS window/canvas) actually gets built.
    // The requested size MUST match shell-shmup.html's canvas exactly (see
    // that file for why) - read back the same value its inline script
    // already computed and set the canvas to, instead of guessing a fixed
    // resolution here. 720x1280 is only the non-web fallback (irrelevant in
    // practice - this game is touch-only).
#ifdef __EMSCRIPTEN__
    unsigned int shmupWidth = EM_ASM_INT({ return window.__shmupCanvasSize.w; });
    unsigned int shmupHeight = EM_ASM_INT({ return window.__shmupCanvasSize.h; });
#else
    unsigned int shmupWidth = 720u;
    unsigned int shmupHeight = 1280u;
#endif
    Engine::configureWindow({ shmupWidth, shmupHeight }, "Shmup");
    Engine* engine = Engine::instance();
    SceneManager* sceneManager = SceneManager::instance();

    // MUST be `static` (not plain locals): emscripten_set_main_loop_arg
    // unwinds main()'s stack immediately after being called, which would
    // destroy stack-local scene objects before the first frame ever runs.
    static ShmupScene level1;
    level1.setName("ShmupLevel");
    // Native desktop has no "device" concept to read - always show the
    // arrow-keys hint there. On web, it depends on which trigger opened this
    // (see shell-shmup.html's window.__shmupDevice, set from game-embed.js's
    // own coarse-pointer/narrow-viewport heuristic).
#ifdef __EMSCRIPTEN__
    bool isDesktop = EM_ASM_INT({ return window.__shmupDevice !== 'mobile' ? 1 : 0; });
#else
    bool isDesktop = true;
#endif
    level1.setShowControlHint(isDesktop);
    sceneManager->addScene(&level1);

#ifdef __EMSCRIPTEN__
    static WebGameOverScene gameOver;
#else
    static GameOverScene gameOver;
#endif
    gameOver.setName("GameOver");
    sceneManager->addScene(&gameOver);

    sceneManager->setCurrentScene(&level1);
    sceneManager->getCurrentScene()->init();
    sceneManager->getCurrentScene()->commitPendingEntities();

    engine->run();
}
