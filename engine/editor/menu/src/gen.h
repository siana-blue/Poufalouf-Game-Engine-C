/**
* @file
* @author Ana�s Vernet
* @brief Header inclus dans tous les fichiers header de ce programme ainsi que dans le fichier "main.cpp".
* @date 13/12/2015
*
* Ce fichier d�finit les macros g�n�rales, dont NDEBUG qui doit �tre d�fini avant la release.
*
* Il contient la ligne :
*
* <code>using namespace std;</code>
*
* qui n'est plus � r�p�ter dans les autres fichiers.
*
* Il d�clare la variable globale <em>g_log</em> de type ofstream qui est initialis�e dans le fichier "main.cpp".
*
* Ce fichier ne fait aucune inclusion mis � part \<fstream\> qui n'est pas � r�inclure dans les autres fichiers.
*/

#ifndef GEN_H_INCLUDED
#define GEN_H_INCLUDED

#include "misc_gen.h"

#include <fstream>

#define SCRIPT_DIR "scripts" //!< Le nom du r�pertoire contenant les fichiers scripts.
#define OBJ_DIR "men" //!< Le nom du r�pertoire contenant les fichiers objets ".men".

#endif // GEN_H_INCLUDED
