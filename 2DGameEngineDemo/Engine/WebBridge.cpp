#include "WebBridge.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

namespace WebBridge {

void notifyGameOver(bool won, int score) {
	EM_ASM({
		if (window.parent) {
			window.parent.postMessage({ type: 'gameOver', won: !!$0, score: $1 }, '*');
		}
	}, won ? 1 : 0, score);
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
