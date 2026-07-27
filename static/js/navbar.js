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
})();
