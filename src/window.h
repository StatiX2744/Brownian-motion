/**
 * @file window.h
 * @brief Fonctions et constantes liées à la fenêtre SDL.
 */

#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

/* Paramètres généraux de la fenêtre. */
#define WINDOW_TITLE  "Mouvement Brownien"
#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 800
#define FPS            60

/**
 * @brief Traite les événements SDL.
 *
 * @param main_loop Pointeur vers le drapeau indiquant si la boucle doit continuer.
 */
void process_input(int *main_loop);

/**
 * @brief Initialise SDL, la fenêtre et le renderer.
 *
 * @param window Adresse du pointeur vers la fenêtre.
 * @param renderer Adresse du pointeur vers le renderer.
 * @return 1 en cas de succès, 0 en cas d'erreur.
 */
int initialize_window(SDL_Window **window, SDL_Renderer **renderer);

/**
 * @brief Libère la fenêtre, le renderer et SDL.
 *
 * @param window Pointeur vers la fenêtre.
 * @param renderer Pointeur vers le renderer.
 */
void destroy_window(SDL_Window **window, SDL_Renderer **renderer);

/**
 * @brief Affiche le contenu actuellement dessiné par le renderer.
 *
 * @param renderer Renderer SDL utilisé pour l'affichage.
 */
void show(SDL_Renderer *renderer);

#endif /* WINDOW_H */
