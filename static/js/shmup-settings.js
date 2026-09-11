(() => {
  const gear = document.getElementById("game-settings-toggle");
  const panel = document.getElementById("game-settings-panel");
  const backdrop = document.getElementById("game-settings-backdrop");
  const frame = document.getElementById("game-frame");
  const musicVolumeInput = document.getElementById("music-volume");
  const sfxVolumeInput = document.getElementById("sfx-volume");
  const musicMuteBtn = document.getElementById("music-mute");
  const sfxMuteBtn = document.getElementById("sfx-mute");

  if (!gear || !panel || !backdrop || !frame || !musicVolumeInput || !sfxVolumeInput || !musicMuteBtn || !sfxMuteBtn) return;

  const STORAGE = {
    musicVolume: "shmup-music-volume",
    sfxVolume: "shmup-sfx-volume",
    musicMuted: "shmup-music-muted",
    sfxMuted: "shmup-sfx-muted",
  };

  const state = {
    // 50, not 100, for both: matches AudioManager's own musicVolume/
    // sfxVolume defaults (see their own comments) - the music was drowning
    // out the sfx at full volume, so both sliders start at their center
    // instead of their max, same as each other.
    musicVolume: Number(localStorage.getItem(STORAGE.musicVolume) ?? 50),
    sfxVolume: Number(localStorage.getItem(STORAGE.sfxVolume) ?? 50),
    musicMuted: localStorage.getItem(STORAGE.musicMuted) === "true",
    sfxMuted: localStorage.getItem(STORAGE.sfxMuted) === "true",
  };

  const send = (type, value) => {
    frame.contentWindow?.postMessage({ type, value }, "*");
  };

  const applyAll = () => {
    send("setMusicVolume", state.musicVolume / 100);
    send("setSfxVolume", state.sfxVolume / 100);
    send("setMusicMuted", state.musicMuted);
    send("setSfxMuted", state.sfxMuted);
  };

  const syncUI = () => {
    musicVolumeInput.value = state.musicVolume;
    sfxVolumeInput.value = state.sfxVolume;
    musicMuteBtn.classList.toggle("is-muted", state.musicMuted);
    musicMuteBtn.setAttribute("aria-pressed", String(state.musicMuted));
    sfxMuteBtn.classList.toggle("is-muted", state.sfxMuted);
    sfxMuteBtn.setAttribute("aria-pressed", String(state.sfxMuted));
  };

  syncUI();

  const openSettings = () => {
    panel.classList.add("is-open");
    panel.setAttribute("aria-hidden", "false");
    backdrop.classList.add("is-active");
    send("setPaused", true);
  };

  // Also refocuses the game frame: closing via the gear (or the backdrop,
  // which sits on top of the iframe while open) leaves keyboard focus on
  // whatever HTML element was actually clicked, not the iframe itself -
  // without this, arrow-key/WASD input stays dead until the player clicks
  // the game a second time.
  const closeSettings = () => {
    panel.classList.remove("is-open");
    panel.setAttribute("aria-hidden", "true");
    backdrop.classList.remove("is-active");
    send("setPaused", false);
    frame.focus();
    try {
      frame.contentWindow?.focus();
    } catch {

    }
  };

  gear.addEventListener("click", () => {
    if (panel.classList.contains("is-open")) {
      closeSettings();
    } else {
      openSettings();
    }
  });

  backdrop.addEventListener("click", closeSettings);

  musicVolumeInput.addEventListener("input", () => {
    state.musicVolume = Number(musicVolumeInput.value);
    localStorage.setItem(STORAGE.musicVolume, String(state.musicVolume));
    send("setMusicVolume", state.musicVolume / 100);
  });

  sfxVolumeInput.addEventListener("input", () => {
    state.sfxVolume = Number(sfxVolumeInput.value);
    localStorage.setItem(STORAGE.sfxVolume, String(state.sfxVolume));
    send("setSfxVolume", state.sfxVolume / 100);
  });

  musicMuteBtn.addEventListener("click", () => {
    state.musicMuted = !state.musicMuted;
    localStorage.setItem(STORAGE.musicMuted, String(state.musicMuted));
    syncUI();
    send("setMusicMuted", state.musicMuted);
  });

  sfxMuteBtn.addEventListener("click", () => {
    state.sfxMuted = !state.sfxMuted;
    localStorage.setItem(STORAGE.sfxMuted, String(state.sfxMuted));
    syncUI();
    send("setSfxMuted", state.sfxMuted);
  });

  // Re-applies the stored preferences every time the game announces it
  // finished booting (see shell-shmup.html's onRuntimeInitialized) - each
  // open/replay spins up a brand-new wasm instance at its audio defaults,
  // since game-embed.js reassigns frame.src fresh every time.
  window.addEventListener("message", (event) => {
    if (event.data?.type === "shmupReady") {
      applyAll();
    }
  });
})();
