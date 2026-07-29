#pragma once

// Thin JS interop layer used only by the web (Emscripten) build. Desktop
// never includes/compiles this file, so no #ifdef is needed at call sites -
// on any other target these would simply be link errors, which is fine
// since they are only ever called from web-only code paths.
namespace WebBridge {
	// Tells the hosting web page the run just ended (win/loss + score) so it
	// can show its own HTML popup instead of an in-canvas menu.
	void notifyGameOver(bool won, int score);

	// Tells the hosting web page the player asked to close the game (Escape
	// key) so it can tear down the embedding iframe/overlay.
	void requestQuit();

	// Reads the `?lang=` query param the hosting page passed to the game
	// iframe's src (game-embed.js sets it from the site's own i18n language),
	// so in-game UI text (hints, score) can match it without duplicating the
	// site's i18n dictionary in C++.
	bool isEnglish();
}
