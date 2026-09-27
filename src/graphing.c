/**
 * @file graphing.c
 * @brief Implémentation des fonctions de dessin des graphiques SDL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL_ttf.h>

#include "graphing.h"
#include "window.h"

/**
 * @brief Dessine une ligne horizontale ou verticale.
 *
 * SDL_RenderDrawLine ne permet pas directement de choisir une épaisseur.
 * On dessine donc un rectangle fin autour de la ligne demandée.
 */
void draw_line(SDL_Renderer *renderer, int x1, int y1,
               int x2, int y2, int thickness)
{
    SDL_Rect rect;

    if (y1 == y2) {
        /* Ligne horizontale : la largeur correspond à la longueur de la ligne. */
        rect.x = (x1 < x2) ? x1 : x2;
        rect.y = y1 - thickness / 2;
        rect.w = abs(x2 - x1);
        rect.h = thickness;
    } else if (x1 == x2) {
        /* Ligne verticale : la hauteur correspond à la longueur de la ligne. */
        rect.x = x1 - thickness / 2;
        rect.y = (y1 < y2) ? y1 : y2;
        rect.w = thickness;
        rect.h = abs(y2 - y1);
    } else {
        /* Cette fonction est prévue pour les lignes horizontales/verticales. */
        SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
        return;
    }

    SDL_RenderFillRect(renderer, &rect);
}

/**
 * @brief Convertit une abscisse mathématique en pixel.
 */
int toGraphCoordsX(const graph *graphique, precision x)
{
    return (int)(((x - graphique->x_min) * WINDOW_WIDTH)
                 / (graphique->x_max - graphique->x_min));
}

/**
 * @brief Convertit une ordonnée mathématique en pixel.
 *
 * L'axe Y de SDL est inversé : 0 se trouve en haut de l'écran.
 */
int toGraphCoordsY(const graph *graphique, precision y)
{
    return WINDOW_HEIGHT
           - (int)(((y - graphique->y_min) * WINDOW_HEIGHT)
                   / (graphique->y_max - graphique->y_min));
}

/**
 * @brief Convertit un point mathématique en coordonnées écran.
 */
SDL_Point toGraphCoords(const graph *graphique, precision x, precision y)
{
    SDL_Point point = {
        toGraphCoordsX(graphique, x),
        toGraphCoordsY(graphique, y)
    };

    return point;
}

/**
 * @brief Dessine une graduation sur l'un des axes.
 *
 * orientation :
 * - 'v' : graduation verticale sur l'axe X ;
 * - 'h' : graduation horizontale sur l'axe Y.
 *
 * main_stroke :
 * - 'y' : graduation principale ;
 * - 'n' : graduation secondaire.
 */
void stroke_plot_lines(SDL_Renderer *renderer, const graph *graphique,
                       int position, char orientation, char main_stroke)
{
    const int is_main = (main_stroke == 'y');
    const int length = is_main ? MAIN_STROKE_LENGTH : SECOND_STROKE_LENGTH;
    const int thickness =
        is_main ? MAIN_STROKE_THICKNESS : SECOND_STROKE_THICKNESS;

    if (orientation == 'v') {
        draw_line(renderer,
                  position,
                  graphique->y_origin - length / 2,
                  position,
                  graphique->y_origin + length / 2,
                  thickness);
    } else {
        draw_line(renderer,
                  graphique->x_origin - length / 2,
                  position,
                  graphique->x_origin + length / 2,
                  position,
                  thickness);
    }
}

/**
 * @brief Dessine un point ou une croix.
 *
 * plot_style == 0 : croix.
 * plot_style != 0 : petit point.
 */
void plot(SDL_Renderer *renderer, const graph *graphique,
          precision x, precision y, short int plot_style)
{
    const SDL_Point point = toGraphCoords(graphique, x, y);

    if (plot_style != 0) {
        draw_line(renderer,
                  point.x,
                  point.y - POINT_THICKNESS / 2,
                  point.x,
                  point.y + POINT_THICKNESS / 2,
                  POINT_THICKNESS);
    } else {
        draw_line(renderer,
                  point.x - CROSS_SIZE / 2,
                  point.y,
                  point.x + CROSS_SIZE / 2,
                  point.y,
                  CROSS_THICKNESS);

        draw_line(renderer,
                  point.x,
                  point.y - CROSS_SIZE / 2,
                  point.x,
                  point.y + CROSS_SIZE / 2,
                  CROSS_THICKNESS);
    }
}

