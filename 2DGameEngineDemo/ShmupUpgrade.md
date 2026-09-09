# Idées d'amélioration - Shmup & Platformer

Document de suivi des idées de gameplay discutées, pour référence avant implémentation. Rien n'est codé pour l'instant.

## Shmup

### Mode Score Attack (90s) - existant, inchangé

Déjà calibré et jugé fun tel quel (chasse au high score, ~10k atteint en quelques minutes de pratique). Ne reçoit **pas** la courbe de difficulté ni les power-up : la partie est trop courte pour que ces systèmes aient le temps de se faire sentir. Reste le "sprint difficile" de référence.

### Mode Infini (nouveau)

Pas de limite de temps - la partie continue tant que le joueur survit. C'est ici que vivent la courbe de difficulté et les power-up, puisqu'ils ont besoin d'espace pour exister.

- **Courbe de difficulté** : `EnemySpawner` a actuellement un rythme de spawn et un mix de types **fixes** sur toute la durée (`EnemySpawner.cpp:11-29` - intervalle 0.7-1.8s, proba 50/25/15/10 Basic/Fast/Tank/Shooter, constant du début à la fin). Pour le mode infini, il faut probablement **démarrer plus doux** que cette densité actuelle (le rythme actuel monte déjà vite) puis grimper progressivement pour la rejoindre, la dépasser, et continuer au-delà. Ne pas utiliser la densité du mode 90s comme plancher de départ.
- **Power-up** : tir plus rapide, tir multi-directionnel, etc. Mécanisme de distribution le plus naturel vu l'archi existante : drop occasionnel à la mort d'un ennemi, qui tombe comme un ennemi normal - le joueur doit se déplacer pour l'intercepter (risque/récompense, réutilise le pattern de spawn/chute déjà en place).
- **Fin de partie / score** : pas de fin naturelle, donc le "high score" a plus de sens ici que dans le mode 90s. L'écran de fin devrait probablement afficher le **temps survécu** en plus du score, puisque ce sera la vraie mesure de performance dans ce mode.

### Profondeur de scoring : streaks

Le jeu tue en un seul coup (`ShipBehavior::beginCollision`, pas de tampon de vie). Un streak basé sur "pas de dégât reçu" serait donc identique au score de la run entière (un coup = fin du streak = fin de la run) - ça n'ajouterait rien.

**Piste retenue à valider** : streak basé sur le **rythme des kills** - tuer dans les X secondes qui suivent le kill précédent prolonge/augmente un multiplicateur, sinon il retombe à 1x. Fonctionne bien avec la mort instantanée actuelle, encourage à jouer agressif plutôt que défensif, ne dépend d'aucun autre changement.

**Questions ouvertes :**
- Kill-chain (rythme) confirmé, ou variante "no-hit" voulue malgré la remarque ci-dessus ?
- Appliqué aux deux modes (90s + Infini), ou réservé à l'Infini ? Le système est peu coûteux et renforce la boucle déjà appréciée en 90s - recommandation : les deux.

### Leaderboard

Écran de fin de partie qui affiche un classement, avec les boutons habituels "Rejouer" / "Fermer". En cas de victoire, bouton additionnel type "Enregistrer mon score" qui permet au joueur de saisir un pseudo et d'apparaître dans le leaderboard global (opt-in, pas automatique).

**Implications à anticiper (pas encore tranchées) :**
- Backend/stockage nécessaire (le jeu est aujourd'hui 100% client-side, WASM statique) - il faudra une forme de persistance côté serveur pour un leaderboard *global*.
- "En cas de victoire" : à définir pour chaque mode - fin du timer (90s) vs quel événement pour l'Infini (mort ? seuil de score/temps ?).
- Score comparable entre le mode 90s et le mode Infini ? Probablement deux classements séparés (score fixe vs score+temps survécu ne se comparent pas).

### Idées explorées mais non retenues (pour référence)

- Vies/tampon de dégât (2-3 HP + invincibilité brève après un coup) - écarté pour l'instant, mais à reconsidérer si un système "no-hit streak" devient prioritaire un jour.
- Juice/feedback (screen shake, sons différenciés par type de kill, écran de fin plus riche) - pas écarté, juste pas priorisé dans cette passe.

## Platformer

### Timer façon Mario

Ajout d'un timer à la partie (le composant `CountdownUI` existant est déjà générique et réutilisable tel quel pour ça - voir `Engine/CountdownUI.h`, conçu pour ne pas dépendre du Shmup).

- Timer à 0 → partie perdue.
- Victoire → le temps restant se convertit en score (plus le niveau est terminé vite, plus le score est grand). Le Platformer a déjà un système de score partagé avec le Shmup (`AScene::addScore()`, utilisé par exemple dans `CoinBehavior.cpp`), donc la conversion temps→score se branche directement dessus sans nouveau système.

**Questions ouvertes :**
- Durée du timer par niveau (fixe, ou calibrée par niveau selon sa longueur) ?
- Formule de conversion temps restant → score (linéaire ? palier ?).
