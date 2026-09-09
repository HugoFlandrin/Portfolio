#include "WebBridge.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

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

#endif
