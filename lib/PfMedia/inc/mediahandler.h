/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les fonctions d'initialisation et de fermeture des bibliothèques media utilisées ici.
* @date xx/xx/xxxx
* @version 0.0.0
*
* Le bon usage des fonctions de ce fichier est tout simplement d'appeler <em>initEverything</em> en début de programme
* et <em>closeEverything</em> en fin de programme.
* Les bibliothèques SDL, OpenGL et FMOD seront ainsi correctement initialisées puis fermées.
*/

#ifndef MEDIAHANDLER_H_INCLUDED
#define MEDIAHANDLER_H_INCLUDED

#include "media_gen.h"

#include <string>

/**
* @brief Initialise tous les contextes nécessaires.
* @param appName Le nom de l'application, à afficher en titre de la fenêtre.
* @throw PfException si une erreur survient.
*
* SDL, OpenGL et FMOD.
*/
void initEverything(const string& appName);

/**
* @brief Ferme les contextes ouverts.
* @throw PfException si une erreur survient lors d'une fermeture.
*
* SDL, OpenGL et FMOD sont fermés si nécessaire.
*
* Les actions effectuées sont les suivantes :
*
* <ul><li>Si la SDL est initialisée, appel de la fonction <em>SDL_Quit</em>.</li>
* <li>Appel de la fonction <em>freeTextures</em> (fichier "glfunc.h").</li>
* <li>Si FMOD est initialisé, appel de la fonction <em>closeFMOD</em> (fichier "fmodfunc.h").</li>
* <li>Si la macro NDEBUG n'est pas définie et si le fichier <em>g_log</em> est ouvert, celui-ci est fermé.</li></ul>
*/
void closeEverything();

#endif // MEDIAHANDLER_H_INCLUDED
