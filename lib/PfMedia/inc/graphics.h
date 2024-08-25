/**
* @file
* @author Anaïs Vernet
* @brief Fichier regroupant différentes fonctions de création graphiques.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Il s'agit ici principalement de fonctions mathématiques permettant la génération de textures procédurales (embryonaires).
*/

#ifndef GRAPHICS_H_INCLUDED
#define GRAPHICS_H_INCLUDED

#include "media_gen.h"

/**
* @brief Enumération des modes de génération de tableaux 2D de textures.
*/
enum TextureGenerationMode {GRAPHICS_FLAT, //!< Texture unie.
							GRAPHICS_LINEAR, //!< Texture pyramidale.
							GRAPHICS_DOME, //!< Texture parabolique.
							GRAPHICS_RANDOM, //!< Texture entièrement aléatoire.
							GRAPHICS_NOISE //!< Texture de bruit cohérent.
							};

/**
* @brief Retourne un tableau 2D de valeurs comprises entre -1 et 1.
* @param r Le nombre de lignes du tableau.
* @param c Le nombre de colonnes du tableau.
* @param mode Le mode de génération de texture.
* @param v1 Une valeur d'initialisation.
* @param v2 Une valeur d'initialisation.
* @param v3 Une valeur d'initialisation.
* @throw ArgumentException si l'une des coordonnées est nulle.
*
* Les textures suivantes (tableau t[r][c]) sont retournées :
* <ul><li>GRAPHICS_FLAT : t[i][j] = v1</li>
* <li>GRAPHICS_LINEAR : t[i][j] = (v1 - v2*|i-r/2| + v1 - v2*|j-c/2|)/2</li>
* <li>GRAPHICS_DOME : t[i][j] = (v1 - v2*(i-r/2)² + v1 - v2*(j-c/2)²)/2</li>
* <li>GRAPHICS_RANDOM : t[i][j] = rand()/RAND_MAX*v1</li>
* <li>GRAPHICS_NOISE : t[i][j] = pfNoiseFct(i,j,v2)</li></ul>
*
* Dans le cas de GRAPHICS_NOISE, la foncton <em>init2DNoise</em> est auparavant
* appelée, avec pour arguments (w=r, h=c, seed=v3, max=v1).
*
* @warning
* De la mémoire est allouée pour le tableau retourné.
*/
float** generate2DTexture(unsigned int r, unsigned int c, TextureGenerationMode mode, float v1 = 0, float v2 = 0, float v3 = 0);

/**
* @brief Retourne une valeur de bruit cohérent 2D.
* @param x L'abscisse.
* @param y L'ordonnée.
* @param step Le pas du bruit.
* @return La valeur calculée.
* @throw ArgumentException si le pas est nul.
*
* Si x ou y est négatif, sa valeur absolue est prise.
*/
float pfNoiseFct(float x, float y, unsigned int step);

/**
* @brief Retourne un nombre pseudo-aléatoire compris entre 0 et <em>max</em>.
* @param seed La graine de hasard.
* @param max La valeur maximale admissible.
* @return Une valeur pseudo-aléatoire.
* @throw ArgumentException si la graine est strictement négative ou si le maximum est négatif ou nul.
*
* La même valeur est retournée pour une valeur <em>seed</em> donnée.
*
* @warning
* Méthode abandonnée.
*/
int pfRand(int seed, int max);

/**
* @brief Retourne l'interpolation linéaire 1D du point <em>x</em>.
* @param a Le point précédent.
* @param b Le point suivant.
* @param x Le point à interpoler.
* @return La valeur interpolée.
*/
float linearInterpolation(float a, float b, float x);

/**
* @brief Retourne l'interpolation linéaire 2D du point (<em>x</em>;<em>y</em>).
* @param a1 Le point (0;0).
* @param b1 Le point (0;1).
* @param a2 Le point (1;0).
* @param b2 Le point (1;1).
* @param x L'abscisse du point à interpoler.
* @param y L'ordonnée du point à interpoler.
* @return La valeur interpolée.
*/
float linearInterpolation2D(float a1, float b1, float a2, float b2, float x, float y);

/**
* @brief Retourne l'interpolation cubique 1D du point <em>x</em>.
* @param y0 Le point précédent <em>y1</em>.
* @param y1 Le point précédent <em>x</em>.
* @param y2 Le point suivant <em>x</em>.
* @param y3 Le point suivant <em>y2</em>.
* @param x Le point à interpoler.
* @return La valeur interpolée.
*/
float cubicInterpolation(float y0, float y1, float y2, float y3, float x);

/**
* @brief Retourne l'interpolation cubique 2D du point (<em>x</em>;<em>y</em>).
* @param y00 Le point (0;0).
* @param y01 Le point (0;1).
* @param y02 Le point (0;2).
* @param y03 Le point (0;3).
* @param y10 Le point (1;0).
* @param y11 Le point (1;1).
* @param y12 Le point (1;2).
* @param y13 Le point (1;3).
* @param y20 Le point (2;0).
* @param y21 Le point (2;1).
* @param y22 Le point (2;2).
* @param y23 Le point (2;3).
* @param y30 Le point (3;0).
* @param y31 Le point (3;1).
* @param y32 Le point (3;2).
* @param y33 Le point (3;3).
* @param x L'abscisse du point à interpoler.
* @param y L'ordonnée du point à interpoler.
* @return La valeur interpolée.
*
* avec
*
* <code>y-0 < y-1 < x < y-2 < y-3</code>
*
* et
*
* <code>y0- < y1- < y < y2- < y3-</code>
*/
float cubicInterpolation2D(float y00, float y01, float y02, float y03, float y10, float y11, float y12, float y13,
						float y20, float y21, float y22, float y23, float y30,float y31, float y32, float y33, float x, float y);

/**
* @brief Génère un bruit 2D.
* @param w La largeur du tableau 2D.
* @param h La hauteur du tableau 2D.
* @param seed La graine de hasard.
* @param max La valeur maximale admissible.
* @throw ArgumentException si l'une des dimensions est nulle.
* @throw ArgumentException si la graine est strictement négative ou si le maximum est négatif ou nul.
*
* Modifie la variable globale <em>g_noise2D_v2</em>.
*/
void init2DNoise(unsigned int w, unsigned int h, int seed, int max);

#endif // GRAPHICS_H_INCLUDED
