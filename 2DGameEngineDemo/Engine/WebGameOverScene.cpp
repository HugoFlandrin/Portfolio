#include "WebGameOverScene.h"
#include "SceneManager.h"
#include "WebBridge.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

WebGameOverScene::WebGameOverScene() {}

void WebGameOverScene::init() {
	setCamera();

	SceneManager* sm = SceneManager::instance();
	WebBridge::notifyGameOver(sm->getLastRunWon(), sm->getLastScore());

#ifdef __EMSCRIPTEN__
	emscripten_cancel_main_loop();
#endif
}
