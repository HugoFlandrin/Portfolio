
if (false) {
(() => {
  const overlay = document.getElementById("page-transition");
  if (!overlay) return;
  const bar = overlay.querySelector(".pt-bar");

  overlay.classList.add("pt-js");

  const PRE_FILL_MS = 150;
  const FILL_MS = 300;
  const HOLD_MS = 100;
  const ARRIVE_SETTLE_MS = 70;

  function hideOverlay() {
    overlay.classList.add("pt-hide");
    overlay.classList.remove("pt-active");
  }

  setTimeout(hideOverlay, ARRIVE_SETTLE_MS);

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
        if (bar) bar.classList.remove("pt-empty");
        setTimeout(() => {
          window.location.href = link.href;
        }, FILL_MS + HOLD_MS);
      }, PRE_FILL_MS);
    }

    if (bar) {

      bar.classList.add("pt-instant");
      bar.classList.add("pt-empty");

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
