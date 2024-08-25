/**
* @file
* @author Anaïs Vernet
* @brief Fichier contenant les fonctions de conversion de script.
* @version 0.0.0
* @date 15/06/2015
*/

#ifndef SCRIPT_H_INCLUDED
#define SCRIPT_H_INCLUDED

#include "gen.h"
#include <string>

/**
* @brief Convertit un fichier txt en fichier tex.
* @param fileName le nom du script à convertir.
* @throw PfException si une erreur survient lors de la conversion.
*
* Le fichier tex portera le même nom que le fichier txt, à l'extension près.
*/
void convertTxtToTex(const string& fileName);

#endif // SCRIPT_H_INCLUDED
