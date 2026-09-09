(() => {

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

  const scriptSrc = document.currentScript?.src || "";
  const basePath = scriptSrc.replace(/static\/js\/game-embed\.js.*$/, "");
  const DEFAULT_GAME_SRC = `${basePath}static/game/index.html`;

  // Must match the CSS breakpoint that swaps .featured-media--desktop/mobile
  // and hide-on-mobile/hide-on-desktop (see styles.css/game-embed.css):
  // "(pointer: coarse), (max-width: 640px)". Checking pointer type alone
  // used to disagree with that CSS on a narrow *desktop* window (mouse, but
  // <=640px wide) - the CSS would swap to the mobile-styled trigger while
  // this still classified it as desktop, so clicking it wrongly showed the
  // "come back on your phone" notice instead of opening the game.
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

  const showResult = (won, score, twoPlayer, scoreP1, scoreP2) => {
    const t = window.__i18n?.t ?? ((key) => key);
    resultTitle.textContent = won ? t("home.featured.resultWin") : t("home.featured.resultLoss");
    resultTitle.className = won ? "is-win" : "is-loss";
    let scoreLine = `${t("home.featured.resultScore")} ${score}`;
    if (twoPlayer) {

      const isFrench = (window.__i18n?.getLang?.() ?? "en") === "fr";
      const p1Label = isFrench ? "J1" : "P1";
      const p2Label = isFrench ? "J2" : "P2";
      scoreLine += ` (${p1Label}: ${scoreP1} · ${p2Label}: ${scoreP2})`;
    }
    resultScore.textContent = scoreLine;
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
