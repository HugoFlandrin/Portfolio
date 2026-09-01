/* =========================
   PAGE TRANSITION — fully disabled.
   Wrapped in `if (false)` so none of this ever executes: no timers, no
   click interception, no listeners, nothing that can touch navigation
   timing. Delete the `if (false) {` line below and its matching closing
   `}` (right after the IIFE's final `})();`) to bring it back.
========================= */
if (false) {
(() => {
  const overlay = document.getElementById("page-transition");
  if (!overlay) return;
  const bar = overlay.querySelector(".pt-bar");

  // Signal that JS is in control — CSS's failsafe fade (in case this
  // script never ran) hands off to the precise transition-based timing.
  overlay.classList.add("pt-js");

  // Total budget stays comfortably under 1s even on the click-through path:
  // empty bar (visible) -> fill -> hold full -> navigate -> quick reveal.
  const PRE_FILL_MS = 150; // time the empty bar stays visible before filling
  const FILL_MS = 300;     // matches the CSS transition duration on .pt-bar
  const HOLD_MS = 100;     // time the full bar stays visible before navigating
  const ARRIVE_SETTLE_MS = 70; // brief pause on arrival before revealing the page

  function hideOverlay() {
    overlay.classList.add("pt-hide");
    overlay.classList.remove("pt-active");
  }

  // Every load this script sees (fresh visit, reload, or arrival after an
  // intercepted click) starts from "already full" (the bar's CSS resting
  // state — no class needed, so there's no first-paint race) and just
  // reveals the page. The fill animation itself only ever plays once,
  // on the outgoing page.
  setTimeout(hideOverlay, ARRIVE_SETTLE_MS);

  // Guard against the browser's back/forward cache restoring this page
  // frozen mid-transition (overlay stuck visible and blocking clicks).
  window.addEventListener("pageshow", (e) => {
    if (e.persisted) hideOverlay();
  });

  document.addEventListener("click", (e) => {
    if (overlay.classList.contains("pt-active")) return;

    const link = e.target.closest("a[href]");
    if (!link) return;
    if (link.target && link.target !== "_self") return;
    if (e.button !== 0 || e.metaKey || e.ctrlKey || e.shiftKey || e.altKey) return;

    let url;
    try {
      url = new URL(link.href, window.location.href);
    } catch {
      return;
    }
    if (url.origin !== window.location.origin) return;

    const samePageAnchor = url.pathname === window.location.pathname && url.hash !== "";
    if (samePageAnchor) return;

    e.preventDefault();

    overlay.classList.remove("pt-hide");
    overlay.classList.add("pt-active");

    function fillThenNavigate() {
      setTimeout(() => {
        if (bar) bar.classList.remove("pt-empty"); // the one real fill animation
        setTimeout(() => {
          window.location.href = link.href;
        }, FILL_MS + HOLD_MS);
      }, PRE_FILL_MS);
    }

    if (bar) {
      // Snap to empty with transitions disabled first, so this reset is
      // instant and never itself plays as a visible (un)fill animation —
      // without this, removing/re-adding the fill state mid-transition is
      // what caused the bar to appear to fill twice.
      bar.classList.add("pt-instant");
      bar.classList.add("pt-empty");
      // A forced reflow (offsetWidth) guarantees layout is recalculated,
      // but not that the browser actually PAINTS that instant "empty"
      // frame before the next line runs. Without waiting for a real paint,
      // re-enabling the transition here animates from the last frame that
      // *was* painted (still "full") down to empty — the bar visibly
      // empties before filling. Two animation frames guarantee a real
      // paint of the instant reset happens first.
      requestAnimationFrame(() => {
        requestAnimationFrame(() => {
          bar.classList.remove("pt-instant");
          fillThenNavigate();
        });
      });
    } else {
      fillThenNavigate();
    }
  });
})();
}

/* =========================
   ACHIEVEMENTS — temporarily disabled.
   Wrapped in `if (false)` so none of this runs (no localStorage reads/
   writes, no toasts). Delete the `if (false) {` line below and its
   matching closing `}` (right after the IIFE's final `})();`) to restore.
   Purely local (localStorage) — nothing tracked or sent anywhere.
========================= */
if (false) {
(() => {
  const ACHIEVEMENTS = {
    explorer: { icon: "🧭", key: "ach.explorer" },
    cartographer: { icon: "🗺️", key: "ach.cartographer" },
    curious: { icon: "🔍", key: "ach.curious" },
    cinephile: { icon: "🎬", key: "ach.cinephile" },
    completionist: { icon: "🏆", key: "ach.completionist" },
    clientReady: { icon: "💼", key: "ach.clientReady" },
  };

  const ALL_PROJECT_SLUGS = [
    "2d-game-engine", "3d-game-engine", "escape", "nightlife", "lifeawake",
    "platypus-of-corner", "gametalk", "ue-minecraft-prototype",
    "ue-stamina-system", "ue-board-game-prototype", "pokemon-monster",
  ];

  function getSet(key) {
    try {
      return new Set(JSON.parse(localStorage.getItem(key) || "[]"));
    } catch {
      return new Set();
    }
  }

  function saveSet(key, set) {
    try {
      localStorage.setItem(key, JSON.stringify([...set]));
    } catch {
      /* private browsing / storage full — skip silently */
    }
  }

  const unlocked = getSet("lw_achievements");

  function unlock(id) {
    if (unlocked.has(id) || !ACHIEVEMENTS[id]) return;
    unlocked.add(id);
    saveSet("lw_achievements", unlocked);
    showToast(ACHIEVEMENTS[id]);
  }

  function showToast(a) {
    const t = window.__i18n ? window.__i18n.t : (k) => k;
    const toast = document.createElement("div");
    toast.className = "ach-toast";
    toast.innerHTML =
      '<span class="ach-icon">' + a.icon + "</span>" +
      '<span class="ach-text"><strong>' + t("ach.unlocked") + "</strong><span>" +
      t(a.key) + "</span></span>";
    document.body.appendChild(toast);
    requestAnimationFrame(() => toast.classList.add("ach-toast--show"));
    setTimeout(() => {
      toast.classList.remove("ach-toast--show");
      setTimeout(() => toast.remove(), 400);
    }, 4200);
  }

  const slug = window.location.pathname.split("/").pop().replace(".html", "");

  if (slug === "all-work") unlock("cartographer");
  if (slug === "LevelWeb-Studio") unlock("clientReady");

  if (ALL_PROJECT_SLUGS.includes(slug)) {
    const visited = getSet("lw_visited_projects");
    visited.add(slug);
    saveSet("lw_visited_projects", visited);
    if (visited.size >= 3) unlock("explorer");
    if (visited.size >= ALL_PROJECT_SLUGS.length) unlock("completionist");
  }

  document.addEventListener("click", (e) => {
    if (e.target.closest(".media-item img")) unlock("curious");
  });

  document.querySelectorAll("video").forEach((v) => {
    v.addEventListener("ended", () => unlock("cinephile"));
  });
})();
}
