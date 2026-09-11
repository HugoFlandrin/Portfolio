#include "AudioManager.h"
#include "ResourceManager.h"
#include <SFML/Audio/AudioContext.hpp>
#include <SFML/Audio/PlaybackDevice.hpp>
#include <SFML/Audio/AudioSettings.hpp>
#include <algorithm>

namespace {
	// This mixer sums every simultaneously-playing sample linearly, with no
	// limiter/compressor stage, so several sounds overlapping at or near
	// full volume can in principle sum past 0dBFS and clip. General headroom
	// margin for that - NOT the fix for the specific music distortion this
	// was first written for (that turned out to be a streaming-decode issue,
	// unaffected by volume - see playMusicLooping()'s own comment; worth
	// keeping this anyway as normal mixing practice, since it costs nothing
	// and the settings panel's own sliders are unaffected: they stay on
	// their normal 0..1 range, this is folded in underneath them).
	constexpr float mixHeadroom = 0.6f;

	// Extra cap specifically on repeats of the exact same cue (see
	// AudioManager::playSound()) - several overlapping copies of one short
	// SFX add up on top of each other even with the headroom above, since
	// headroom alone assumes a handful of *different* sounds overlapping,
	// not the same one several times over.
	constexpr size_t maxConcurrentPerSound = 3;
}

AudioManager* AudioManager::m_instance = nullptr;

AudioManager::AudioManager() {
	audioContext = new sf::AudioContext(sf::AudioContext::create().value());
	playbackDevice = new sf::PlaybackDevice(sf::AudioContext::getDefaultPlaybackDeviceHandle().value());
}

void AudioManager::playSound(const std::string& _path, float _volume) {
	std::erase_if(activeSounds, [](const std::unique_ptr<sf::Sound>& _sound) {
		return !_sound->isPlaying();
	});

	sf::SoundBuffer* buffer = ResourceManager::instance()->loadSound(_path);

	size_t concurrentCount = std::count_if(activeSounds.begin(), activeSounds.end(), [buffer](const std::unique_ptr<sf::Sound>& _sound) {
		return &_sound->getBuffer() == buffer;
	});
	if (concurrentCount >= maxConcurrentPerSound) {
		// Silently dropped: the existing overlapping copies already cover
		// this same cue, so losing one more is inaudible - far simpler and
		// safer than trying to duck/limit the mix after the fact.
		return;
	}

	sf::AudioSettings settings;
	settings.volume = _volume * mixHeadroom * (sfxMuted ? 0.f : sfxVolume);
	auto sound = std::make_unique<sf::Sound>(*playbackDevice, *buffer, settings);
	sound->play();
	activeSounds.push_back(std::move(sound));
}

void AudioManager::playMusicLooping(const std::string& _path) {
	// Already set up from an earlier run (e.g. replaying re-enters
	// ShmupScene::init(), which calls this again) - restart it (play()
	// rewinds to the beginning, see MiniaudioSoundSource::play()) rather
	// than recreating it from scratch. Covers both "still playing" (harmless
	// restart) and "stopped" (stopMusic() was called on game over - see
	// EnemySpawner.cpp/CoopGameOverWatcher.cpp), which otherwise would have
	// stayed silent forever on this no-op-after-first-call path.
	if (music != nullptr) {
		music->play();
		return;
	}

	sf::SoundBuffer* buffer = ResourceManager::instance()->loadSound(_path);
	sf::AudioSettings settings;
	settings.looping = true;
	settings.volume = mixHeadroom * (musicMuted ? 0.f : musicVolume);
	music = new sf::Sound(*playbackDevice, *buffer, settings);
	music->play();
}

void AudioManager::stopMusic() {
	if (music != nullptr) {
		music->stop();
	}
}

void AudioManager::applyMusicVolume() {
	if (music != nullptr) {
		music->setVolume(mixHeadroom * (musicMuted ? 0.f : musicVolume));
	}
}

void AudioManager::setMusicVolume(float _volume) {
	musicVolume = _volume;
	applyMusicVolume();
}

void AudioManager::setSfxVolume(float _volume) {
	sfxVolume = _volume;
}

void AudioManager::setMusicMuted(bool _muted) {
	musicMuted = _muted;
	applyMusicVolume();
}

void AudioManager::setSfxMuted(bool _muted) {
	sfxMuted = _muted;
}

AudioManager* AudioManager::instance()
{
	if (m_instance == nullptr) {
		m_instance = new AudioManager();
	}
	return m_instance;
}