/**
 * @brief Dessine une courbe à partir d'un tableau de valeurs Y.
 */
void plot_line(SDL_Renderer *renderer, const graph *graphique,
               const precision *points_y, int points_x_spread,
               int nb_points)
{
    SDL_Point *points;

    if (nb_points <= 0) {
        return;
    }

    points = malloc((size_t)nb_points * sizeof(*points));
    if (points == NULL) {
        fprintf(stderr, "Erreur d'allocation des points du graphique.\n");
        return;
    }

    for (int i = 0; i < nb_points; ++i) {
        points[i].x =
            toGraphCoordsX(graphique, i * points_x_spread);
        points[i].y =
            toGraphCoordsY(graphique, points_y[i]);
    }

    SDL_RenderDrawLines(renderer, points, nb_points);
    free(points);
}

/**
 * @brief Dessine une étiquette numérique sur le graphique.
 */
static void draw_label(SDL_Renderer *renderer, TTF_Font *font,
                       const char *text, int x, int y)
{
    SDL_Color black = {0, 0, 0, SDL_ALPHA_OPAQUE};
    SDL_Surface *surface;
    SDL_Texture *texture;
    SDL_Rect destination;

    surface = TTF_RenderUTF8_Solid(font, text, black);
    if (surface == NULL) {
        return;
    }

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == NULL) {
        SDL_FreeSurface(surface);
        return;
    }

    destination.x = x;
    destination.y = y;
    destination.w = NUMBERS_SIZE * (int)strlen(text) / 2;
    destination.h = NUMBERS_SIZE;

    SDL_RenderCopy(renderer, texture, NULL, &destination);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

/**
 * @brief Dessine toutes les graduations d'un axe.
 *
 * Cette fonction remplace les quatre boucles presque identiques de
 * l'ancienne version.
 */
static void draw_axis_graduations(SDL_Renderer *renderer,
                                  graph *graphique,
                                  TTF_Font *font,
                                  precision interval,
                                  int sublines,
                                  char axis)
{
    const int is_x_axis = (axis == 'x');

    /* Parcours du côté positif de l'axe. */
    for (precision value = interval;
         value < (is_x_axis ? graphique->x_max : graphique->y_max);
         value += interval) {

        const int position = is_x_axis
            ? toGraphCoordsX(graphique, value)
            : toGraphCoordsY(graphique, value);

        stroke_plot_lines(renderer, graphique, position,
                          is_x_axis ? 'v' : 'h', 'y');

        char label[32];
        snprintf(label, sizeof(label), "%.0f", value);

        if (is_x_axis) {
            draw_label(renderer, font, label,
                       position - (int)strlen(label) * NUMBERS_SIZE / 4,
                       graphique->y_origin + 20);
        } else {
            draw_label(renderer, font, label,
                       graphique->x_origin - 60,
                       position - NUMBERS_SIZE / 2);
        }

        /* Ajout des petites graduations entre deux graduations principales. */
        for (int j = 0; j < sublines; ++j) {
            const precision offset =
                (j + 1) * interval / (sublines + 1);

            const precision sub_value = value - offset;
            const int sub_position = is_x_axis
                ? toGraphCoordsX(graphique, sub_value)
                : toGraphCoordsY(graphique, sub_value);

            stroke_plot_lines(renderer, graphique, sub_position,
                              is_x_axis ? 'v' : 'h', 'n');
        }
    }

    /* Parcours du côté négatif de l'axe. */
    for (precision value = -interval;
         value > (is_x_axis ? graphique->x_min : graphique->y_min);
         value -= interval) {

        const int position = is_x_axis
            ? toGraphCoordsX(graphique, value)
            : toGraphCoordsY(graphique, value);

        stroke_plot_lines(renderer, graphique, position,
                          is_x_axis ? 'v' : 'h', 'y');

        char label[32];
        snprintf(label, sizeof(label), "%.0f", value);

        if (is_x_axis) {
            draw_label(renderer, font, label,
                       position - (int)strlen(label) * NUMBERS_SIZE / 4,
                       graphique->y_origin + 20);
        } else {
            draw_label(renderer, font, label,
                       graphique->x_origin - 60,
                       position - NUMBERS_SIZE / 2);
        }

        /* Ajout des petites graduations entre deux graduations principales. */
        for (int j = 0; j < sublines; ++j) {
            const precision offset =
                (j + 1) * interval / (sublines + 1);

            const precision sub_value = value + offset;
            const int sub_position = is_x_axis
                ? toGraphCoordsX(graphique, sub_value)
                : toGraphCoordsY(graphique, sub_value);

            stroke_plot_lines(renderer, graphique, sub_position,
                              is_x_axis ? 'v' : 'h', 'n');
        }
    }
}

