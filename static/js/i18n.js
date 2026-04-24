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

      "project.escape.subtitle":
        "A psychological thriller made with Unreal Engine 5 (escape-room inspired).",
      "project.escape.description":
        "Escape is a first-person thriller blending exploration, tension, and puzzle-solving. You wake up in a dark room with only a flickering flashlight. To survive, you must investigate, solve escape room-style puzzles, and uncover fragments of the story before time runs out.",
      "project.escape.challengeHighlight":
        "Atmosphere above all: lighting, sound, and pacing needed to support tension while keeping puzzles readable.",
      "project.escape.challenge1":
        "Maintaining clarity in a dark environment (flashlight-driven exploration)",
      "project.escape.challenge2":
        "Designing puzzle progression that reveals story beats",
      "project.escape.challenge3":
        "Time-pressure mechanic that increases tension without explicit timers",
      "project.escape.built1":
        "First-person exploration with flashlight-based navigation",
      "project.escape.built2":
        "Escape room-style puzzles tied to investigation and story",
      "project.escape.built3": "Unsettling ambiance and tension-driven pacing",

      "project.pokemon.subtitle":
        "A C# / WPF app inspired by the Pokemon battle system.",
      "project.pokemon.description":
        "PokemonMonster is a desktop application built with WPF to recreate (in a simplified way) the classic Pokemon battle loop. The project includes authentication, catalogs for Pokemon and abilities, and a battle mode with progression to keep runs engaging.",
      "project.pokemon.challengeHighlight":
        "The focus was building a solid, readable app structure (MVVM) while keeping the battle logic simple, consistent, and fun.",
      "project.pokemon.challenge1":
        "Implementing MVVM while keeping navigation practical",
      "project.pokemon.challenge2":
        "Connecting and initializing the database reliably (SQL Server)",
      "project.pokemon.challenge3":
        "Balancing simple rules with meaningful progression",
      "project.pokemon.built1": "Login / registration flow",
      "project.pokemon.built2":
        "Catalog of the first 151 Pokemon (stats + abilities)",
      "project.pokemon.built3":
        "Abilities catalog (details + compatible Pokemon)",
      "project.pokemon.built4":
        "Battle system with dynamic difficulty after 3 victories",
      "project.pokemon.built5":
        "Top navigation bar to switch between key pages",

      "project.gametalk.subtitle":
        "A video game forum built with Go (API + server) and vanilla web tech.",
      "project.gametalk.description":
        "GameTalk is a forum dedicated to video games. Users can log in, browse categories, create topics, react with likes/dislikes, and discuss via comments. The project is split into two Go apps (API + server) and a classic HTML/CSS/JavaScript front-end.",
      "project.gametalk.challengeHighlight":
        "The goal was a complete end-to-end forum flow: authentication, content creation, and interactions, while keeping it simple to run locally.",
      "project.gametalk.challenge1":
        "Designing clean topic/category navigation",
      "project.gametalk.challenge2":
        "Implementing interactions (likes/dislikes) on posts and comments",
      "project.gametalk.challenge3":
        "Handling cross-origin requests during development",
      "project.gametalk.built1": "Landing page + authentication flow",
      "project.gametalk.built2": "Categories view listing existing topics",
      "project.gametalk.built3": "Create topic flow (left menu)",
      "project.gametalk.built4":
        "Topic page: like/dislike + comments (+ like/dislike comments)",
      "project.gametalk.built5": "Profile/settings access (right menu)",

      "project.nightlife.subtitle":
        "A 3D first-person Unreal Engine 5 game made in 5 days (hackathon).",
      "project.nightlife.description":
        "NightLife was created during a five-day hackathon with the theme nightlife. The game starts inside a nightclub, then sends the player into the city at night, where choices, exploration, and unexpected events drive a comedic and absurd adventure.",
      "project.nightlife.challengeHighlight":
        "Shipping a complete Unreal project in 5 days means ruthless prioritization: readable levels, stable interactions, and fast iteration.",
      "project.nightlife.challenge1":
        "Designing a branching, choice-driven flow under tight time constraints",
      "project.nightlife.challenge2":
        "Maintaining strong atmosphere (night city) while keeping navigation clear",
      "project.nightlife.challenge3":
        "Audio integration with all voices in French to support the comedic tone",
      "project.nightlife.built1":
        "First-person exploration and interaction moments (club to city)",
      "project.nightlife.built2":
        "Choice-driven situations (talk, drink, bouncer conflict, etc.)",
      "project.nightlife.built3":
        "Easter eggs and inside jokes for an unpredictable run",

      "project.engine.subtitle":
        "A C++ 2D game engine (SFML + Box2D) with a platformer demo.",
      "project.engine.description":
        "This project is a custom 2D engine built in C++ using SFML for rendering/input and Box2D for physics. A platformer demo validates the engine systems through a concrete gameplay scenario (movement, collisions, enemies, levels, UI).",
      "project.engine.challengeHighlight":
        "Building a modular engine means defining clear system boundaries while keeping the demo game fun and stable.",
      "project.engine.challenge1":
        "Integrating Box2D physics cleanly with gameplay logic",
      "project.engine.challenge2":
        "Scene/level management and reliable transitions",
      "project.engine.challenge3":
        "Maintaining a predictable game loop and update order",
      "project.engine.built1":
        "Core engine systems: game loop, scenes/levels, rendering pipeline",
      "project.engine.built2":
        "Entity/gameplay systems: player controller, collisions, basic enemies, health and game state",
      "project.engine.built3":
        "Level management: two playable levels with structured platformer layout",
      "project.engine.built4":
        "UI systems: main menu, game over, score and timer",

      "project.platypus.subtitle":
        "A dreamlike 3D platformer made in Unity during a 48h Game Jam.",
      "project.platypus.description":
        "Platypus of Corner is a short 3D platformer created in 48 hours with the theme Onirique (dreamlike). You play as a mysterious creature trapped in a surreal floating world, racing against time to recover 3 dream fragments.",
      "project.platypus.challengeHighlight":
        "Building a complete platformer experience in 48 hours required fast iteration, clean scope, and strong readability.",
      "project.platypus.challenge1":
        "Designing levels with distinct mechanics/puzzles under strict time",
      "project.platypus.challenge2":
        "Making exploration readable while keeping the dreamlike vibe",
      "project.platypus.challenge3":
        "Polishing core interactions and game feel quickly",
      "project.platypus.built1":
        "Time-limited exploration (3 minutes) with 3 collectible fragments",
      "project.platypus.built2":
        "3D platforming plus small puzzles and interactions (E key)",
      "project.platypus.built3":
        "Player controls: movement (ZQSD) plus camera (arrow keys)",
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

      "project.escape.subtitle":
        "Un thriller psychologique réalisé avec Unreal Engine 5 (inspiré des escape games).",
      "project.escape.description":
        "Escape est un thriller à la première personne qui mêle exploration, tension et résolution d'énigmes. Vous vous réveillez dans une pièce sombre avec pour seule aide une lampe torche vacillante. Pour survivre, vous devez enquêter, résoudre des puzzles façon escape game et reconstituer des fragments d'histoire avant la fin du temps.",
      "project.escape.challengeHighlight":
        "L'atmosphère avant tout : l'éclairage, le son et le rythme devaient soutenir la tension tout en gardant les énigmes lisibles.",
      "project.escape.challenge1":
        "Garder de la lisibilité dans un environnement sombre (exploration à la lampe torche)",
      "project.escape.challenge2":
        "Concevoir une progression d'énigmes qui révèle le récit",
      "project.escape.challenge3":
        "Mettre en place une pression temporelle sans minuteur explicite",
      "project.escape.built1":
        "Exploration à la première personne avec navigation à la lampe torche",
      "project.escape.built2":
        "Puzzles type escape game liés à l'enquête et au récit",
      "project.escape.built3":
        "Ambiance oppressante et rythme axé sur la tension",

      "project.pokemon.subtitle":
        "Une application C# / WPF inspirée du système de combat de Pokemon.",
      "project.pokemon.description":
        "PokemonMonster est une application desktop développée avec WPF pour recréer (de manière simplifiée) la boucle de combat classique de Pokemon. Le projet inclut l'authentification, des catalogues de Pokemon et de capacités, ainsi qu'un mode combat avec progression pour garder des parties intéressantes.",
      "project.pokemon.challengeHighlight":
        "L'objectif principal était de construire une structure applicative solide et lisible (MVVM), tout en conservant une logique de combat simple, cohérente et amusante.",
      "project.pokemon.challenge1":
        "Implémenter MVVM tout en gardant une navigation pratique",
      "project.pokemon.challenge2":
        "Connecter et initialiser la base de données de manière fiable (SQL Server)",
      "project.pokemon.challenge3":
        "Équilibrer des règles simples avec une progression intéressante",
      "project.pokemon.built1": "Parcours de connexion / inscription",
      "project.pokemon.built2":
        "Catalogue des 151 premiers Pokemon (stats + capacités)",
      "project.pokemon.built3":
        "Catalogue des capacités (détails + Pokemon compatibles)",
      "project.pokemon.built4":
        "Système de combat avec difficulté dynamique après 3 victoires",
      "project.pokemon.built5":
        "Barre de navigation haute pour passer entre les pages clés",

      "project.gametalk.subtitle":
        "Un forum de jeux vidéo construit avec Go (API + serveur) et des technos web natives.",
      "project.gametalk.description":
        "GameTalk est un forum dédié aux jeux vidéo. Les utilisateurs peuvent se connecter, parcourir les catégories, créer des sujets, réagir avec des likes/dislikes et échanger via les commentaires. Le projet est séparé en deux applications Go (API + serveur) et un front-end HTML/CSS/JavaScript classique.",
      "project.gametalk.challengeHighlight":
        "Le but était d'obtenir un flux forum complet de bout en bout : authentification, création de contenu et interactions, tout en restant simple à lancer en local.",
      "project.gametalk.challenge1":
        "Concevoir une navigation claire entre sujets et catégories",
      "project.gametalk.challenge2":
        "Implémenter les interactions (likes/dislikes) sur les posts et les commentaires",
      "project.gametalk.challenge3":
        "Gérer les requêtes cross-origin pendant le développement",
      "project.gametalk.built1":
        "Page d'accueil + parcours d'authentification",
      "project.gametalk.built2":
        "Vue catégories listant les sujets existants",
      "project.gametalk.built3":
        "Parcours de création de sujet (menu gauche)",
      "project.gametalk.built4":
        "Page sujet : like/dislike + commentaires (+ like/dislike des commentaires)",
      "project.gametalk.built5":
        "Accès profil/paramètres (menu droit)",

      "project.nightlife.subtitle":
        "Un jeu 3D à la première personne sous Unreal Engine 5 réalisé en 5 jours (hackathon).",
      "project.nightlife.description":
        "NightLife a été créé pendant un hackathon de cinq jours sur le thème nightlife. Le jeu commence dans une boîte de nuit, puis envoie le joueur dans la ville de nuit, où les choix, l'exploration et des événements inattendus créent une aventure comique et absurde.",
      "project.nightlife.challengeHighlight":
        "Livrer un projet Unreal complet en 5 jours exige une priorisation drastique : niveaux lisibles, interactions stables et itérations rapides.",
      "project.nightlife.challenge1":
        "Concevoir un déroulé à embranchements centré sur les choix avec un temps limité",
      "project.nightlife.challenge2":
        "Conserver une forte ambiance (ville nocturne) tout en gardant une navigation claire",
      "project.nightlife.challenge3":
        "Intégrer l'audio avec toutes les voix en français pour renforcer le ton comique",
      "project.nightlife.built1":
        "Exploration à la première personne et moments d'interaction (boîte vers ville)",
      "project.nightlife.built2":
        "Situations basées sur les choix (parler, boire, conflit avec un videur, etc.)",
      "project.nightlife.built3":
        "Easter eggs et blagues internes pour une partie imprévisible",

      "project.engine.subtitle":
        "Un moteur de jeu 2D en C++ (SFML + Box2D) avec une démo de platformer.",
      "project.engine.description":
        "Ce projet est un moteur 2D personnalisé développé en C++ avec SFML pour le rendu/les entrées et Box2D pour la physique. Une démo de platformer valide les systèmes du moteur via un scénario de gameplay concret (déplacements, collisions, ennemis, niveaux, interface).",
      "project.engine.challengeHighlight":
        "Construire un moteur modulaire implique de définir des frontières de systèmes claires tout en gardant la démo fun et stable.",
      "project.engine.challenge1":
        "Intégrer la physique Box2D proprement avec la logique gameplay",
      "project.engine.challenge2":
        "Gérer les scènes/niveaux et des transitions fiables",
      "project.engine.challenge3":
        "Maintenir une boucle de jeu et un ordre d'update prévisibles",
      "project.engine.built1":
        "Systèmes coeur du moteur : boucle de jeu, scènes/niveaux, pipeline de rendu",
      "project.engine.built2":
        "Systèmes entités/gameplay : contrôleur joueur, collisions, ennemis basiques, santé et état de jeu",
      "project.engine.built3":
        "Gestion de niveaux : deux niveaux jouables avec un layout de platformer structuré",
      "project.engine.built4":
        "Systèmes UI : menu principal, game over, score et timer",

      "project.platypus.subtitle":
        "Un platformer 3D onirique réalisé sur Unity pendant une Game Jam de 48h.",
      "project.platypus.description":
        "Platypus of Corner est un court platformer 3D créé en 48 heures sur le thème Onirique. Vous incarnez une créature mystérieuse piégée dans un monde flottant surréaliste, qui doit récupérer 3 fragments de rêve avant la fin du temps.",
      "project.platypus.challengeHighlight":
        "Construire une expérience de platformer complète en 48 heures a demandé des itérations rapides, un scope propre et une forte lisibilité.",
      "project.platypus.challenge1":
        "Concevoir des niveaux avec des mécaniques/énigmes distinctes sous forte contrainte de temps",
      "project.platypus.challenge2":
        "Rendre l'exploration lisible tout en conservant l'ambiance onirique",
      "project.platypus.challenge3":
        "Polir rapidement les interactions clés et le game feel",
      "project.platypus.built1":
        "Exploration limitée dans le temps (3 minutes) avec 3 fragments à collecter",
      "project.platypus.built2":
        "Platforming 3D avec petites énigmes et interactions (touche E)",
      "project.platypus.built3":
        "Contrôles joueur : déplacement (ZQSD) et caméra (flèches)",
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

