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

    // Second registered instance of the same scene class, switched to an
    // endless survival run instead of the timed one (see
    // ShmupScene::setInfiniteMode()) - kept under its own scene name so both
    // can coexist and be selected independently.
    static ShmupScene level1Infinite;
    level1Infinite.setName("ShmupInfinite");
    level1Infinite.setInfiniteMode(true);
    // TEMP DEV: 2-player co-op is being built directly on top of the
    // infinite scene (the intended flagship combo) so it's reachable from
    // the same dev-only default entry point (see shell-shmup.html) without
    // more URL fiddling - revisit whether infinite/2-player should be
    // selectable independently once both are further along.
    level1Infinite.setTwoPlayer(true);
    level1Infinite.setShowControlHint(isDesktop);
    sceneManager->addScene(&level1Infinite);

#ifdef __EMSCRIPTEN__
    static WebGameOverScene gameOver;
#else
    static GameOverScene gameOver;
#endif
    gameOver.setName("GameOver");
    sceneManager->addScene(&gameOver);

    // window.__shmupMode (see shell-shmup.html) picks which of the two
    // scenes above this run starts on - same read-a-JS-global pattern as
    // window.__shmupDevice above. Native desktop has no such global to read,
    // so it always falls back to the default timed mode.
#ifdef __EMSCRIPTEN__
    bool startInfinite = EM_ASM_INT({ return window.__shmupMode === 'infinite' ? 1 : 0; });
#else
    bool startInfinite = false;
#endif
    sceneManager->setCurrentScene(startInfinite ? &level1Infinite : &level1);
    sceneManager->getCurrentScene()->init();
    sceneManager->getCurrentScene()->commitPendingEntities();

    engine->run();
}
