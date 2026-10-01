/*
 * Self-uninstalling: this replaces an AudioWorklet experiment's service
 * worker (see CMakeLists.txt's own note - that approach was reverted after
 * it crashed game loading on real-world testing, not just in-sandbox).
 * Browsers keep running a previously-installed service worker indefinitely
 * even after this file changes server-side, so anyone who loaded the game
 * while that experiment was live needs this replacement to actually remove
 * it - deleting the file outright would leave their installation stuck
 * running forever, undetected. Kept in place (not deleted) as a permanent
 * safety net for exactly that case.
 */
self.addEventListener("install", () => self.skipWaiting());
self.addEventListener("activate", (event) => {
	event.waitUntil(
		self.registration.unregister().then(() => self.clients.matchAll()).then((clients) => {
			clients.forEach((client) => client.navigate(client.url));
		})
	);
});
