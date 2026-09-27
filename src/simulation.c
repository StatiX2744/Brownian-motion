/**
 * @file simulation.c
 * @brief Génération et affichage des simulations de mouvement brownien.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "graphing.h"
#include "mt19937ar.h"
#include "simulation.h"
#include "window.h"

/* ========================================================================== */
/*                               Outils internes                              */
/* ========================================================================== */

/**
 * @brief Génère une variable normale centrée réduite avec Box-Muller.
 *
 * La transformation de Box-Muller convertit deux variables uniformes
 * indépendantes U1 et U2 en une variable suivant une loi normale N(0,1).
 */
precision box_muller_1D(precision u1, precision u2)
{
    return sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
}

/**
 * @brief Génère un déplacement gaussien dans le plan.
 */
coo box_muller_2D(precision u1, precision u2)
{
    const precision radius = sqrt(-2.0 * log(u1));
    const precision angle = 2.0 * M_PI * u2;

    coo result = {
        radius * cos(angle),
        radius * sin(angle)
    };

    return result;
}

/**
 * @brief Calcule la distance euclidienne entre deux points.
 */
static precision distance_between(coo a, coo b)
{
    const precision dx = a.x - b.x;
    const precision dy = a.y - b.y;

    return sqrt(dx * dx + dy * dy);
}

/**
 * @brief Crée un graphique 2D avec les paramètres standards du projet.
 */
static graph *create_standard_2D_graph(SDL_Renderer *renderer)
{
    return create_graph(
        renderer,
        -MAX_2D_SIZE, MAX_2D_SIZE,
        -MAX_2D_SIZE, MAX_2D_SIZE,
        HORIZONTAL_GAP_2D, HORIZONTAL_SUBLINES_2D,
        VERTICAL_GAP_2D, VERTICAL_SUBLINES_2D
    );
}

/* ========================================================================== */
/*                            Simulation 1D                                  */
/* ========================================================================== */

/**
 * @brief Génère une trajectoire brownienne en une dimension.
 *
 * Chaque position est obtenue en ajoutant un déplacement gaussien à
 * la position précédente.
 *
 * @param sim_size Nombre de positions à générer.
 * @return Tableau des positions, ou NULL en cas d'erreur.
 */
precision *trajectory_1D(int sim_size)
{
    precision *trajectory;

    if (sim_size <= 0) {
        return NULL;
    }

    trajectory = malloc((size_t)sim_size * sizeof(*trajectory));
    if (trajectory == NULL) {
        fprintf(stderr,
                "Erreur d'allocation de la trajectoire 1D.\n");
        return NULL;
    }

    for (int i = 0; i < sim_size; ++i) {
        const precision previous =
            (i == 0) ? 0.0 : trajectory[i - 1];

        trajectory[i] =
            previous + box_muller_1D(genrand_real3(), genrand_real3());
    }

    return trajectory;
}

/**
 * @brief Ajoute une trajectoire 1D à un tableau de sommes.
 *
 * Le tableau tab doit avoir été initialisé à zéro.
 */
void add_trajectory_1D(precision *tab, int size)
{
    precision position = 0.0;

    if (tab == NULL || size <= 0) {
        return;
    }

    for (int i = 0; i < size; ++i) {
        tab[i] += position;

        position +=
            box_muller_1D(genrand_real3(), genrand_real3());
    }
}

/**
 * @brief Calcule la moyenne de plusieurs trajectoires 1D.
 *
 * @return Tableau contenant la position moyenne à chaque instant.
 */
precision *simulation_increments_1D(int sim_number, int sim_size)
{
    precision *average;

    if (sim_number <= 0 || sim_size <= 0) {
        return NULL;
    }

    /* calloc est important : add_trajectory_1D additionne sur ce tableau. */
    average = calloc((size_t)sim_size, sizeof(*average));
    if (average == NULL) {
        fprintf(stderr,
                "Erreur d'allocation de la moyenne 1D.\n");
        return NULL;
    }

    for (int i = 0; i < sim_number; ++i) {
        add_trajectory_1D(average, sim_size);
    }

    for (int i = 0; i < sim_size; ++i) {
        average[i] /= sim_number;
    }

    return average;
}

/**
 * @brief Affiche une trajectoire 1D dans une fenêtre SDL.
 */
