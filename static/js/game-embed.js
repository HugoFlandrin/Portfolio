(() => {
  // Any element on the page can open a game (the featured screenshot on the
  // home page, a dedicated "Play" button on a project page, the mobile
  // shmup card, etc.) - they all share this data attribute instead of a
  // single fixed id. Optional per-trigger attributes:
  //   data-game-src="../static/shmup/index.html" - which build to load
  //     (falls back to static/game/index.html, the platformer's build).
  //   data-game-aspect="portrait" - use a 9:16 overlay frame instead of the
  //     default 16:9.
  //   data-device="desktop" | "mobile" - blocks the trigger on the wrong
  //     kind of device (coarse-pointer heuristic - see isMobile) and shows
  //     the matching notice instead of opening the overlay. Omit for a game
  //     playable on both.
  const triggers = document.querySelectorAll("[data-game-trigger]");
  const overlay = document.getElementById("game-overlay");
  const overlayFrameEl = document.querySelector(".game-overlay-frame");
  const frame = document.getElementById("game-frame");
  const closeBtn = document.getElementById("game-overlay-close");

  const resultPopup = document.getElementById("game-result-popup");
  const resultTitle = document.getElementById("game-result-title");
  const resultScore = document.getElementById("game-result-score");
  const retryBtn = document.getElementById("game-result-retry");
  const closeResultBtn = document.getElementById("game-result-close");

  const mobileNotice = document.getElementById("game-mobile-notice");
  const mobileNoticeClose = document.getElementById("game-mobile-notice-close");

  const desktopNotice = document.getElementById("game-desktop-notice");
  const desktopNoticeClose = document.getElementById("game-desktop-notice-close");

  if (!triggers.length || !overlay || !frame) return;

  // Derived from this script's own <script src>, so relative default paths
  // resolve correctly whether this runs from index.html (static/js/...) or
  // a page under pages/ (../static/js/...).
  const scriptSrc = document.currentScript?.src || "";
  const basePath = scriptSrc.replace(/static\/js\/game-embed\.js.*$/, "");
  const DEFAULT_GAME_SRC = `${basePath}static/game/index.html`;

  // Coarse-pointer/narrow-viewport heuristic, used both to gate device-locked
  // triggers (see data-device above) and to fall back a desktop-only game to
  // its "PC only" notice.
  const isMobile = () =>
    window.matchMedia("(pointer: coarse)").matches || !window.matchMedia("(pointer: fine)").matches;

  const openNotice = (notice) => {
    if (!notice) return;
    notice.classList.add("is-open");
    notice.setAttribute("aria-hidden", "false");
  };

  const closeNotice = (notice) => {
    if (!notice) return;
    notice.classList.remove("is-open");
    notice.setAttribute("aria-hidden", "true");
  };

  // The game reads keyboard input via the browser, which only delivers it to
  // whichever element/frame currently has focus - without this, the player
  // has to click a second time inside the iframe before movement keys work.
  const focusGameFrame = () => {
    frame.focus();
    try {
      frame.contentWindow?.focus();
      frame.contentDocument?.getElementById("canvas")?.focus();
    } catch {
      // Cross-origin iframe (shouldn't happen here, same-origin game build)
      // - nothing more we can do, fall back to just the frame having focus.
    }
  };

  frame.addEventListener("load", focusGameFrame);

  // Remembers the last-opened game's src/aspect so the "play again" button
  // (which has no trigger element of its own) reloads the right build.
  let activeSrc = DEFAULT_GAME_SRC;

  // Builds the iframe's src, including the actual on-screen pixel size of
  // .game-overlay-frame as ?w=&h= - the game's shell reads these to size
  // its canvas. Measuring here (the host page), right after the aspect
  // class is applied, is deliberate: reading getBoundingClientRect() forces
  // the browser to resolve any pending layout synchronously, so the value
  // is always correct immediately, with no extra frame to wait for. Reading
  // the equivalent (window.innerWidth/innerHeight) from *inside* the iframe
  // was tried first and is NOT reliable at this point - the iframe can
  // still report a stale size from before its own layout has caught up
  // with the just-applied aspect class, which left the game's canvas sized
  // for the wrong dimensions.
  const buildFrameSrc = (src) => {
    const lang = window.__i18n?.getLang() ?? "en";
    const rect = overlayFrameEl?.getBoundingClientRect();
    const size = rect ? `&w=${Math.round(rect.width)}&h=${Math.round(rect.height)}` : "";
    // Read by the shmup shell only (see shell-shmup.html), to decide whether
    // to show its "use the arrow keys" onboarding hint - showing it to a
    // touch player would be actively wrong advice.
    const device = isMobile() ? "mobile" : "desktop";
    return `${src}?lang=${lang}${size}&device=${device}`;
  };

  const openOverlay = (trigger) => {
    activeSrc = trigger?.dataset.gameSrc || DEFAULT_GAME_SRC;
    overlayFrameEl?.classList.toggle("is-portrait", trigger?.dataset.gameAspect === "portrait");

    frame.src = buildFrameSrc(activeSrc);
    overlay.classList.add("is-open");
    overlay.setAttribute("aria-hidden", "false");
    document.body.style.overflow = "hidden";
  };

  const closeOverlay = () => {
    overlay.classList.remove("is-open");
    overlay.setAttribute("aria-hidden", "true");
    frame.src = "";
    document.body.style.overflow = "";

    resultPopup.classList.remove("is-open");
    resultPopup.setAttribute("aria-hidden", "true");
  };

  const showResult = (won, score) => {
    const t = window.__i18n?.t ?? ((key) => key);
    resultTitle.textContent = won ? t("home.featured.resultWin") : t("home.featured.resultLoss");
    resultTitle.className = won ? "is-win" : "is-loss";
    resultScore.textContent = `${t("home.featured.resultScore")} ${score}`;
    resultPopup.classList.add("is-open");
    resultPopup.setAttribute("aria-hidden", "false");
  };

  triggers.forEach((trigger) => {
    trigger.addEventListener("click", (e) => {
      e.preventDefault();
      const device = trigger.dataset.device;
      if (device === "desktop" && isMobile()) {
        openNotice(mobileNotice);
        return;
      }
      if (device === "mobile" && !isMobile()) {
        openNotice(desktopNotice);
        return;
      }
      openOverlay(trigger);
    });

    // Cursor-following zoom (only meaningful for the featured screenshot,
    // but harmless to set on any trigger): the zoom origin tracks the
    // pointer instead of always scaling from a fixed center.
    trigger.addEventListener("mousemove", (e) => {
      const rect = trigger.getBoundingClientRect();
      const x = ((e.clientX - rect.left) / rect.width) * 100;
      const y = ((e.clientY - rect.top) / rect.height) * 100;
      trigger.style.setProperty("--zoom-x", `${x}%`);
      trigger.style.setProperty("--zoom-y", `${y}%`);
    });

    trigger.addEventListener("mouseleave", () => {
      trigger.style.setProperty("--zoom-x", "50%");
      trigger.style.setProperty("--zoom-y", "50%");
    });
  });

  if (closeBtn) {
    closeBtn.addEventListener("click", closeOverlay);
  }

  overlay.addEventListener("click", (e) => {
    if (e.target === overlay) closeOverlay();
  });

  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape" && overlay.classList.contains("is-open")) closeOverlay();
  });

  if (retryBtn) {
    retryBtn.addEventListener("click", () => {
      resultPopup.classList.remove("is-open");
      resultPopup.setAttribute("aria-hidden", "true");
      frame.src = buildFrameSrc(activeSrc);
    });
  }

  if (closeResultBtn) {
    closeResultBtn.addEventListener("click", closeOverlay);
  }

  if (mobileNoticeClose) {
    mobileNoticeClose.addEventListener("click", () => closeNotice(mobileNotice));
  }
  if (mobileNotice) {
    mobileNotice.addEventListener("click", (e) => {
      if (e.target === mobileNotice) closeNotice(mobileNotice);
    });
  }

  if (desktopNoticeClose) {
    desktopNoticeClose.addEventListener("click", () => closeNotice(desktopNotice));
  }
  if (desktopNotice) {
    desktopNotice.addEventListener("click", (e) => {
      if (e.target === desktopNotice) closeNotice(desktopNotice);
    });
  }

  document.addEventListener("keydown", (e) => {
    if (e.key !== "Escape") return;
    if (mobileNotice?.classList.contains("is-open")) closeNotice(mobileNotice);
    if (desktopNotice?.classList.contains("is-open")) closeNotice(desktopNotice);
  });

  window.addEventListener("message", (event) => {
    const data = event.data;
    if (!data || typeof data !== "object") return;

    if (data.type === "closeEmbeddedGame") {
      closeOverlay();
    } else if (data.type === "gameOver") {
      showResult(!!data.won, Number(data.score) || 0);
    }
  });
})();
