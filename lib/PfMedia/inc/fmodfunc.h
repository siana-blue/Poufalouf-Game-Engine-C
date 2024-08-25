/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les fonctions de manipulation des sons via FMOD.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Pour utiliser la bibliothèque FMOD, commencer par appeler <em>initFMOD</em>.
*
* Les sons sont ensuite chargés en mémoire grâce à la fonction <em>addSound</em>.
* Un indice de son est alors associé au nom de fichier utilisé.
* La map globale <em>gp_sounds_map</em> intègre un pointeur sur FMOD_SOUND avec pour clé le nom de fichier.
* En parallèle, le vecteur global <em>g_sounds_v</em> ajoute le nom de fichier, sa position + 1 étant par la suite son indice de son.
*
* Il sera ensuite possible d'appeler ce son par la fonction <em>playSound</em>, en utilisant soit le nom de fichier directement,
* soit l'indice de son.
* Si la fonction <em>playSound</em> est appelée pour un son dont le nom de fichier ne correspond pas à une ressource chargée, la fonction <em>addSound</em>
* est automatiquement appelée. Cela ne fonctionne bien entendu que si l'argument de <em>playSound</em> est un nom de fichier et non un indice de son.
*
* Pour arrêter tous les sons en cours de lecture, utiliser la fonction <em>stopAllSounds</em>.
*
* Avant de quitter le programme, fermer proprement FMOD en utilisant la fonction <em>closeFMOD</em>, qui elle-même appelle au préalable <em>freeSounds</em>.
*/

#ifndef FMODFUNC_H_INCLUDED
#define FMODFUNC_H_INCLUDED

#include "media_gen.h"

#include <string>

/**
* @brief Initialise FMOD.
* @throw PfException si l'initialisation échoue.
*/
void initFMOD();

/**
* @brief Ajoute un son dans la map des sons disponibles.
* @param soundFileName Le nom du fichier son.
* @param music <code>true</code> si le son est une musique, <code>false</code> pour un son court.
* @return L'indice du son dans le vecteur global <em>g_sounds_v</em>, base 1 (cohérence avec <em>playSound</em>).
* @throw PfException si le son ne peut pas être ajouté ou s'il en existe déjà un du même nom et qu'une erreur survient lors de sa suppression.
*
* Si le nom de fichier n'était pas encore associé à un son, alors un nouvel indice de son est associé à celui-ci.
* Sinon, le même indice est conservé.
* Les indices de son, comme pour les textures, commencent à partir de 1.
*
* L'indice de son n'est pas stocké sous forme d'entier.
* Un vecteur de sons ajoutés est complété lors de l'ajout d'un nouveau son, tandis qu'il n'est pas modifié si un son portant le même nom de fichier
* est ajouté.
*
* Si un son avec le même nom de fichier est déjà présent, alors il est détruit et remplacé.
*/
unsigned int addSound(const string& soundFileName, bool music = false);

/**
* @brief Joue un son.
* @param soundFileName Le nom du fichier son.
* @param music <code>true</code> si le son est une musique, <code>false</code> pour un son court.
* @param loop Le nombre de répétitions de la musique.
* @throw PfException si le son ne peut pas être joué.
*
* Si le son n'est pas disponible, il est automatiquement ajouté au moyen de la fonction <em>addSound</em>.
*
* Par défaut, les musiques bouclent à l'infini (valeur de <em>loop</em> négative). La répétition ne fonctionne que pour les musiques.
*/
void playSound(const string& soundFileName, bool music = false, int loop = -1);

/**
* @brief Joue un son.
* @param soundIndex L'indice du fichier son, dont le premier est 1.
* @param music <code>true</code> si le son est une musique, <code>false</code> pour un son court.
* @param loop Le nombre de répétitions de la musique.
* @throw PfException si le son ne peut pas être joué.
*
* Si l'indice n'existe pas, alors rien n'est fait.
*
* L'indice de son est en réalité l'indice dans le vecteur de sons ajoutés <em>g_sounds_v</em> + 1.
*
* Par défaut, les musiques bouclent à l'infini (valeur de <em>loop</em> négative). La répétition ne fonctionne que pour les musiques.
*/
void playSound(unsigned int soundIndex, bool music = false, int loop = -1);

/**
* @brief Arrête tous les sons en cours.
*/
void stopAllSounds();

/**
* @brief Libère tous les sons chargés de la mémoire.
* @throw PfException si une erreur FMOD survient.
*/
void freeSounds();

/**
* @brief Ferme FMOD.
* @throw PfException si une erreur de fermeture survient.
*
* Les sons chargés sont libérés par appel à la méthode <em>freeSounds</em>.
*/
void closeFMOD();

#endif // FMODFUNC_H_INCLUDED
