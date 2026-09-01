(() => {
  const t = window.__i18n ? window.__i18n.t : (k) => k;

  const companion = document.createElement("div");
  companion.id = "companion";
  companion.setAttribute("aria-hidden", "true");
  companion.innerHTML =
    '<div class="comp-bubble" id="comp-bubble"></div>' +
    '<div class="comp-sprite">' +
    '<svg viewBox="0 0 80 90" xmlns="http://www.w3.org/2000/svg">' +
    "<defs><linearGradient id=\"comp-grad\" gradientUnits=\"userSpaceOnUse\" x1=\"0\" y1=\"0\" x2=\"80\" y2=\"90\">" +
    '<stop offset="0" stop-color="#4da3ff"/><stop offset="1" stop-color="#9966ff"/>' +
    "</linearGradient></defs>" +
    // Two small antennae (uneven heights, for a bit of personality) topped
    // with little sensor balls — a scout that's always tuned in.
    '<rect x="24" y="10" width="7" height="14" rx="3.5" fill="url(#comp-grad)"/>' +
    '<circle cx="27.5" cy="10" r="4.5" fill="url(#comp-grad)"/>' +
    '<rect x="49" y="6" width="7" height="16" rx="3.5" fill="url(#comp-grad)"/>' +
    '<circle cx="52.5" cy="6" r="5" fill="url(#comp-grad)"/>' +
    // Rounded head.
    '<rect x="8" y="18" width="64" height="60" rx="26" fill="url(#comp-grad)"/>' +
    // Bar-shaped eyes — the same pill motif as the logo and loading bar.
    '<rect class="comp-eye comp-eye-l" x="24" y="42" width="10" height="16" rx="5"/>' +
    '<rect class="comp-eye comp-eye-r" x="46" y="42" width="10" height="16" rx="5"/>' +
    '<circle cx="18" cy="58" r="5" fill="#ff8a65" opacity="0.35"/>' +
    '<circle cx="62" cy="58" r="5" fill="#ff8a65" opacity="0.35"/>' +
    // Resting smile, hidden while talking; a small open-mouth oval takes
    // over in its place to sell the "talking" animation (see .comp-talking).
    '<path class="comp-mouth-closed" d="M30 66 Q40 72 50 66" stroke="#0b0f1f" stroke-width="3" stroke-linecap="round" fill="none"/>' +
    '<ellipse class="comp-mouth-open" cx="40" cy="68" rx="7" ry="5" fill="#0b0f1f"/>' +
    "</svg></div>";
  document.body.appendChild(companion);

  const bubble = companion.querySelector("#comp-bubble");
  const sprite = companion.querySelector(".comp-sprite");
  let hideTimer = null;
  let lastKey = null;
  let lastSide = null;

  function setPosition(left, top) {
    companion.style.left = left + "px";
    companion.style.top = top + "px";
  }

  // Idle corner spot — set once, synchronously, before first paint, so it
  // never itself plays as a "glide in from nowhere" animation.
  setPosition(24, window.innerHeight - sprite.offsetHeight - 36);

  // Picks where, relative to the target, the companion should stand —
  // left/right are always available; above/below only when there's
  // enough room, and it won't repeat the same side twice in a row.
  function pickSide(rect) {
    const candidates = ["left", "right"];
    if (rect.top > 160) candidates.push("above");
    if (window.innerHeight - rect.bottom > 160) candidates.push("below");

    let side = candidates[Math.floor(Math.random() * candidates.length)];
    if (candidates.length > 1) {
      while (side === lastSide) {
        side = candidates[Math.floor(Math.random() * candidates.length)];
      }
    }
    lastSide = side;
    return side;
  }

  function moveTo(targetEl) {
    if (!targetEl) return;
    const rect = targetEl.getBoundingClientRect();
    const w = sprite.offsetWidth;
    const h = sprite.offsetHeight;
    const margin = 18;
    const side = pickSide(rect);

    let left, top;
    if (side === "left") {
      left = rect.left - w - margin;
      top = rect.top + rect.height / 2 - h / 2;
    } else if (side === "right") {
      left = rect.right + margin;
      top = rect.top + rect.height / 2 - h / 2;
    } else if (side === "above") {
      left = rect.left + rect.width / 2 - w / 2;
      top = rect.top - h - margin;
    } else {
      left = rect.left + rect.width / 2 - w / 2;
      top = rect.bottom + margin;
    }

    // Keep well clear of the edges, and never let it settle in the bottom
    // quarter of the screen — a target near the page's bottom (e.g. the
    // contact form) would otherwise pin it right against the edge.
    left = Math.max(20, Math.min(left, window.innerWidth - w - 20));
    const maxTop = Math.min(window.innerHeight - h - 28, window.innerHeight * 0.72 - h);
    top = Math.max(28, Math.min(top, maxTop));
    setPosition(left, top);
  }

  function say(key, duration = 5000) {
    lastKey = key;
    clearTimeout(hideTimer);
    bubble.textContent = t(key);
    bubble.classList.add("comp-bubble--show");
    companion.classList.add("comp-talking");
    hideTimer = setTimeout(() => {
      bubble.classList.remove("comp-bubble--show");
      companion.classList.remove("comp-talking");
    }, duration);
  }

  let lastMoveTime = 0;
  const MOVE_COOLDOWN_MS = 700; // absorbs scroll jitter right at a section boundary

  function sayNear(key, targetEl) {
    const now = Date.now();
    if (now - lastMoveTime < MOVE_COOLDOWN_MS) return;
    lastMoveTime = now;
    moveTo(targetEl);
    say(key);
  }

  sprite.addEventListener("click", () => {
    if (lastKey) say(lastKey);
  });

  // Re-fires every time the section crosses into view — scrolling back up
  // to a section it already reacted to makes it react again, the same as
  // scrolling down to a new one.
  function watchSection(el, callback) {
    if (!el) return;
    let wasIntersecting = false;
    const io = new IntersectionObserver(
      (entries) => {
        entries.forEach((entry) => {
          if (entry.isIntersecting && !wasIntersecting) {
            wasIntersecting = true;
            callback();
          } else if (!entry.isIntersecting) {
            wasIntersecting = false;
          }
        });
      },
      { threshold: 0.4 }
    );
    io.observe(el);
  }

  const isHome = !!document.getElementById("featured") && !!document.getElementById("projects");
  const isAllWork = !!document.querySelector(".all-work-category");

  if (isHome) {
    setTimeout(() => say("comp.home.greet"), 900);
    watchSection(document.getElementById("featured"), () =>
      sayNear("comp.home.featured", document.querySelector(".featured-card"))
    );
    watchSection(document.getElementById("experience"), () =>
      sayNear("comp.home.experience", document.querySelector(".experience-card"))
    );
    watchSection(document.getElementById("projects"), () =>
      sayNear("comp.home.projects", document.querySelector(".project-card--featured-grid"))
    );
    watchSection(document.getElementById("contact"), () =>
      sayNear("comp.home.contact", document.querySelector(".contact-form"))
    );
  } else if (isAllWork) {
    const CATEGORY_KEYS = {
      "home.allWork.cat.engines": "comp.allwork.engines",
      "home.allWork.cat.unreal": "comp.allwork.unreal",
      "home.allWork.cat.unity": "comp.allwork.unity",
      "home.allWork.cat.web": "comp.allwork.web",
      "home.allWork.cat.prototypes": "comp.allwork.prototypes",
      "home.allWork.cat.applications": "comp.allwork.applications",
    };

    setTimeout(() => say("comp.allwork.greet"), 900);

    document.querySelectorAll(".all-work-category h3[data-i18n]").forEach((h3) => {
      const key = CATEGORY_KEYS[h3.getAttribute("data-i18n")];
      if (!key) return;
      const category = h3.closest(".all-work-category");
      category.addEventListener("mouseenter", () => sayNear(key, h3));
    });
  }
})();
