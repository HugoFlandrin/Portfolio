(() => {
  const STORAGE_KEY = "lang";
  const SUPPORTED = ["en", "fr"];

  /** @type {Record<string, Record<string, string>>} */
  const dict = {
    en: {
      "nav.home": "Home",
      "nav.experience": "Experience",
      "nav.projects": "Projects",
      "nav.freelance": "LevelWeb-Studio",
      "nav.contact": "Contact",
      "nav.lang": "FR",

      "home.hero.eyebrow": "Gameplay & Engine Programmer",
      "home.hero.lead":
        "I build modular gameplay systems and engine foundations in C++, with hands-on experience on custom engines, Unreal Engine, and Unity.",
      "home.cta.resume": "View Resume",
      "home.cta.github": "GitHub",
      "home.cta.linkedin": "LinkedIn",
      "home.status.label": "Currently looking for",
      "home.status.text":
        "Internship or junior role — Gameplay Programmer, Engine Programmer, or Generalist Programmer (C++ / Unreal / custom engine).",
      "home.featured.title": "Featured project",
      "home.featured.tag": "Engine · C++ · SFML · Box2D",
      "home.featured.summary":
        "Custom C++ engine with modular systems (game loop, scenes, rendering, physics) validated by a full platformer demo.",
      "home.featured.point1": "Deterministic update order & scene transitions",
      "home.featured.point2": "Box2D integrated with gameplay entities",
      "home.featured.point3": "Architecture designed for maintainable engine growth",
      "home.featured.cta": "Explore technical breakdown",
      "home.experience.title": "Professional experience",
      "home.experience.sub": "Real-world exposure alongside academic engine work",
      "home.exp1.type": "Freelance",
      "home.exp1.period": "2024 — Present",
      "home.exp1.role": "Web & Game Development — Freelance",
      "home.exp1.item1": "Front-end and back-end updates on production websites",
      "home.exp1.item2": "Feature delivery, bug fixes, and iterative client feedback loops",
      "home.exp1.item3": "Small-scope game and web prototypes under time constraints",
      "home.exp2.type": "Academic / Personal",
      "home.exp2.period": "2023 — Present",
      "home.exp2.role": "Gameplay & Engine Projects — Lead Programmer",
      "home.exp2.item1": "Designed and implemented a modular 2D game engine (SFML + Box2D)",
      "home.exp2.item2": "Shipped gameplay systems for UE5 and Unity projects (jam & semester scope)",
      "home.exp2.item3": "Owned technical architecture decisions, debugging, and milestone delivery",
      "home.exp3.type": "University",
      "home.exp3.period": "Team projects",
      "home.exp3.role": "Game Development — Programmer",
      "home.exp3.item1": "Collaborated on multidisciplinary teams (design, art, audio)",
      "home.exp3.item2": "Delivered playable builds under hackathon and coursework deadlines",
      "home.exp3.item3": "Built player interactions, pacing systems, and level scripting",
      "home.projects.title": "Projects",
      "home.projects.sub": "Technical case studies — select a project for engineering details",
      "home.card.engine.tag": "Engine",
      "home.card.engine.summary": "C++ · SFML · Box2D · modular architecture",
      "home.card.escape.tag": "Gameplay · UE5",
      "home.card.escape.summary": "First-person thriller · puzzles · atmosphere",
      "home.card.nightlife.tag": "Gameplay · UE5",
      "home.card.nightlife.summary": "5-day hackathon · branching choices",
      "home.card.platypus.tag": "Gameplay · Unity",
      "home.card.platypus.summary": "48h jam · 3D platformer",
      "home.card.gametalk.tag": "Full-stack · Go",
      "home.card.gametalk.summary": "API + server · forum systems",
      "home.card.pokemon.tag": "Application · C#",
      "home.card.pokemon.summary": "WPF · MVVM · SQL Server",
      "home.contact.sub": "Recruiters and collaborators — I reply within a few days",
      "home.footer.about":
        "This portfolio highlights engineering decisions and system design — built as a lightweight static site (HTML/CSS/JS).",
      "home.footer.version": "Portfolio v.0.3",
      "home.footer.updated": "Updated May 2026",

      "home.about.title": "About",
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
      "project.tech.title": "Technical focus",
      "project.decisions.title": "Key decisions",
      "project.workflow.title": "Workflow",
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
      "project.escape.tech.note":
        "Gameplay systems prioritize readable interaction in low light: flashlight-driven exploration, inspectable objects, and puzzle gates tied to narrative beats.",
      "project.escape.decision1":
        "Implicit time pressure via audio/visual cues instead of on-screen timers",
      "project.escape.decision2": "Modular puzzle actors with shared interaction interface",
      "project.escape.decision3": "Lighting volumes tuned per room for consistent readability",
      "project.escape.workflow":
        "Blockout → gameplay pass → lighting/audio polish → playtest for puzzle clarity and pacing.",
      "project.escape.caption1":
        "Low-light exploration — flashlight as primary navigation tool",
      "project.escape.caption2":
        "Puzzle space designed for investigation and environmental storytelling",
      "project.escape.captionVideo": "April demo — interaction loop and atmosphere pacing",

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
      "project.pokemon.tech.note":
        "MVVM separation keeps views thin; battle logic and data access live in testable services with SQL-backed catalogs.",
      "project.pokemon.decision1":
        "ViewModels expose commands for navigation — no logic in code-behind",
      "project.pokemon.decision2": "Repository layer for Pokémon/ability data initialization",
      "project.pokemon.decision3": "Battle rules kept deterministic and easy to extend",
      "project.pokemon.caption1": "Battle UI and catalog navigation — MVVM-bound views",

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
      "project.gametalk.tech.note":
        "Split API and server processes keep concerns isolated; the front-end consumes JSON endpoints for auth, topics, and reactions.",
      "project.gametalk.decision1": "Separate API binary for clearer deployment and testing",
      "project.gametalk.decision2": "Session/auth handled before topic mutations",
      "project.gametalk.decision3": "CORS configuration documented for local dev",
      "project.gametalk.caption1": "Topic list and category navigation — client/server split",

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
      "project.nightlife.tech.note":
        "Branching choice flow built with reusable interaction components; scope cut to shippable vertical slice (club → city).",
      "project.nightlife.decision1": "Priority on stable interactions over content volume",
      "project.nightlife.decision2": "French VO integrated early for comedic timing",
      "project.nightlife.decision3": "Navigation landmarks in night city for readability",
      "project.nightlife.caption1": "Night city navigation and choice-driven encounters",

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
      "project.engine.arch.title": "Architecture",
      "project.engine.arch.note":
        "Systems are layered: a fixed-timestep game loop drives scene updates; each scene owns entity lists and delegates rendering, physics, and gameplay in a strict order.",
      "project.engine.core.title": "Engine core",
      "project.engine.core1": "Fixed update step with accumulator (decouples physics from frame rate)",
      "project.engine.core2": "Scene stack: menu, gameplay levels, game over",
      "project.engine.core3": "Centralized resource loading for textures and fonts",
      "project.engine.systems2d.title": "2D systems",
      "project.engine.systems2d1": "Sprite rendering pipeline with camera-relative transforms",
      "project.engine.systems2d2": "Tile-based level layout with collision proxies",
      "project.engine.systems2d3": "Player controller, enemies, health, and game state flags",
      "project.engine.render.title": "Rendering",
      "project.engine.render.body":
        "SFML handles windowing, input polling, and draw calls. The render pass runs after simulation so visuals always reflect the latest physics state.",
      "project.engine.physics.title": "Physics",
      "project.engine.physics.body":
        "Box2D bodies are owned by gameplay entities. Collisions trigger gameplay events (damage, pickups) without tight coupling to the physics library internals.",
      "project.engine.modular.title": "Modular growth (3D-ready separation)",
      "project.engine.modular.body":
        "Rendering and simulation are isolated behind scene interfaces so future 3D backends could replace SFML draw paths without rewriting gameplay rules.",
      "project.engine.decisions.title": "Key engineering decisions",
      "project.engine.decision1": "Single-threaded loop first — predictable debugging before optimization",
      "project.engine.decision2": "Data-oriented entity lists per scene instead of deep inheritance trees",
      "project.engine.decision3": "Explicit scene transitions to avoid stale physics bodies",
      "project.engine.workflow.title": "Development workflow",
      "project.engine.workflow.body":
        "Iteration cycle: implement engine feature → validate in platformer demo → refactor boundaries → document update order. Git branches per system (physics, UI, levels).",
      "project.engine.timeline.title": "Progression",
      "project.engine.t1.title": "Phase 1 — Core loop",
      "project.engine.t1.body": "Window, input, scene switching, empty gameplay scene",
      "project.engine.t2.title": "Phase 2 — Physics & player",
      "project.engine.t2.body": "Box2D integration, movement, collisions, basic enemies",
      "project.engine.t3.title": "Phase 3 — Content & UI",
      "project.engine.t3.body": "Two levels, menus, score/timer, game over flow",
      "project.engine.caption1":
        "Platformer demo validating physics, level transitions, and UI flow",

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
      "project.platypus.tech.note":
        "Tight scope: one movement controller, three collectibles, timer-driven pressure, and E-key interactions reused across levels.",
      "project.platypus.decision1": "Single interaction component for puzzles and pickups",
      "project.platypus.decision2": "Readable silhouettes and floating paths for dreamlike navigation",
      "project.platypus.decision3": "Polish budget spent on jump feel and camera",
      "project.platypus.caption1": "3D platforming layout — time-limited fragment collection",

      "freelance.meta.title": "LevelWeb-Studio - Hugo Flandrin",
      "freelance.title": "LevelWeb-Studio",
      "freelance.subtitle":
        "LevelWeb Studio is my freelance activity focused on web and video game development. Through this structure, I work on concrete and rewarding projects, ranging from modern web applications to gameplay and technical programming. It allows me to continuously improve my skills while gaining real-world experience in software development, client communication, and project management.",
      "freelance.what.title": "What I can help with",
      "freelance.what.item1": "Web development (front-end & back-end)",
      "freelance.what.item2": "Video game development (gameplay / tools / prototypes)",
      "freelance.what.item3": "Improving and maintaining existing projects",
      "freelance.profile.title": "Profile",
      "freelance.profile.body":
        "I have around 1 year of experience, mainly doing front-end and back-end modifications on existing websites. I position myself as a junior / beginner profile: motivated, rigorous, and focused on shipping useful improvements.",
      "freelance.rates.title": "Rates",
      "freelance.rates.body":
        "Negotiable depending on scope, timeline, and project constraints.",
      "freelance.cta.title": "Let’s talk",
      "freelance.cta.body":
        "If you have a project in mind (new build, improvements, bug fixes, or a prototype), send me a message and we’ll discuss the best collaboration format.",
      "freelance.cta.contactLink": "Go to contact",
    },
    fr: {
      "nav.home": "Accueil",
      "nav.experience": "Expérience",
      "nav.projects": "Projets",
      "nav.freelance": "LevelWeb-Studio",
      "nav.contact": "Contact",
      "nav.lang": "EN",

      "home.hero.eyebrow": "Programmeur Gameplay & Moteur",
      "home.hero.lead":
        "Je conçois des systèmes de gameplay modulaires et des fondations de moteur en C++, avec une expérience concrète sur moteurs maison, Unreal Engine et Unity.",
      "home.cta.resume": "Voir le CV",
      "home.cta.github": "GitHub",
      "home.cta.linkedin": "LinkedIn",
      "home.status.label": "Actuellement à la recherche de",
      "home.status.text":
        "Stage ou poste junior — Programmeur Gameplay, Programmeur Moteur, ou Programmeur Généraliste (C++ / Unreal / moteur maison).",
      "home.featured.title": "Projet phare",
      "home.featured.tag": "Moteur · C++ · SFML · Box2D",
      "home.featured.summary":
        "Moteur C++ maison avec systèmes modulaires (boucle de jeu, scènes, rendu, physique) validé par une démo platformer complète.",
      "home.featured.point1": "Ordre d'update déterministe et transitions de scènes",
      "home.featured.point2": "Box2D intégré aux entités gameplay",
      "home.featured.point3": "Architecture pensée pour faire évoluer le moteur",
      "home.featured.cta": "Voir le détail technique",
      "home.experience.title": "Expérience professionnelle",
      "home.experience.sub": "Exposition terrain et projets moteur académiques",
      "home.exp1.type": "Freelance",
      "home.exp1.period": "2024 — Aujourd'hui",
      "home.exp1.role": "Développement Web & Jeu — Freelance",
      "home.exp1.item1": "Modifications front-end et back-end sur des sites en production",
      "home.exp1.item2": "Livraison de fonctionnalités, corrections et boucles client",
      "home.exp1.item3": "Prototypes web et jeu à périmètre réduit sous contrainte de temps",
      "home.exp2.type": "Académique / Personnel",
      "home.exp2.period": "2023 — Aujourd'hui",
      "home.exp2.role": "Projets Gameplay & Moteur — Lead programmeur",
      "home.exp2.item1": "Conception et implémentation d'un moteur 2D modulaire (SFML + Box2D)",
      "home.exp2.item2": "Systèmes gameplay livrés sur UE5 et Unity (jam & projets universitaires)",
      "home.exp2.item3": "Décisions d'architecture, debug et respect des jalons",
      "home.exp3.type": "Université",
      "home.exp3.period": "Projets d'équipe",
      "home.exp3.role": "Développement Jeu — Programmeur",
      "home.exp3.item1": "Collaboration multidisciplinaire (game design, art, audio)",
      "home.exp3.item2": "Builds jouables livrés en hackathon et projets de cours",
      "home.exp3.item3": "Interactions joueur, rythme et scripting de niveaux",
      "home.projects.title": "Projets",
      "home.projects.sub": "Études de cas techniques — cliquez pour les détails d'ingénierie",
      "home.card.engine.tag": "Moteur",
      "home.card.engine.summary": "C++ · SFML · Box2D · architecture modulaire",
      "home.card.escape.tag": "Gameplay · UE5",
      "home.card.escape.summary": "Thriller FPS · énigmes · atmosphère",
      "home.card.nightlife.tag": "Gameplay · UE5",
      "home.card.nightlife.summary": "Hackathon 5 jours · choix narratifs",
      "home.card.platypus.tag": "Gameplay · Unity",
      "home.card.platypus.summary": "Jam 48h · platformer 3D",
      "home.card.gametalk.tag": "Full-stack · Go",
      "home.card.gametalk.summary": "API + serveur · système forum",
      "home.card.pokemon.tag": "Application · C#",
      "home.card.pokemon.summary": "WPF · MVVM · SQL Server",
      "home.contact.sub": "Recruteurs et collaborateurs — réponse sous quelques jours",
      "home.footer.about":
        "Ce portfolio met en avant les choix d'ingénierie et l'organisation des systèmes — site statique léger (HTML/CSS/JS).",
      "home.footer.version": "Portfolio v.0.3",
      "home.footer.updated": "Mis à jour — mai 2026",

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
      "project.tech.title": "Focus technique",
      "project.decisions.title": "Décisions clés",
      "project.workflow.title": "Workflow",
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
      "project.escape.tech.note":
        "Les systèmes gameplay privilégient une interaction lisible dans l'obscurité : exploration à la lampe, objets inspectables et énigmes liées au récit.",
      "project.escape.decision1":
        "Pression temporelle implicite via audio/visuel plutôt que minuteurs à l'écran",
      "project.escape.decision2": "Acteurs d'énigmes modulaires avec interface d'interaction commune",
      "project.escape.decision3": "Volumes de lumière réglés par pièce pour une lisibilité constante",
      "project.escape.workflow":
        "Blockout → passe gameplay → polish lumière/audio → playtests clarté des énigmes.",
      "project.escape.caption1":
        "Exploration en basse lumière — lampe torche comme outil de navigation",
      "project.escape.caption2":
        "Espace d'énigme conçu pour l'enquête et la narration environnementale",
      "project.escape.captionVideo": "Démo d'avril — boucle d'interaction et rythme d'atmosphère",

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
      "project.pokemon.tech.note":
        "Séparation MVVM : vues légères, logique de combat et accès données dans des services testables avec catalogues SQL.",
      "project.pokemon.decision1":
        "ViewModels avec commandes de navigation — pas de logique dans le code-behind",
      "project.pokemon.decision2": "Couche repository pour l'initialisation des données Pokémon/capacités",
      "project.pokemon.decision3": "Règles de combat déterministes et faciles à étendre",
      "project.pokemon.caption1": "UI de combat et navigation catalogues — vues liées MVVM",

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
      "project.gametalk.tech.note":
        "API et serveur séparés : le front consomme des endpoints JSON pour auth, sujets et réactions.",
      "project.gametalk.decision1": "Binaire API distinct pour déploiement et tests plus clairs",
      "project.gametalk.decision2": "Auth/session avant les mutations de contenu",
      "project.gametalk.decision3": "Configuration CORS documentée pour le dev local",
      "project.gametalk.caption1": "Liste de sujets et catégories — architecture client/serveur",

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
      "project.nightlife.tech.note":
        "Flux à embranchements avec composants d'interaction réutilisables ; scope réduit à une tranche verticale livrable (boîte → ville).",
      "project.nightlife.decision1": "Priorité aux interactions stables plutôt qu'au volume de contenu",
      "project.nightlife.decision2": "VO française intégrée tôt pour le timing comique",
      "project.nightlife.decision3": "Repères de navigation dans la ville nocturne",
      "project.nightlife.caption1": "Navigation nocturne et rencontres basées sur les choix",

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
      "project.engine.arch.title": "Architecture",
      "project.engine.arch.note":
        "Systèmes en couches : boucle à pas fixe, scènes propriétaires des entités, rendu/physique/gameplay dans un ordre strict.",
      "project.engine.core.title": "Cœur du moteur",
      "project.engine.core1": "Update à pas fixe avec accumulateur (physique découplée du framerate)",
      "project.engine.core2": "Pile de scènes : menu, niveaux, game over",
      "project.engine.core3": "Chargement centralisé des textures et polices",
      "project.engine.systems2d.title": "Systèmes 2D",
      "project.engine.systems2d1": "Pipeline de sprites avec transforms relatifs à la caméra",
      "project.engine.systems2d2": "Niveaux en tuiles avec proxies de collision",
      "project.engine.systems2d3": "Joueur, ennemis, santé et états de jeu",
      "project.engine.render.title": "Rendu",
      "project.engine.render.body":
        "SFML gère fenêtre, entrées et draw calls. Le rendu suit la simulation pour refléter l'état physique.",
      "project.engine.physics.title": "Physique",
      "project.engine.physics.body":
        "Les corps Box2D appartiennent aux entités gameplay. Les collisions déclenchent des événements sans couplage fort à la lib.",
      "project.engine.modular.title": "Croissance modulaire (séparation prête 3D)",
      "project.engine.modular.body":
        "Rendu et simulation isolés derrière des interfaces de scène pour pouvoir remplacer le backend draw plus tard.",
      "project.engine.decisions.title": "Décisions d'ingénierie",
      "project.engine.decision1": "Boucle mono-thread d'abord — debug prévisible avant optimisation",
      "project.engine.decision2": "Listes d'entités orientées données plutôt qu'arbres d'héritage profonds",
      "project.engine.decision3": "Transitions de scène explicites pour éviter les corps physiques obsolètes",
      "project.engine.workflow.title": "Workflow de développement",
      "project.engine.workflow.body":
        "Feature moteur → validation dans la démo platformer → refactor des frontières → documentation de l'ordre d'update.",
      "project.engine.timeline.title": "Progression",
      "project.engine.t1.title": "Phase 1 — Boucle cœur",
      "project.engine.t1.body": "Fenêtre, entrées, changement de scène, scène gameplay vide",
      "project.engine.t2.title": "Phase 2 — Physique & joueur",
      "project.engine.t2.body": "Intégration Box2D, déplacements, collisions, ennemis basiques",
      "project.engine.t3.title": "Phase 3 — Contenu & UI",
      "project.engine.t3.body": "Deux niveaux, menus, score/timer, flux game over",
      "project.engine.caption1":
        "Démo platformer validant physique, transitions et UI",

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
      "project.platypus.tech.note":
        "Scope serré : un contrôleur, trois collectibles, timer, interactions touche E réutilisées.",
      "project.platypus.decision1": "Un composant d'interaction pour énigmes et collectibles",
      "project.platypus.decision2": "Silhouettes lisibles et chemins flottants pour l'exploration onirique",
      "project.platypus.decision3": "Budget polish sur le saut et la caméra",
      "project.platypus.caption1": "Layout platformer 3D — collecte de fragments limitée dans le temps",

      "freelance.meta.title": "LevelWeb-Studio - Hugo Flandrin",
      "freelance.title": "LevelWeb-Studio",
      "freelance.subtitle":
        "LevelWeb Studio est mon activité indépendante axée sur le développement web et de jeux vidéo. Dans le cadre de cette structure, je travaille sur des projets concrets et enrichissants, allant des applications web modernes à la programmation technique et au gameplay. Cela me permet d'améliorer continuellement mes compétences tout en acquérant une expérience pratique dans le développement logiciel, la communication avec les clients et la gestion de projets.",
      "freelance.what.title": "Ce que je peux faire",
      "freelance.what.item1": "Développement web (front-end & back-end)",
      "freelance.what.item2":
        "Développement de jeux vidéo (gameplay / outils / prototypes)",
      "freelance.what.item3": "Amélioration et maintenance de projets existants",
      "freelance.profile.title": "Profil",
      "freelance.profile.body":
        "J’ai environ 1 an d’expérience, principalement sur des prestations de modifications front-end et back-end sur des sites internet existants. Je me positionne comme un profil jeune / débutant : motivé, rigoureux, et orienté résultats.",
      "freelance.rates.title": "Rémunération",
      "freelance.rates.body":
        "Négociable selon le périmètre, les délais et les contraintes du projet.",
      "freelance.cta.title": "Discutons",
      "freelance.cta.body":
        "Si vous avez un projet en tête (création, améliorations, corrections de bugs, ou prototype), envoyez-moi un message et nous définirons ensemble le format de collaboration le plus adapté.",
      "freelance.cta.contactLink": "Aller au contact",
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

