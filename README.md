# Projet ISIMA S4 — Mouvement Brownien

Projet de simulation du mouvement brownien en **1D et 2D**, avec affichage
graphique grâce à **SDL2** et **SDL2_ttf**.

## Auteurs

- Vivien THOMAS
- Tristan PICHARD

## Organisation

```text
.
├── main.c                  # Point d'entrée du programme
├── Makefile                # Compilation et commandes du projet
├── font/
│   └── Arial.ttf           # Police utilisée pour les graphiques
├── src/
│   ├── graphing.c/.h       # Fonctions de dessin et de conversion
│   ├── mt19937ar.c/.h      # Générateur pseudo-aléatoire MT19937
│   ├── simulation.c/.h     # Algorithmes de simulation
│   └── window.c/.h         # Gestion de la fenêtre SDL
└── report/
    └── report.tex          # Rapport LaTeX
```

## Compilation

Les dépendances nécessaires sont :

- GCC
- SDL2
- SDL2_ttf
- Make
- LaTeX / `pdflatex` pour le rapport

Pour compiler :

```bash
make build
```

Pour compiler puis exécuter :

```bash
make run
```

## Simulations disponibles

Le programme propose cinq modes :

1. **Trajectoire 1D** — génération et affichage d'une marche brownienne en 1D.
2. **Trajectoire 2D** — affichage progressif d'une marche brownienne en 2D.
3. **Trajectoire 1D moyenne** — moyenne de plusieurs trajectoires 1D.
4. **Trajectoire 2D moyenne** — moyenne de plusieurs trajectoires 2D.
5. **Histogramme 2D** — étude du nombre de pas nécessaires pour atteindre une
   zone autour d'une cible.

La touche **Échap** permet de fermer une simulation graphique.

## Commandes Make

| Commande | Rôle |
|---|---|
| `make build` | Compile le programme |
| `make run` | Compile puis lance le programme |
| `make pdf` | Compile le rapport LaTeX |
| `make zip` | Crée une archive du projet |
| `make clean` | Supprime les fichiers générés |

## Principales améliorations du code

La version nettoyée conserve les fonctionnalités du projet tout en améliorant
notamment :

- l'indentation et la convention de nommage ;
- la séparation entre simulation, affichage et gestion de la fenêtre ;
- les commentaires et la documentation des fonctions ;
- la gestion des erreurs d'allocation mémoire ;
- l'initialisation des tableaux utilisés pour calculer les moyennes ;
- la vérification des saisies utilisateur ;
- la libération des ressources SDL et SDL_ttf ;
- la gestion du temps de rafraîchissement ;
- la génération du repère, dont les boucles répétitives ont été factorisées ;
- la gestion des cas limites de l'histogramme.
