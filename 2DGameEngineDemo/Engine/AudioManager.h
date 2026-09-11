#pragma once
#include <string>
#include <vector>
#include <memory>
#include <SFML/Audio/Sound.hpp>

namespace sf {
	class AudioContext;
	class PlaybackDevice;
}

// Fire-and-forget SFX playback, plus a single looping background music
// track. In this SFML fork, sf::Sound is neither copyable nor movable and
// must be bound to a PlaybackDevice that outlives it, so every playing
// sound is heap-allocated (via unique_ptr, so the vector itself only ever
// moves pointers around) and kept in activeSounds for as long as it takes
// to finish - pruning the ones that are done on the next call instead of
// every frame.
class AudioManager
{
	sf::AudioContext* audioContext = nullptr;
	sf::PlaybackDevice* playbackDevice = nullptr;
	std::vector<std::unique_ptr<sf::Sound>> activeSounds;

	// The background track, fully decoded into memory and played back with
	// looping=true - same sf::Sound/SoundBuffer path as every SFX (see
	// playSound()), deliberately NOT sf::Music/MusicReader (streaming
	// decode). Streaming was the original approach, but it was the one
	// audio path that came out distorted specifically on the Emscripten
	// build, unaffected by volume - a strong sign that was a
	// streaming/decode-timing bug in this fork's web audio backend, not a
	// mixing/gain problem. The track is only ~3MB, small enough to just
	// preload whole like everything else and sidestep that path entirely.
	// Only ever set up once - see playMusicLooping()'s own comment.
	sf::Sound* music = nullptr;

	// Master category levels, driven by the in-browser settings panel (see
	// WebBridge's ShmupSet*Volume/ShmupSet*Muted exports) - kept separate
	// from mute so the UI can remember the last volume while silenced,
	// matching typical OS/app volume UX. Applied on top of each individual
	// playSound() call's own _volume (itself already tuned per-cue - see
	// e.g. BulletBehavior.cpp's player/enemy shot volumes). Both default to
	// half, not full: at 100% the music was drowning out the tuned-per-cue
	// sfx almost entirely - matched by the settings panel's two sliders also
	// starting at their center/50 position (see pages/space-shooter.html),
	// so either slider still reaches today's full-strength mix at its max
	// instead of silently capping below what it visually shows.
	float musicVolume = 0.5f;
	float sfxVolume = 0.5f;
	bool musicMuted = false;
	bool sfxMuted = false;

	static AudioManager* m_instance;

	AudioManager();

	// music->setVolume(effective music level) - a no-op if no music is
	// playing yet (setMusicVolume/setMusicMuted can be called, e.g. from a
	// settings panel opened before gameplay starts, before playMusicLooping()
	// ever runs).
	void applyMusicVolume();

public:
	// _volume: 0 (silent) to 1 (full volume, the default) - this cue's own
	// mix level, further scaled by the master sfx volume/mute below.
	void playSound(const std::string& _path, float _volume = 1.f);

	// Starts _path looping forever. Safe to call again later (e.g. replaying
	// a run re-enters ShmupScene::init(), which calls this again) - restarts
	// the same track from the beginning instead of stacking a second copy on
	// top of itself or reloading it from scratch. See stopMusic().
	void playMusicLooping(const std::string& _path);

	// Stops the background track (e.g. on game over - see
	// EnemySpawner.cpp/CoopGameOverWatcher.cpp) - a no-op if none is playing.
	// The next playMusicLooping() call restarts it from the beginning.
	void stopMusic();

	// Master category controls for the settings panel. Music applies to the
	// currently-playing track immediately; sfx applies to every sound played
	// from here on (already-playing one-shots are too short-lived for this
	// to matter).
	void setMusicVolume(float _volume);
	void setSfxVolume(float _volume);
	void setMusicMuted(bool _muted);
	void setSfxMuted(bool _muted);

	static AudioManager* instance();
};
