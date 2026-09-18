(() => {
  const navbar = document.querySelector(".navbar");
  const toggle = document.querySelector("[data-nav-toggle]");
  const menu = document.querySelector(".nav-links");

  const setNavShadow = () => {
    if (!navbar) return;
    navbar.classList.toggle("scrolled", window.scrollY > 8);
  };

  window.addEventListener("scroll", setNavShadow, { passive: true });
  setNavShadow();

  if (!toggle || !menu) return;

  const closeMenu = () => {
    toggle.setAttribute("aria-expanded", "false");
    navbar.classList.remove("nav-open");
    document.body.classList.remove("nav-open");
  };

  toggle.addEventListener("click", () => {
    const isOpen = toggle.getAttribute("aria-expanded") === "true";
    toggle.setAttribute("aria-expanded", isOpen ? "false" : "true");
    navbar.classList.toggle("nav-open", !isOpen);
    document.body.classList.toggle("nav-open", !isOpen);
  });

  menu.querySelectorAll(".nav-link").forEach((link) => {
    link.addEventListener("click", closeMenu);
  });

  document.addEventListener("keydown", (e) => {
    if (e.key === "Escape") closeMenu();
  });

  const sideLinks = document.querySelectorAll(".home-side-link");
  if (!sideLinks.length) return;

  const sectionTargets = Array.from(sideLinks)
    .map((link) => document.querySelector(link.dataset.target))
    .filter(Boolean);

  const setActiveSideLink = () => {
    const offset = window.innerHeight * 0.35;
    let activeId = "#top";

    for (const section of sectionTargets) {
      const sectionTop = section.getBoundingClientRect().top;
      if (sectionTop <= offset) {
        activeId = `#${section.id}`;
      }
    }

    sideLinks.forEach((link) => {
      const isActive = link.dataset.target === activeId;
      link.classList.toggle("is-active", isActive);
      link.setAttribute("aria-current", isActive ? "true" : "false");
    });
  };

  sideLinks.forEach((link) => {
    link.addEventListener("click", () => {
      const target = document.querySelector(link.dataset.target);
      if (!target) return;

      const offset = 88;
      const targetTop = Math.max(target.getBoundingClientRect().top + window.scrollY - offset, 0);
      window.scrollTo({ top: targetTop, behavior: "smooth" });
      setActiveSideLink();
    });
  });

  window.addEventListener("scroll", setActiveSideLink, { passive: true });
  setActiveSideLink();
})();