/**
 * @brief Crée un repère et dessine ses axes et ses graduations.
 */
graph *create_graph(SDL_Renderer *renderer,
                    precision x_min, precision x_max,
                    precision y_min, precision y_max,
                    precision graduation_interval_x, int nb_sublines_x,
                    precision graduation_interval_y, int nb_sublines_y)
{
    graph *graphique;
    TTF_Font *font;

    graphique = malloc(sizeof(*graphique));
    if (graphique == NULL) {
        fprintf(stderr, "Erreur d'allocation du graphique.\n");
        return NULL;
    }

    graphique->x_min = x_min;
    graphique->x_max = x_max;
    graphique->y_min = y_min;
    graphique->y_max = y_max;

    /* Conversion de l'origine mathématique en coordonnées SDL. */
    graphique->x_origin = toGraphCoordsX(graphique, 0.0);
    graphique->y_origin = toGraphCoordsY(graphique, 0.0);

    font = TTF_OpenFont("font/Arial.ttf", 50);
    if (font == NULL) {
        fprintf(stderr, "Erreur lors du chargement de la police : %s\n",
                TTF_GetError());
        free(graphique);
        return NULL;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    /* Dessin des deux axes du repère. */
    draw_line(renderer, graphique->x_origin, 0,
              graphique->x_origin, WINDOW_HEIGHT, PEN_THICKNESS);
    draw_line(renderer, 0, graphique->y_origin,
              WINDOW_WIDTH, graphique->y_origin, PEN_THICKNESS);

    /* Dessin des graduations et de leurs valeurs. */
    draw_axis_graduations(renderer, graphique, font,
                          graduation_interval_x, nb_sublines_x, 'x');

    draw_axis_graduations(renderer, graphique, font,
                          graduation_interval_y, nb_sublines_y, 'y');

    TTF_CloseFont(font);

    return graphique;
}

/**
 * @brief Dessine une trajectoire 2D en reliant les positions successives.
 */
void plot_2D(SDL_Renderer *renderer, const graph *graphic,
             const coo *tab, int size)
{
    if (tab == NULL || size <= 0) {
        return;
    }

    for (int i = 1; i < size; ++i) {
        const SDL_Point previous =
            toGraphCoords(graphic, tab[i - 1].x, tab[i - 1].y);
        const SDL_Point current =
            toGraphCoords(graphic, tab[i].x, tab[i].y);

        SDL_RenderDrawLine(renderer,
                           previous.x, previous.y,
                           current.x, current.y);
    }
}

/**
 * @brief Dessine un histogramme simple.
 */
void draw_histogram(SDL_Renderer *renderer, const int *histogram, int size)
{
    int max_value = 0;

    if (histogram == NULL || size <= 0) {
        return;
    }

    /* Recherche de la valeur maximale pour normaliser les hauteurs. */
    for (int i = 0; i < size; ++i) {
        if (histogram[i] > max_value) {
            max_value = histogram[i];
        }
    }

    if (max_value == 0) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);

    for (int i = 0; i < size; ++i) {
        SDL_Rect bar = {
            i * WINDOW_WIDTH / size,
            WINDOW_HEIGHT - histogram[i] * WINDOW_HEIGHT / max_value,
            WINDOW_WIDTH / size,
            histogram[i] * WINDOW_HEIGHT / max_value
        };

        SDL_RenderFillRect(renderer, &bar);
    }
}
