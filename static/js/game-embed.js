(() => {

  const triggers = document.querySelectorAll("[data-game-trigger]");
  const overlay = document.getElementById("game-overlay");
  const overlayFrameEl = document.querySelector(".game-overlay-frame");
  const frame = document.getElementById("game-frame");
  const closeBtn = document.getElementById("game-overlay-close");

  const resultPopup = document.getElementById("game-result-popup");
  const resultCard = resultPopup?.querySelector(".game-result-card");
  const resultTitle = document.getElementById("game-result-title");
  const resultScore = document.getElementById("game-result-score");
  const retryBtn = document.getElementById("game-result-retry");
  const closeResultBtn = document.getElementById("game-result-close");
  // Only present on pages with a "demo" entry point (home page, engine
  // page) - see showResult()'s own comment on what these are for. A single
  // button (solo and duo used to be two separate ones) since both just lead
  // to the same Space Shooter project page either way.
  const discoverBtn = document.getElementById("game-result-discover");

  const mobileNotice = document.getElementById("game-mobile-notice");
  const mobileNoticeClose = document.getElementById("game-mobile-notice-close");

  const desktopNotice = document.getElementById("game-desktop-notice");
  const desktopNoticeClose = document.getElementById("game-desktop-notice-close");

  if (!triggers.length || !overlay || !frame) return;

  const scriptSrc = document.currentScript?.src || "";
  const basePath = scriptSrc.replace(/static\/js\/game-embed\.js.*$/, "");
  const DEFAULT_GAME_SRC = `${basePath}static/platformer/index.html`;

  // Must match the CSS breakpoint that swaps hide-on-mobile/hide-on-desktop
  // (see game-embed.css): "(pointer: coarse), (max-width: 640px)". Checking
  // pointer type alone used to disagree with that CSS on a narrow *desktop*
  // window (mouse, but <=640px wide) - the CSS would swap to the
  // mobile-styled trigger while this still classified it as desktop, so
  // clicking it wrongly showed the "come back on your phone" notice instead
  // of opening the game.
  const isMobile = () =>
    window.matchMedia("(pointer: coarse)").matches ||
    !window.matchMedia("(pointer: fine)").matches ||
    window.matchMedia("(max-width: 640px)").matches;

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

  const focusGameFrame = () => {
    frame.focus();
    try {
      frame.contentWindow?.focus();
      frame.contentDocument?.getElementById("canvas")?.focus();
    } catch {

    }
  };

  frame.addEventListener("load", focusGameFrame);

  let activeSrc = DEFAULT_GAME_SRC;
  // Set from the trigger's own data-game-demo (see openOverlay()) - marks
  // this run as one of the "try it right here" entry points (home page,
  // engine page), which always play the same solo/90s run. showResult()
  // reads this to swap in a demo-flavored end screen instead of the normal
  // win/loss one - see its own comment for why.
  let isDemoContext = false;

  const buildFrameSrc = (src) => {
    const lang = window.__i18n?.getLang() ?? "en";
    // Measure the IFRAME itself, not .game-overlay-frame: the frame has its
    // own 1px border (see game-embed.css), which offsetWidth/Height on the
    // outer element would count as part of the size - 2px the iframe (and
    // the canvas sized to match it below) never actually has to draw into,
    // leaving a sliver of the frame's own black background showing around
    // the game. The iframe has no border/padding of its own, so its
    // offsetWidth/Height is exactly the space available to the canvas.
    //
    // offsetWidth/Height (not getBoundingClientRect) deliberately: the frame
    // opens with a scale(0.97) -> scale(1) transition (see .game-overlay-frame
    // / #game-overlay.is-open in game-embed.css), and getBoundingClientRect
    // reports the *transformed* box - measuring mid-transition sized the
    // canvas a few percent smaller than the frame's settled size, leaving a
    // black gap (the frame's own background) around the game on open.
    // offsetWidth/Height reflect the untransformed layout box, so they're
    // correct regardless of where the open animation currently is.
    const size = frame
      ? `&w=${Math.round(frame.offsetWidth)}&h=${Math.round(frame.offsetHeight)}`
      : "";

    const device = isMobile() ? "mobile" : "desktop";
    // A trigger's own data-game-src can already carry a query string (e.g.
    // "?mode=infinite" to pick a specific Shmup run) - append with "&" in
    // that case instead of a second "?", which would otherwise get treated
    // as a literal character inside the "mode" value rather than starting
    // a new param, silently swallowing lang/w/h/device.
    const separator = src.includes("?") ? "&" : "?";
    return `${src}${separator}lang=${lang}${size}&device=${device}`;
  };

  const openOverlay = (trigger) => {
    activeSrc = trigger?.dataset.gameSrc || DEFAULT_GAME_SRC;
    isDemoContext = trigger?.dataset.gameDemo === "true";
    overlayFrameEl?.classList.toggle("is-portrait", trigger?.dataset.gameAspect === "portrait");

    // The settings gear only means anything for Space Shooter (the only
    // build wired to WebBridge's ShmupSet* audio exports) - pages that can
    // also open the platformer from the same overlay (2d-game-engine.html)
    // would otherwise show a gear that silently does nothing once that
    // other game is loaded instead.
    const settingsToggle = document.getElementById("game-settings-toggle");
    if (settingsToggle) {
      settingsToggle.hidden = !activeSrc.includes("space-shooter");
    }

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

    // Only present on pages with a settings panel (Space Shooter) - reset
    // here too so it doesn't linger visually "open" the next time this
    // overlay opens on a fresh game instance, which always boots unpaused.
    const settingsPanel = document.getElementById("game-settings-panel");
    const settingsBackdrop = document.getElementById("game-settings-backdrop");
    settingsPanel?.classList.remove("is-open");
    settingsPanel?.setAttribute("aria-hidden", "true");
    settingsBackdrop?.classList.remove("is-active");
  };

  const showResult = (won, score, twoPlayer, scoreP1, scoreP2) => {
    const t = window.__i18n?.t ?? ((key) => key);

    // Demo entry points (home page, engine page) always run the same
    // solo/90s Score Attack, so "Victory!"/"Defeat" + Retry doesn't fit -
    // there's nothing to "win" or "lose" in a fixed showcase run. Swap in a
    // dedicated end screen instead: no verdict, no retry, just a thank-you
    // and a nudge toward the two real modes on the project page.
    if (isDemoContext) {
      resultTitle.textContent = t("home.featured.demoEndTitle");
      resultTitle.className = "";
      resultCard?.classList.add("is-demo");
      // A <br> (not a space) between the thank-you line and the score, plus
      // its own span so CSS can size it to fit on one line - see
      // game-embed.css's .is-demo rules.
      resultScore.innerHTML = `<span class="game-result-demo-line">${t("home.featured.demoEndBody")}</span><br>${t("home.featured.resultScore")} ${score}`;
      if (retryBtn) retryBtn.hidden = true;
      if (discoverBtn) discoverBtn.hidden = false;
      resultPopup.classList.add("is-open");
      resultPopup.setAttribute("aria-hidden", "false");
      return;
    }

    resultTitle.textContent = won ? t("home.featured.resultWin") : t("home.featured.resultLoss");
    resultTitle.className = won ? "is-win" : "is-loss";
    resultCard?.classList.remove("is-demo");
    const scoreLine = `${t("home.featured.resultScore")} ${score}`;
    if (twoPlayer) {
      const isFrench = (window.__i18n?.getLang?.() ?? "en") === "fr";
      const p1Label = isFrench ? "J1" : "P1";
      const p2Label = isFrench ? "J2" : "P2";
      // A <br> (not inline parentheses) between the global score and the
      // per-player breakdown - see the demo line above for the same reasoning.
      resultScore.innerHTML = `${scoreLine}<br>${p1Label}: ${scoreP1} · ${p2Label}: ${scoreP2}`;
    } else {
      resultScore.textContent = scoreLine;
    }
    if (retryBtn) retryBtn.hidden = false;
    if (discoverBtn) discoverBtn.hidden = true;
    resultPopup.classList.add("is-open");
    resultPopup.setAttribute("aria-hidden", "false");
  };

  triggers.forEach((trigger) => {
    trigger.addEventListener("click", (e) => {
      const device = trigger.dataset.device;
      if (device === "desktop" && isMobile()) {
        e.preventDefault();
        openNotice(mobileNotice);
        return;
      }
      if (device === "mobile" && !isMobile()) {
        e.preventDefault();
        openNotice(desktopNotice);
        return;
      }
      // A trigger can itself sit inside a notice (e.g. the mobile notice's
      // own "play solo instead" fallback button - see space-shooter.html) -
      // close it so it doesn't linger on top of whatever it just opened.
      closeNotice(mobileNotice);
      closeNotice(desktopNotice);

      // A separately-hosted game (see platypus-of-corner.html - its build
      // got heavy enough to warrant its own repo/Pages site instead of
      // living in this one) opens in its own tab via the trigger's real
      // href/target instead of the embedded overlay. No preventDefault here
      // and no synthetic window.open(): letting the browser's native anchor
      // navigation run is what makes ctrl/cmd-click, middle-click, and
      // right-click "open in new tab" all keep working for free.
      if (trigger.dataset.externalUrl) {
        return;
      }

      e.preventDefault();
      openOverlay(trigger);
    });

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
      showResult(!!data.won, Number(data.score) || 0, !!data.twoPlayer, Number(data.scoreP1) || 0, Number(data.scoreP2) || 0);
    }
  });
})();
