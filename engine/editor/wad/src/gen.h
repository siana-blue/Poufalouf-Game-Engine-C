/**
* @file
* @author Anaïs Vernet
* @brief Header inclus dans tous les fichiers header de ce programme ainsi que dans le fichier "main.cpp".
* @version 0.0.2
* @date 13/12/2015
*
* Ce fichier définit les macros générales, dont NDEBUG qui doit être défini avant la release.
*
* Il contient la ligne :
*
* <code>using namespace std;</code>
*
* qui n'est plus à répéter dans les autres fichiers.
*
* Il déclare la variable globale <em>g_log</em> de type ofstream qui est initialisée dans le fichier "main.cpp".
*
* Ce fichier ne fait aucune inclusion mis à part <fstream> qui n'est pas à réinclure dans les autres fichiers.
*/

#ifndef GEN_H_INCLUDED
#define GEN_H_INCLUDED

#include "misc_gen.h"

#include <fstream>

#define SCRIPT_DIR "scripts" //!< Le nom du répertoire contenant les fichiers scripts.
#define OBJ_DIR "wad" //!< Le nom du répertoire contenant les fichiers objets ".wad".

#endif // GEN_H_INCLUDED
