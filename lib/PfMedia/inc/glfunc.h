/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les fonctions gérant l'affichage OpenGL.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Les deux variables globales déclarées ici, <em>gp_mainScreen</em> et <em>gp_renderer</em> sont utilisées par les fonctions de ce fichier
* mais également gérées par les fonctions du fichier "mediahandler.h".
*
* Les fonctions de ce fichier permettent la manipulation du contexte OpenGL (affichage d'images notamment).
*
* Les images texturées se réfèrent à une map d'images référencées par des numéros (unsigned int) pour connaître leurs textures.
* Cette map, définie dans le fichier "glfunc.cpp" et entièrement gérée en interne, a pour clés les valeurs assignées à
* chaque nouvel ajout de texture par les fonctions <em>addTexture</em> de ce fichier, et pour valeurs correspondantes les indices OpenGL de chaque texture
* (les noms OpenGL).
* Les noms OpenGL ne sont donc jamais utilisés en dehors des fonctions internes à ce fichier, c'est toujours à l'indice de texture qu'on se réfère
* dans le reste du programme.
*
* Pour mieux comprendre le principe de rendu d'une image texturée, se référer à la documentation de la fonction <em>drawGL</em> de ce fichier.
*
* @see addTexture, drawGL
*/

#ifndef GLFUNC_H_INCLUDED
#define GLFUNC_H_INCLUDED

#include "media_gen.h"

#include <GL/gl.h>
#include <SDL.h>
#include <string>
#include <fstream>
#include "geometry.h"

class GLImage;

/**
* @brief Pointeur vers l'écran principal SDL.
*/
extern SDL_Window* gp_mainScreen;
/**
* @brief Pointeur vers le moteur de rendu SDL.
*/
extern SDL_Renderer* gp_renderer;

/**
* @brief Initialise le contexte OpenGL.
*/
void initGL();
/**
* @brief Redimensionne le contexte OpenGL.
* @param width La largeur du contexte.
* @param height La hauteur du contexte.
*/
void resizeGL(GLsizei width, GLsizei height);
/**
* @brief Modifie le mode d'affichage.
* @param flags les flags SDL.
* @param width La nouvelle largeur de la fenêtre, 0 pour ne pas la modifier, -1 pour les dimensions plein écran.
* @param height La nouvelle hauteur de la fenêtre, 0 pour ne pas la modifier, -1 pour les dimensions plein écran.
* @throw PfException si une erreur SDL survient.
*
* @remarks
* Si l'un des paramètres <em>width</em> et <em>height</em> est nul, la fenêtre n'est pas redimensionnée, les dimensions sont conservées en l'état.
* Si l'un d'eux est négatif et que le mode plein écran est demandé, alors les dimensions sont celles de l'écran.
*
* @warning
* Des paramètres négatifs avec un mode fenêtré peuvent mener à un résultat inattendu.
*/
void changeSDLMode(uint32_t flags, GLsizei width = 0, GLsizei height = 0);
/**
* @brief Efface les tampons OpenGL.
*/
void clearGL();
/**
* @brief Appelle la fonction <em>glFlush</em>.
*/
void flushGL();
/**
* @brief Appelle la fonction <em>SDL_SwapWindow</em>.
*/
void swapSDLBuffers();
/**
* @brief Modifie les coordonnées de la caméra.
* @param x La nouvelle abscisse de la caméra.
* @param y La nouvelle ordonnée de la caméra.
*
* Les coordonnées sont données en pourcentage de la largeur de l'écran, et la macro Y_X_RATIO est utilisé pour déterminer le déplacement selon Y.
*/
void translateCamera(float x, float y);
/**
* @brief Génère une texture OpenGL à partir d'un fichier PNG.
* @param fileName Le nom du fichier PNG à lire.
* @param textureIndex L'indice de la texture à ajouter.
* @throw PfException si le fichier ne peut pas être ouvert.
*
* L'indice de texture passé en paramètre doit être celui qui sera utilisé par ce programme par la suite comme identificateur de la texture.
*
* La map des indices de textures gérée en interne dans le fichier "glfunc.cpp" est alors mise à jour en associant le nom OpenGL généré
* à cet indice de texture.
*
* @warning
* Si l'indice de texture passé en paramètre n'est pas disponible (déjà associé), alors rien n'est fait.
*/
void addTexture(const string& fileName, unsigned int textureIndex);
/**
* @brief Génère une texture OpenGL à partir d'un fichier contenant une image PNG.
* @param r_ifs Une référence vers le fichier à lire.
* @param textureIndex L'indice de la texture à ajouter.
* @throw PfException si le flux ne présente pas suffisamment d'octets à lire.
*
* Les quatre premiers octets indiquent le nombre d'octets du fichier PNG.
* Autant d'octets sont ensuite lus et considérés comme une image PNG chargée au moyen d'un PNGToGLLoader.
*
* Le flux doit être ouvert en binaire à l'endroit souhaité et n'est pas rembobiné en fin de fonction.
*
* L'indice de texture passé en paramètre doit être celui qui sera utilisé par ce programme par la suite comme identificateur de la texture.
*
* La map des indices de textures gérée en interne dans le fichier "glfunc.cpp" est alors mise à jour en associant le nom OpenGL généré
* à cet indice de texture.
*
* @warning
* Si l'indice de texture passé en paramètre est déjà utilisé, alors rien n'est fait.
* Le flux en lecture est alors déplacé à la fin de la portion "PNG" qui aurait été lue.
*/
void addTexture(ifstream& r_ifs, unsigned int textureIndex);
/**
* @brief Lie la texture dont l'indice est passé en paramètre au contexte OpenGL.
* @param textureIndex L'indice de la texture.
* @throw ArgumentException si l'indice passé en paramètre ne correspond à aucune texture.
*
* C'est cette fonction et elle seule qui utilise la map interne au fichier "glfunc.cpp" pour associer une nouvelle texture OpenGL au contexte
* à partir de son nom OpenGL, associé à l'indice de texture passé en paramètre.
*/
void bindTexture(unsigned int textureIndex);
/**
* @brief Rend sous OpenGL l'image passée en paramètre.
* @param rc_glImage L'image à rendre.
*
* Si l'image n'est pas valide, alors rien n'est fait.
*
* Si l'image est statique, alors elle n'est pas affectée par la position de la caméra et ses coordonnées sont exprimées selon un repère absolu
* dont 0 et 1 sont les bornes des dimensions de l'écran.
*
* Une image avec un angle absolu de plus de 1° sera soumise à la rotation correspondante.
*
* Si l'image est texturée, alors la fonction <em>bindTexture</em> est appelée pour charger la texture correspondante dans le contexte.
*
* @warning
* Une exception peut être levée par <em>bindTexture</em> si la texture n'a pas été préalablement chargée (si l'indice de l'image n'est pas associé
* à un nom OpenGL).
*/
void drawGL(const GLImage& rc_glImage);
/**
* @brief Affiche la transition en cours.
*
* Affiche la transition à la frame correspondant à la variable globale <em>g_transitionFrame</em> (fichier "media_gen.h").
*
* Actuellement, la transition est un écran noir totalement transparent à la frame 0 et totalement opaque à la frame TRANSITION_FRAMES_COUNT.
*/
void drawTransition();
/**
* @brief Libère la mémoire allouée pour les textures.
*
* Cette méthode appelle la fonction <em>glDeleteTextures</em> sur les textures de la map gérée en interne du fichier "glfunc.cpp".
* La map est alors vidée.
*/
void freeTextures();
/**
* @brief Retourne un point de coordonnées comprises entre 0.0 et 1.0 (point OpenGL) depuis un point SDL (coordonnées de souris).
* @param x L'abscisse en pixels SDL du point.
* @param y L'ordonnée en pixels SDL du point.
* @param coordRelativeToBorders <code>true</code> si les coordonnées retournées excluent les barres latérales de l'écran
* (SYSTEM_BORDER_WIDTH, fichier "media_gen.h").
*
* @warning
* Cette méthode n'est valable que pour un écran de largeur supérieure à sa hauteur.
*/
PfPoint glCoordFromSDLPoint(int x, int y, bool coordRelativeToBorders = true);
/**
* @brief Retourne un point de coordonnées comprises entre 0.0 et 1.0 (point OpenGL) depuis un point SDL (coordonnées de souris).
* @param rc_point Le point contenant les coordonnées en pixels SDL du point.
* @param coordRelativeToBorders <code>true</code> si les coordonnées retournées excluent les barres latérales de l'écran
* (SYSTEM_BORDER_WIDTH, fichier "media_gen.h").
*
* @warning
* Cette méthode n'est valable que pour un écran de largeur supérieure à sa hauteur.
*/
PfPoint glCoordFromSDLPoint(const PfPoint& rc_point, bool coordRelativeToBorders = true);

#endif // GLFUNC_H_INCLUDED