void trajectory_1D_graphique(SDL_Renderer *renderer, int sim_size)
{
    precision *values;
    graph *graphic;
    precision min;
    precision max;

    values = trajectory_1D(sim_size);
    if (values == NULL) {
        return;
    }

    min = values[0];
    max = values[0];

    /* Recherche des bornes verticales nécessaires au graphique. */
    for (int i = 1; i < sim_size; ++i) {
        if (values[i] < min) {
            min = values[i];
        }
        if (values[i] > max) {
            max = values[i];
        }
    }

    graphic = create_graph(
        renderer,
        -LEFT_PADDING,
        sim_size + RIGHT_PADDING,
        min - VERTICAL_PADDING,
        max + VERTICAL_PADDING,
        HORIZONTAL_GAP,
        HORIZONTAL_SUBLINES,
        VERTICAL_GAP,
        VERTICAL_SUBLINES
    );

    if (graphic != NULL) {
        plot_line(renderer, graphic, values, 1, sim_size);
        free(graphic);
    }

    free(values);
}

/**
 * @brief Affiche la moyenne de plusieurs trajectoires 1D.
 */
void trajectory_average_graph(SDL_Renderer *renderer,
                              int sim_size, int sim_number)
{
    precision *average;
    graph *graphic;
    precision min;
    precision max;

    average = simulation_increments_1D(sim_number, sim_size);
    if (average == NULL) {
        return;
    }

    min = average[0];
    max = average[0];

    for (int i = 1; i < sim_size; ++i) {
        if (average[i] < min) {
            min = average[i];
        }
        if (average[i] > max) {
            max = average[i];
        }
    }

    graphic = create_graph(
        renderer,
        -LEFT_PADDING,
        sim_size + RIGHT_PADDING,
        min - VERTICAL_PADDING,
        max + VERTICAL_PADDING,
        HORIZONTAL_GAP,
        HORIZONTAL_SUBLINES,
        VERTICAL_GAP,
        VERTICAL_SUBLINES
    );

    if (graphic != NULL) {
        plot_line(renderer, graphic, average, 1, sim_size);
        free(graphic);
    }

    free(average);
}

/* ========================================================================== */
/*                            Simulation 2D                                  */
/* ========================================================================== */

/**
 * @brief Génère une trajectoire brownienne complète en 2D.
 */
coo *trajectory_2D(int sim_size)
{
    coo *trajectory;

    if (sim_size <= 0) {
        return NULL;
    }

    trajectory = malloc((size_t)sim_size * sizeof(*trajectory));
    if (trajectory == NULL) {
        fprintf(stderr,
                "Erreur d'allocation de la trajectoire 2D.\n");
        return NULL;
    }

    for (int i = 0; i < sim_size; ++i) {
        const coo increment =
            box_muller_2D(genrand_real3(), genrand_real3());

        const coo previous = (i == 0)
            ? (coo){0.0, 0.0}
            : trajectory[i - 1];

        trajectory[i].x = previous.x + increment.x;
        trajectory[i].y = previous.y + increment.y;
    }

    return trajectory;
}

/**
 * @brief Ajoute une trajectoire 2D à un tableau de sommes.
 */
void add_trajectory_2D(coo *tab, int size)
{
    coo position = {0.0, 0.0};

    if (tab == NULL || size <= 0) {
        return;
    }

    for (int i = 0; i < size; ++i) {
        tab[i].x += position.x;
        tab[i].y += position.y;

        const coo increment =
            box_muller_2D(genrand_real3(), genrand_real3());

        position.x += increment.x;
        position.y += increment.y;
    }
}

/**
 * @brief Calcule la moyenne de plusieurs trajectoires 2D.
 */
coo *simulation_increments_2D(int sim_number, int sim_size)
{
    coo *average;

    if (sim_number <= 0 || sim_size <= 0) {
        return NULL;
    }

    /* Le tableau doit être initialisé à zéro avant les additions. */
    average = calloc((size_t)sim_size, sizeof(*average));
    if (average == NULL) {
        fprintf(stderr,
                "Erreur d'allocation de la moyenne 2D.\n");
        return NULL;
    }

    for (int i = 0; i < sim_number; ++i) {
        add_trajectory_2D(average, sim_size);
    }

    for (int i = 0; i < sim_size; ++i) {
        average[i].x /= sim_number;
        average[i].y /= sim_number;
    }

    return average;
}

/**
 * @brief Affiche une trajectoire 2D générée entièrement avant l'affichage.
 */
void trajectory_2D_graphique(SDL_Renderer *renderer, int sim_size)
{
    coo *trajectory;
    graph *graphic;

    trajectory = trajectory_2D(sim_size);
    if (trajectory == NULL) {
        return;
    }

    graphic = create_standard_2D_graph(renderer);
    if (graphic != NULL) {
        plot_2D(renderer, graphic, trajectory, sim_size);
        free(graphic);
    }

    free(trajectory);
}

/**
 * @brief Affiche la moyenne de plusieurs trajectoires 2D.
 */
