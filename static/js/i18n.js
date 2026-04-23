(() => {
  const STORAGE_KEY = "lang";
  const SUPPORTED = ["en", "fr"];

  /** @type {Record<string, Record<string, string>>} */
  const dict = {
    en: {
      "nav.home": "Home",
      "nav.projects": "Projects",
      "nav.contact": "Contact",
      "nav.lang": "FR",

      "home.about.title": "About me",
      "home.about.body":
        "I am a gameplay and engine programmer, specializing in scalable gameplay systems and modular technical architecture. I have experience in C++, C#, Unreal Engine, Unity, and custom engine development, gained through university and personal projects. Alongside my university studies, I work on freelance projects to apply my skills to concrete and rewarding projects in the fields of web and video games.",

      "home.contact.title": "Contact",
      "home.contact.name": "Name",
      "home.contact.email": "Email",
      "home.contact.message": "Message",
      "home.contact.send": "Send",
      "home.contact.sent": "Your message has been sent!",
      "home.contact.fail": "Failed to send message:",

      "project.challenges": "Challenges",
      "project.whatBuilt": "What I built",
      "project.technologies": "Technologies used",
      "project.links": "Links",
      "project.demoVideo": "Demo video",
      "project.github": "GitHub",
      "project.itch": "Itch.io",
    },
    fr: {
      "nav.home": "Accueil",
      "nav.projects": "Projets",
      "nav.contact": "Contact",
      "nav.lang": "EN",

      "home.about.title": "À propos",
      "home.about.body":
        "Je suis programmeur spécialisé dans le gameplay et les moteurs de jeu, plus particulièrement dans les systèmes de gameplay évolutifs et les architectures techniques modulaires. J'ai de l'expérience en C++, C#, Unreal Engine, Unity et dans le développement de moteurs personnalisés, acquise dans le cadre de projets universitaires et personnels. En parallèle de mes études, je travaille sur des missions en freelance afin de mettre mes compétences au service de projets concrets et enrichissants dans les domaines du web et des jeux vidéo.",

      "home.contact.title": "Contact",
      "home.contact.name": "Nom",
      "home.contact.email": "Email",
      "home.contact.message": "Message",
      "home.contact.send": "Envoyer",
      "home.contact.sent": "Ton message a bien été envoyé !",
      "home.contact.fail": "Échec de l’envoi :",

      "project.challenges": "Défis",
      "project.whatBuilt": "Ce que j’ai fait",
      "project.technologies": "Technologies utilisées",
      "project.links": "Liens",
      "project.demoVideo": "Vidéo de démo",
      "project.github": "GitHub",
      "project.itch": "Itch.io",
    },
  };

  function getLang() {
    const saved = localStorage.getItem(STORAGE_KEY);
    if (saved && SUPPORTED.includes(saved)) return saved;
    return "en";
  }

  function setLang(lang) {
    const next = SUPPORTED.includes(lang) ? lang : "en";
    localStorage.setItem(STORAGE_KEY, next);
    apply(next);
  }

  function t(key) {
    const lang = getLang();
    return dict[lang]?.[key] ?? dict.en[key] ?? key;
  }

  function apply(lang) {
    document.documentElement.lang = lang;

    document.querySelectorAll("[data-i18n]").forEach((el) => {
      const key = el.getAttribute("data-i18n");
      if (!key) return;
      el.textContent = dict[lang]?.[key] ?? dict.en[key] ?? el.textContent;
    });

    document.querySelectorAll("[data-i18n-html]").forEach((el) => {
      const key = el.getAttribute("data-i18n-html");
      if (!key) return;
      el.innerHTML = dict[lang]?.[key] ?? dict.en[key] ?? el.innerHTML;
    });

    document.querySelectorAll("[data-i18n-placeholder]").forEach((el) => {
      const key = el.getAttribute("data-i18n-placeholder");
      if (!key) return;
      el.setAttribute(
        "placeholder",
        dict[lang]?.[key] ?? dict.en[key] ?? el.getAttribute("placeholder") ?? ""
      );
    });

    const toggle = document.querySelector("[data-lang-toggle]");
    if (toggle) toggle.textContent = dict[lang]["nav.lang"];
  }

  function init() {
    apply(getLang());

    const toggle = document.querySelector("[data-lang-toggle]");
    if (toggle) {
      toggle.addEventListener("click", () => {
        const current = getLang();
        setLang(current === "en" ? "fr" : "en");
      });
    }
  }

  window.__i18n = { getLang, setLang, t, apply };

  if (document.readyState === "loading") {
    document.addEventListener("DOMContentLoaded", init);
  } else {
    init();
  }
})();

