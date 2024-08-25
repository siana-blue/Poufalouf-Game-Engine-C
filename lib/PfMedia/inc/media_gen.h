/**
* @file
* @author Anaïs Vernet
* @brief Header inclus dans tous les fichiers header de cette bibliothèque.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Les propriétés de l'affichage SDL/OpenGL dépendent du système d'exploitation. Celui-ci est défini via les macros SYSTEM_WINDOWS et SYSTEM_LINUX
* dans les fichiers headers de la bibliothèque PfMisc.
*
* Sont ainsi définies :
* <ul><li>Les dimensions de l'écran, SYSTEM_WINDOW_WIDTH et SYSTEM_WINDOW_HEIGHT, ces valeurs n'étant plus utilisées actuellement,</li>
* <li>La largeur des bordures de l'écran, en pourcentage de la largeur totale, SYSTEM_BORDER_WIDTH,</li>
* <li>Les flags SDL, dont est déduit l'état d'activation du plein écran, respectivement SYSTEM_SDL_FLAGS et SYSTEM_FULLSCREEN.</li></ul>
*/

/**
* @mainpage
*
* <b>Bibliothèque permettant la gestion d'OpenGL, de SDL et de FMOD, et offrant une suite d'outils graphiques</b>
*
* @warning
* Cette bibliothèque dépend de la bibliothèque PfMisc.
*
* Cette bibliothèque regroupe différentes fonctionnalités organisées comme suit :
* <ul><li>Le fichier "mediahandler.h" initialise tous les contextes nécessaires à une application OpenGL/SDL/FMOD standard,</li>
* <li>Le fichier "glfunc.h" regroupe les fonctions permettant l'utilisation de SDL et OpengGL,</li>
* <li>Le fichier "pngtoglloader.h" offre un outil pour l'exploitation d'images PNG,</li>
* <li>Le fichier "fmodfunc.h" permet la gestion de l'audio via FMOD,</li>
* <li>Le fichier "geometry.h" gère les opérations sur des formes via différentes classes : PfPoint, PfPolygon, PfRectangle, PfOrientation et PfColor,</li>
* <li>Le fichier "graphics.h" permet la génération de textures procédurales.</li></ul>
*
* La classe GLImage définit une image telle qu'elle est affichée à l'écran par les fonctions de cette bibliothèque.
*
* Enfin, la classe EventHandler gère le traitement des actions de l'utilisateur.
*
* Pour rappel, les coordonnées sous OpenGL sont exprimées en pourcentage de l'espace total d'affichage.
* Le point de coordonnées (0;0) est situé en bas à gauche de l'écran.
*
* L'écran étant rectangulaire et non carré, une seule valeur ne suffit pas pour dessiner un carré à l'écran.
* En effet, une forme de dimensions 0.25 et 0.25 occupera un quart de la largeur de l'écran et un quart de sa hauteur. Il s'agira donc d'un rectangle.
* La macro Y_X_RATIO (fichier "media_gen.h") permet de simplifier l'affichage d'un carré. Elle est automatiquement calculée à la création de la fenêtre.
* Pour dessiner un carré, on utilisera alors par exemple les dimensions 0.25 et 0.25/Y_X_RATIO.
*
* @warning
* Avant d'utiliser les fonctions SDL dans un programme, inclure la macro SDL_MAIN_HANDLED, autrement : erreur ld undefined reference to WinMain@16.
*
* @see
* media_gen.h, mediahandler.h, glfunc.h, pngtoglloader.h, fmodfunc.h, geometry.h, graphics.h
*/

#ifndef MEDIA_GEN_H_INCLUDED
#define MEDIA_GEN_H_INCLUDED

#include "misc_gen.h"

#ifdef SYSTEM_LINUX
#define SYSTEM_SDL_FLAGS SDL_WINDOW_OPENGL
#define SYSTEM_WINDOW_WIDTH 0
#define SYSTEM_WINDOW_HEIGHT 0
#define SYSTEM_BORDER_WIDTH 0.175
#else
#define SYSTEM_SDL_FLAGS SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL //!< Les flags utilisés pour la création du contexte SDL.
//#define SYSTEM_SDL_FLAGS SDL_WINDOW_OPENGL
#define SYSTEM_WINDOW_WIDTH 800 //!< La largeur de la fenêtre, en pixels.
#define SYSTEM_WINDOW_HEIGHT 600 //!< La hauteur de la fenêtre, en pixels.
#define SYSTEM_BORDER_WIDTH ((g_windowWidth-g_windowHeight)/2.0/g_windowWidth) //!< La largeur des bordures, en pourcentage de la largeur de la fenêtre.
#endif
#define SYSTEM_FULLSCREEN ((SYSTEM_SDL_FLAGS) & SDL_WINDOW_FULLSCREEN_DESKTOP) //!< Indique si le plein écran est activé lors de la création du contexte SDL.

#define MAX_VERTICES_PER_POLYGON 10 //!< Le nombre maximal de sommets dans un polygone.
#define TRANSITION_FRAMES_COUNT 8 //!< Le nombre de frames d'une transition.

#define Y_PIXEL_SIZE 1./g_windowHeight //!< La taille verticale d'un pixel.
#define Y_X_RATIO ((float) g_windowHeight/g_windowWidth) //!< Rapport entre la hauteur et la largeur de l'écran, utile pour dessiner des carrés sur un écran rectangulaire en utilisant un seul pourcentage.

/**
* @brief Largeur de l'écran.
*/
extern int g_windowWidth;
/**
* @brief Hauteur de l'écran.
*/
extern int g_windowHeight;
/**
* @brief Frame actuelle de la transition en cours.
*
* Lorsqu'un effet de transition est lancé, cette variable s'incrémente jusqu'à atteindre la valeur TRANSITION_FRAMES_COUNT.
* Elle repasse alors à 0.
*/
extern int g_transitionFrame;
/**
* @brief Indique si la souris est visible.
*/
extern bool g_showCursor;
#define SHOW_CURSOR() {g_showCursor = true; SDL_ShowCursor(true);} //!< Macro affichant le curseur SDL.
#define HIDE_CURSOR() {g_showCursor = false; SDL_ShowCursor(false);} //!< Macro masquant le curseur SDL.

#endif // MEDIA_GEN_H_INCLUDED
