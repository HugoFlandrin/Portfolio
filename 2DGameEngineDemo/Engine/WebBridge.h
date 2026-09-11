#pragma once

// Thin JS interop layer used only by the web (Emscripten) build. Desktop
// never includes/compiles this file, so no #ifdef is needed at call sites -
// on any other target these would simply be link errors, which is fine
// since they are only ever called from web-only code paths.
namespace WebBridge {
	// Tells the hosting web page the run just ended (win/loss + score) so it
	// can show its own HTML popup instead of an in-canvas menu. _twoPlayer/
	// _scoreP1/_scoreP2 (all default off/0, so existing solo call sites are
	// unaffected) let that popup show Shmup 2-player co-op's individual
	// score breakdown alongside the shared total in _score.
	void notifyGameOver(bool won, int score, bool twoPlayer = false, int scoreP1 = 0, int scoreP2 = 0);

	// Tells the hosting web page the player asked to close the game (Escape
	// key) so it can tear down the embedding iframe/overlay.
	void requestQuit();

	// Reads the `?lang=` query param the hosting page passed to the game
	// iframe's src (game-embed.js sets it from the site's own i18n language),
	// so in-game UI text (hints, score) can match it without duplicating the
	// site's i18n dictionary in C++.
	bool isEnglish();
}

// Opposite direction from the rest of this file: these are called FROM the
// hosting page's JS INTO C++, not the other way around - the settings panel
// on the game's project page posts a message to the game iframe, and
// shell-shmup.html's own message listener forwards it here by calling
// Module._ShmupSet*(...) directly (the exported C symbol for each
// EMSCRIPTEN_KEEPALIVE function below). Plain `extern "C"` (not inside
// WebBridge's namespace) so the exported symbol names stay unmangled and
// predictable from JS. _volume is 0..1; _muted/_paused are a C bool (0/1).
extern "C" {
	void ShmupSetMusicVolume(float _volume);
	void ShmupSetSfxVolume(float _volume);
	void ShmupSetMusicMuted(int _muted);
	void ShmupSetSfxMuted(int _muted);

	// Freezes/resumes gameplay simulation (see Engine::tick()) while the
	// settings panel is open - the frame keeps rendering either way, just
	// frozen on whatever was last drawn.
	void ShmupSetPaused(int _paused);
}