void trajectory_2D_average_graph(SDL_Renderer *renderer,
                                 int sim_size, int sim_number)
{
    coo *average;
    graph *graphic;

    average = simulation_increments_2D(sim_number, sim_size);
    if (average == NULL) {
        return;
    }

    graphic = create_standard_2D_graph(renderer);
    if (graphic != NULL) {
        plot_2D(renderer, graphic, average, sim_size);
        free(graphic);
    }

    free(average);
}

/**
 * @brief Affiche une marche 2D progressivement, à 60 images/seconde.
 */
void trajectory_2D_live(SDL_Renderer *renderer, int sim_size)
{
    const Uint32 frame_duration = 1000 / FPS;
    coo current = {0.0, 0.0};
    graph *graphic;
    int running = 1;

    graphic = create_standard_2D_graph(renderer);
    if (graphic == NULL) {
        return;
    }

    for (int i = 0; i < sim_size && running; ++i) {
        const Uint32 frame_start = SDL_GetTicks();
        const coo previous = current;

        process_input(&running);
        if (!running) {
            break;
        }

        /* Génération du nouveau déplacement brownien. */
        const coo increment =
            box_muller_2D(genrand_real3(), genrand_real3());

        current.x += increment.x / DIMINUTION_FACTOR;
        current.y += increment.y / DIMINUTION_FACTOR;

        /* Dessine le point et le segment reliant les deux positions. */
        plot(renderer, graphic, current.x, current.y, 1);

        const SDL_Point previous_screen =
            toGraphCoords(graphic, previous.x, previous.y);
        const SDL_Point current_screen =
            toGraphCoords(graphic, current.x, current.y);

        SDL_RenderDrawLine(renderer,
                           previous_screen.x, previous_screen.y,
                           current_screen.x, current_screen.y);

        show(renderer);

        /* Attend seulement le temps restant du frame. */
        const Uint32 elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < frame_duration) {
            SDL_Delay(frame_duration - elapsed);
        }
    }

    free(graphic);
}

/**
 * @brief Calcule et affiche la position finale moyenne de plusieurs marches.
 */
void trajectory_2D_average(SDL_Renderer *renderer,
                           int sim_size, int sim_number)
{
    coo average = {0.0, 0.0};
    graph *graphic;

    if (sim_size <= 0 || sim_number <= 0) {
        return;
    }

    for (int simulation = 0; simulation < sim_number; ++simulation) {
        coo current = {0.0, 0.0};

        for (int step = 0; step < sim_size; ++step) {
            const coo increment =
                box_muller_2D(genrand_real3(), genrand_real3());

            current.x += increment.x / DIMINUTION_FACTOR;
            current.y += increment.y / DIMINUTION_FACTOR;
        }

        average.x += current.x;
        average.y += current.y;
    }

    average.x /= sim_number;
    average.y /= sim_number;

    graphic = create_standard_2D_graph(renderer);
    if (graphic != NULL) {
        plot(renderer, graphic, average.x, average.y, 0);
        free(graphic);
    }

    printf("Position moyenne finale : (%f, %f)\n",
           average.x, average.y);
}

/* ========================================================================== */
/*                          Atteinte d'une cible                             */
/* ========================================================================== */

/**
 * @brief Simule une marche jusqu'à ce qu'elle entre dans une zone autour
 *        d'une cible.
 *
 * @return Nombre de pas nécessaires, ou -1 si la cible n'est pas atteinte
 *         avant max_interval.
 */
int nb_step(coo target, precision gap, int max_interval)
{
    coo position = {0.0, 0.0};

    for (int step = 1; step <= max_interval; ++step) {
        const coo increment =
            box_muller_2D(genrand_real3(), genrand_real3());

        position.x += increment.x;
        position.y += increment.y;

        if (distance_between(position, target) <= gap) {
            return step;
        }
    }

    return -1;
}

/**
 * @brief Calcule et affiche l'histogramme du nombre de pas nécessaires
 *        pour atteindre une cible.
 */
