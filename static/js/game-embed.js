(() => {
  // Any element on the page can open the game (the featured screenshot on
  // the home page, a dedicated "Play" button on the project page, etc.) -
  // they all share this one data attribute instead of a single fixed id.
  const triggers = document.querySelectorAll("[data-game-trigger]");
  const overlay = document.getElementById("game-overlay");
  const frame = document.getElementById("game-frame");
  const closeBtn = document.getElementById("game-overlay-close");

  const resultPopup = document.getElementById("game-result-popup");
  const resultTitle = document.getElementById("game-result-title");
  const resultScore = document.getElementById("game-result-score");
  const retryBtn = document.getElementById("game-result-retry");
  const closeResultBtn = document.getElementById("game-result-close");

  const mobileNotice = document.getElementById("game-mobile-notice");
  const mobileNoticeClose = document.getElementById("game-mobile-notice-close");

  if (!triggers.length || !overlay || !frame) return;

  // Derived from this script's own <script src>, so the relative path to
  // static/game/ resolves correctly whether this runs from index.html
  // (static/js/game-embed.js) or a page under pages/ (../static/js/...).
  const scriptSrc = document.currentScript?.src || "";
  const basePath = scriptSrc.replace(/static\/js\/game-embed\.js.*$/, "");
  const GAME_SRC = `${basePath}static/game/index.html`;

  // Coarse-pointer/narrow-viewport heuristic: the game has no touch controls,
  // so treat anything without a fine pointer (mouse) as "mobile" regardless
  // of screen size (covers tablets too).
  const isMobile = () =>
    window.matchMedia("(pointer: coarse)").matches || !window.matchMedia("(pointer: fine)").matches;

  const openMobileNotice = () => {
    if (!mobileNotice) return;
    mobileNotice.classList.add("is-open");
    mobileNotice.setAttribute("aria-hidden", "false");
  };

  const closeMobileNotice = () => {
    if (!mobileNotice) return;
    mobileNotice.classList.remove("is-open");
    mobileNotice.setAttribute("aria-hidden", "true");
  };

  const openOverlay = () => {
    const lang = window.__i18n?.getLang() ?? "en";
    frame.src = `${GAME_SRC}?lang=${lang}`;
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
      if (isMobile()) {
        openMobileNotice();
        return;
      }
      openOverlay();
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
      const lang = window.__i18n?.getLang() ?? "en";
      frame.src = `${GAME_SRC}?lang=${lang}`;
    });
  }

  if (closeResultBtn) {
    closeResultBtn.addEventListener("click", closeOverlay);
  }

  if (mobileNoticeClose) {
    mobileNoticeClose.addEventListener("click", closeMobileNotice);
  }

  if (mobileNotice) {
    mobileNotice.addEventListener("click", (e) => {
      if (e.target === mobileNotice) closeMobileNotice();
    });
  }

  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape" && mobileNotice?.classList.contains("is-open")) closeMobileNotice();
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
