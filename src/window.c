/**
 * @file window.c
 * @brief Gestion de la fenêtre et des événements SDL.
 */

#include <stdio.h>

#include "window.h"

/**
 * @brief Traite les événements envoyés par SDL.
 *
 * La fenêtre peut être fermée avec la croix ou avec la touche Échap.
 */
void process_input(int *main_loop)
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            *main_loop = 0;
            break;

        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                *main_loop = 0;
            }
            break;

        default:
            break;
        }
    }
}

/**
 * @brief Initialise SDL, la fenêtre et le renderer.
 */
int initialize_window(SDL_Window **window, SDL_Renderer **renderer)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Erreur lors de l'initialisation de SDL : %s\n",
                SDL_GetError());
        return 0;
    }

    *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (*window == NULL) {
        fprintf(stderr, "Erreur lors de la création de la fenêtre : %s\n",
                SDL_GetError());
        SDL_Quit();
        return 0;
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

    if (*renderer == NULL) {
        fprintf(stderr, "Erreur lors de la création du renderer : %s\n",
                SDL_GetError());
        SDL_DestroyWindow(*window);
        *window = NULL;
        SDL_Quit();
        return 0;
    }

    return 1;
}

/**
 * @brief Détruit proprement les ressources SDL.
 */
void destroy_window(SDL_Window **window, SDL_Renderer **renderer)
{
    if (renderer != NULL && *renderer != NULL) {
        SDL_DestroyRenderer(*renderer);
        *renderer = NULL;
    }

    if (window != NULL && *window != NULL) {
        SDL_DestroyWindow(*window);
        *window = NULL;
    }

    SDL_Quit();
}

/**
 * @brief Présente le frame courant à l'écran.
 */
void show(SDL_Renderer *renderer)
{
    SDL_RenderPresent(renderer);
}