void histogram2D(int size_interval, int sim_number,
                 int sim_size, coo target, precision gap)
{
    int *histogram;
    int out_of_bounds = 0;
    const int histogram_size =
        (sim_size + size_interval - 1) / size_interval;

    if (size_interval <= 0 || sim_number <= 0 || sim_size <= 0) {
        fprintf(stderr, "Paramètres d'histogramme invalides.\n");
        return;
    }

    histogram = calloc((size_t)histogram_size, sizeof(*histogram));
    if (histogram == NULL) {
        fprintf(stderr, "Erreur d'allocation de l'histogramme.\n");
        return;
    }

    for (int simulation = 0; simulation < sim_number; ++simulation) {
        const int steps = nb_step(target, gap, sim_size);

        if (steps < 0) {
            ++out_of_bounds;
            continue;
        }

        /* Le dernier intervalle doit aussi pouvoir contenir sim_size. */
        int bucket = steps / size_interval;
        if (bucket >= histogram_size) {
            bucket = histogram_size - 1;
        }
        ++histogram[bucket];
    }

    for (int i = 0; i < histogram_size; ++i) {
        printf("[%d ; %d] : %d\n",
               i * size_interval,
               (i + 1) * size_interval,
               histogram[i]);
    }

    printf("Nombre de points n'ayant pas atteint la cible : %d\n",
           out_of_bounds);

    free(histogram);
}

/* ========================================================================== */
/*                              Programme principal                           */
/* ========================================================================== */

/**
 * @brief Lit les paramètres nécessaires à une simulation.
 */
static int read_simulation_parameters(int id,
                                      int *sim_size,
                                      int *sim_number)
{
    puts("Entrez la taille de la simulation à effectuer :");

    if (scanf("%d", sim_size) != 1 || *sim_size <= 0) {
        fprintf(stderr, "Taille de simulation invalide.\n");
        return 0;
    }

    if (id > 2) {
        puts("Entrez le nombre de simulations :");

        if (scanf("%d", sim_number) != 1 || *sim_number <= 0) {
            fprintf(stderr, "Nombre de simulations invalide.\n");
            return 0;
        }
    }

    return 1;
}

/**
 * @brief Exécute la simulation correspondant à l'identifiant demandé.
 */
int run(int id)
{
    int sim_size = 0;
    int sim_number = 0;

    /* Vérification du choix avant toute initialisation graphique. */
    if (id < 1 || id > 5) {
        fprintf(stderr, "Choix de simulation invalide.\n");
        return EXIT_FAILURE;
    }

    /* Initialise le générateur pseudo-aléatoire avant toute simulation. */
    init_genrand((unsigned long)time(NULL));

    if (id == 5) {
        int interval;
        coo target;
        precision gap;

        if (!read_simulation_parameters(id, &sim_size, &sim_number)) {
            return EXIT_FAILURE;
        }

        puts("Entrez la taille des intervalles :");
        if (scanf("%d", &interval) != 1 || interval <= 0) {
            fprintf(stderr, "Taille d'intervalle invalide.\n");
            return EXIT_FAILURE;
        }

        puts("Entrez les coordonnées de la cible :");
        if (scanf("%lf %lf", &target.x, &target.y) != 2) {
            fprintf(stderr, "Coordonnées invalides.\n");
            return EXIT_FAILURE;
        }

        puts("Entrez la marge d'erreur :");
        if (scanf("%lf", &gap) != 1 || gap < 0.0) {
            fprintf(stderr, "Marge d'erreur invalide.\n");
            return EXIT_FAILURE;
        }

        histogram2D(interval, sim_number, sim_size, target, gap);
        return EXIT_SUCCESS;
    }

    if (!read_simulation_parameters(id, &sim_size, &sim_number)) {
        return EXIT_FAILURE;
    }

    if (TTF_Init() != 0) {
        fprintf(stderr, "Erreur TTF_Init : %s\n", TTF_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    if (!initialize_window(&window, &renderer)) {
        TTF_Quit();
        return EXIT_FAILURE;
    }

    /* Fond blanc pour les graphiques. */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    /* Lance la fonction correspondant au choix utilisateur. */
    switch (id) {
    case 1:
        trajectory_1D_graphique(renderer, sim_size);
        break;

    case 2:
        trajectory_2D_live(renderer, sim_size);
        break;

    case 3:
        trajectory_average_graph(renderer, sim_size, sim_number);
        break;

    case 4:
        trajectory_2D_average_graph(renderer, sim_size, sim_number);
        break;

    default:
        /* Le choix a déjà été vérifié plus haut. */
        break;
    }

    /*
     * Boucle d'affichage : la fenêtre reste ouverte jusqu'à la fermeture
     * de la fenêtre ou à l'appui sur Échap.
     */
    int main_loop = 1;
    const Uint32 frame_duration = 1000 / FPS;

    while (main_loop) {
        const Uint32 frame_start = SDL_GetTicks();

        process_input(&main_loop);
        show(renderer);

        const Uint32 elapsed = SDL_GetTicks() - frame_start;
        if (elapsed < frame_duration) {
            SDL_Delay(frame_duration - elapsed);
        }
    }

    destroy_window(&window, &renderer);
    TTF_Quit();

    return EXIT_SUCCESS;
}
