/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les fonctions de conversion de script.
* @date 14/07/2016
*/

#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

#include "gen.h"
#include <string>

/**
* @brief Convertit un fichier TXT en fichier MEN.
* @param fileName le nom du script à convertir.
* @throw PfException si une erreur survient lors de la conversion.
*
* Le fichier MEN portera le même nom que le fichier TXT, à l'extension près.
*
* Le chemin du fichier passé en paramètre doit être relatif au dossier SCRIPT_DIR (fichier "gen.h").
* Le fichier MEN créé aura le même chemin, mais à partir du dossier OBJ_DIR.
*/
void convertTxtToMen(const string& fileName);

#endif // SCRIPT_H_INCLUDED
