#include "WebBridge.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include "AudioManager.h"
#include "Engine.h"

namespace WebBridge {

void notifyGameOver(bool won, int score, bool twoPlayer, int scoreP1, int scoreP2) {
	EM_ASM({
		if (window.parent) {
			window.parent.postMessage({
				type: 'gameOver', won: !!$0, score: $1,
				twoPlayer: !!$2, scoreP1: $3, scoreP2: $4
			}, '*');
		}
	}, won ? 1 : 0, score, twoPlayer ? 1 : 0, scoreP1, scoreP2);
}

void requestQuit() {
	EM_ASM({
		if (window.parent) {
			window.parent.postMessage({ type: 'closeEmbeddedGame' }, '*');
		}
	});
}

bool isEnglish() {
	return EM_ASM_INT({
		var params = new URLSearchParams(window.location.search);
		return params.get('lang') === 'en' ? 1 : 0;
	}) != 0;
}

}

EMSCRIPTEN_KEEPALIVE
void ShmupSetMusicVolume(float _volume) {
	AudioManager::instance()->setMusicVolume(_volume);
}

EMSCRIPTEN_KEEPALIVE
void ShmupSetSfxVolume(float _volume) {
	AudioManager::instance()->setSfxVolume(_volume);
}

EMSCRIPTEN_KEEPALIVE
void ShmupSetMusicMuted(int _muted) {
	AudioManager::instance()->setMusicMuted(_muted != 0);
}

EMSCRIPTEN_KEEPALIVE
void ShmupSetSfxMuted(int _muted) {
	AudioManager::instance()->setSfxMuted(_muted != 0);
}

EMSCRIPTEN_KEEPALIVE
void ShmupSetPaused(int _paused) {
	Engine::instance()->setPaused(_paused != 0);
}

#endif
